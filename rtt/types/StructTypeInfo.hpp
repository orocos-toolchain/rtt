/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  StructTypeInfo.hpp

                        StructTypeInfo.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_STRUCT_TYPE_INFO_HPP
#define ORO_STRUCT_TYPE_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "PropertyDecomposition.hpp"
#include "type_discovery.hpp"
#include "MemberFactory.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * Type Information for data types that are structs, ie all user data types fall in this category.
         *
         * Use this class to register your data type to the Orocos type system.
         *
         * This class requires that your struct is compatible with the boost::serialization API.
         * This means that it must provide a serialize() function or that you define a free function
         * serialize() in the boost::serialization namespace. If no such function exists, you can
         * fall back to StdTypeInfo or even TemplateTypeInfo.
         */
        template<typename T, bool has_ostream = false>
        class StructTypeInfo: public TemplateTypeInfo<T, has_ostream>, public MemberFactory
        {
        public:
            StructTypeInfo(std::string name) :
                TemplateTypeInfo<T, has_ostream> (name)
            {
            }

        bool installTypeInfoObject(TypeInfo* ti) {
            // aquire a shared reference to the this object
            boost::shared_ptr< StructTypeInfo<T,has_ostream> > mthis = boost::dynamic_pointer_cast<StructTypeInfo<T,has_ostream> >( this->getSharedPtr() );
            assert(mthis);
            // Allow base to install first
            TemplateTypeInfo<T,has_ostream>::installTypeInfoObject(ti);
            // Install the factories for primitive types
            ti->setMemberFactory( mthis );

            // Don't delete us, we're memory-managed.
            return false;
        }

            virtual std::vector<std::string> getMemberNames() const {
                // only discover the part names of this struct:
                type_discovery in;
                T t; // boost can't work without a value.
                in.discover( t );
                return in.mnames;
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                // user tried to pass the member name by data source, but we can't read out this datasource after getMember() returns.
                // ie, we could only read out id as a string and then call the getMember below.
                // type_discovery requires the name right now and does not allow to delay the name, unless we discover the whole type,
                // keep all datasources and then use getMember using some functor data source.... Not going to do that !
                assert(false && "You're doing something new and exotic. Contact the Orocos-dev mailing list.");
                return base::DataSourceBase::shared_ptr();
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                typename internal::AssignableDataSource<T>::shared_ptr adata = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( item );
                // Use a copy in case our parent is not assignable:
                if ( !adata ) {
                    // is it non-assignable ?
                    typename internal::DataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< internal::DataSource<T> >( item );
                    if ( data ) {
                        // create a copy
                        adata = new internal::ValueDataSource<T>( data->get() );
                    }
                }
                if (adata) {
                    type_discovery in( adata );
                    return in.discoverMember( adata->set(), name );
                }
                log(Error) << "Wrong call to type info function " + this->getTypeName() << "'s getMember() can not process "<< item->getTypeName() <<endlog();
                return base::DataSourceBase::shared_ptr();
            }

            virtual bool getMember(internal::Reference* ref, base::DataSourceBase::shared_ptr item, const std::string& name) const {
                typename internal::AssignableDataSource<T>::shared_ptr adata = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( item );
                // Use a copy in case our parent is not assignable:
                if ( !adata ) {
                    // is it non-assignable ?
                    typename internal::DataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< internal::DataSource<T> >( item );
                    if ( data ) {
                        // create a copy -> this is the only place & case where we allocate -> how to fix ?
                        adata = new internal::ValueDataSource<T>( data->get() );
                    }
                }
                if (adata) {
                    type_discovery in( adata );
                    return in.referenceMember( ref, adata->set(), name );
                }
                log(Error) << "Wrong call to type info function " + this->getTypeName() << "'s getMember() can not process "<< item->getTypeName() <<endlog();
                return false;
            }

        virtual bool resize(base::DataSourceBase::shared_ptr arg, int size) const
            {
                return false;
            }


            /**
             * Implementation that updates result with the matching parts in source
             * Relies on the fact that getMember returns a C++ reference to each part of \a result, which is then updated
             * with a property found in source.
             */
            virtual bool composeTypeImpl(const PropertyBag& source,  typename internal::AssignableDataSource<T>::reference_t result) const {
                // The default implementation decomposes result and refreshes it with source.
                TypeInfoRepository::shared_ptr tir = Types();
                internal::ReferenceDataSource<T> rds(result);
                rds.ref(); // prevent dealloc.
                PropertyBag decomp;
                // only try refreshProperties if decomp's type is equal to source type.
                // update vs refresh: since it is intentional that the decomposition leads to references to parts of result,
                // only refreshProperties() is meaningful (ie we have a one-to-one mapping). In case of sequences, this would
                // of course not match, so this is struct specific.
                return typeDecomposition( &rds, decomp, false) && ( tir->type(decomp.getType()) == tir->type(source.getType()) ) && refreshProperties(decomp, source);
            }


        };
    }
}

#endif
