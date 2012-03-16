/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  CArrayTypeInfo.hpp

                        CArrayTypeInfo.hpp -  description
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


#ifndef ORO_TEMPLATE_CARRAY_INFO_HPP
#define ORO_TEMPLATE_CARRAY_INFO_HPP

#include "PrimitiveTypeInfo.hpp"
#include "../internal/ArrayPartDataSource.hpp"
#include <boost/lexical_cast.hpp>
#include "carray.hpp"
#include "../internal/carray.hpp"
#include "PropertyComposition.hpp"
#include "PropertyDecomposition.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * Template for data types that are C-style arrays. You can not use
         * this type as a port data type.
         *
         * C-style arrays are represented by the carray<T> wrapper
         * in the RTT type system.
         * @param T A carray<U> wrapper, where U is a C data type.
         */
        template<typename T, bool has_ostream = false>
        class CArrayTypeInfo: 
            public PrimitiveTypeInfo<T, has_ostream>, 
            public MemberFactory, public CompositionFactory
        {
        public:
            CArrayTypeInfo(std::string name) :
                PrimitiveTypeInfo<T, has_ostream> (name)
            {
            }

        bool installTypeInfoObject(TypeInfo* ti) {
            // aquire a shared reference to the this object
            boost::shared_ptr< CArrayTypeInfo<T> > mthis = boost::dynamic_pointer_cast<CArrayTypeInfo<T> >( this->getSharedPtr() );
            // Allow base to install first
            PrimitiveTypeInfo<T,has_ostream>::installTypeInfoObject(ti);
            // Install the factories for primitive types
            ti->setMemberFactory( mthis );
            ti->setCompositionFactory( mthis );

            // Don't delete us, we're memory-managed.
            return false;
        }

            virtual base::AttributeBase* buildVariable(std::string name,int sizehint) const
            {
                // There were two choices: create an empty carray, ie pointer-like behavior; OR create one with storage in the DS.
                // We need to redefine assignment in case of the latter, and make the storage dynamic, depending on sizehint.
                // pointer-like is dangerous due to non-tracking of reference-counts, so this is left for the default buildVariable
                // without a sizehint (using ValueDataSource), while the size hint version has storage.
                typename internal::ArrayDataSource<T>::shared_ptr ads = new internal::UnboundDataSource<internal::ArrayDataSource<T> >();
                ads->newArray( sizehint );
                return new Attribute<T>( name, ads.get() );
            }

            /* buildConstant() with sizehint is left out since it is identical to buildConstant() without sizehint.
               We make a shallow copy, so the size is automatically taken from the original expression the constant
               refers to. */

            virtual std::vector<std::string> getMemberNames() const {
                // only discover the parts of this struct:
                std::vector<std::string> result;
                result.push_back("size");
                result.push_back("capacity");
                return result;
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                using namespace internal;
                typename DataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< DataSource<T> >( item );

                // size and capacity can not change during program execution:
                if (name == "size" || name == "capacity") {
                    return new ConstantDataSource<int>( data->rvalue().count() );
                }

                typename AssignableDataSource<T>::shared_ptr adata = boost::dynamic_pointer_cast< AssignableDataSource<T> >( item );
                if ( !adata ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // contents of indx can change during program execution:
                try {
                    unsigned int indx = boost::lexical_cast<unsigned int>(name);
                    // @todo could also return a direct reference to item indx using another DS type that respects updated().
                    return new ArrayPartDataSource<typename T::value_type>( *adata->set().address(), new ConstantDataSource<unsigned int>(indx), item, data->rvalue().count() );
                } catch(...) {}
                log(Error) << "CArrayTypeInfo: No such part (or invalid index): " << name << endlog();
                return base::DataSourceBase::shared_ptr();
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                using namespace internal;
                typename DataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< DataSource<T> >( item );
                if ( !data ) {
                    return base::DataSourceBase::shared_ptr();
                }

                // discover if user gave us a part name or index:
                typename DataSource<std::string>::shared_ptr id_name = DataSource<std::string>::narrow( id.get() );
                if ( id_name ) {
                    // size and capacity can not change during program execution:
                    if (id_name->get() == "size" || id_name->get() == "capacity") {
                        return new ConstantDataSource<int>( data->rvalue().count() );
                    } else {
                        log(Error) << "CArrayTypeInfo: No such part : " << id_name->get() << endlog();
                        return base::DataSourceBase::shared_ptr();
                    }
                }

                typename AssignableDataSource<T>::shared_ptr adata = boost::dynamic_pointer_cast< AssignableDataSource<T> >( item );
                if ( !adata ) {
                    log(Error) << "CArrayTypeInfo: need assignable data type for indexing " << this->getTypeName() << endlog();
                    return base::DataSourceBase::shared_ptr();
                }

                typename DataSource<unsigned int>::shared_ptr id_indx = DataSource<unsigned int>::narrow( DataSourceTypeInfo<unsigned int>::getTypeInfo()->convert(id).get() );
                if ( id_indx ) {
                    return new ArrayPartDataSource<typename T::value_type>( *adata->set().address(), id_indx, item, data->rvalue().count() );
                }
                log(Error) << "CArrayTypeInfo: Invalid index) for type " << this->getTypeName() << endlog();
                return base::DataSourceBase::shared_ptr();
            }

            /**
             * Use getMember() for decomposition...
             */
            virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const
            {
                return base::DataSourceBase::shared_ptr();
            }

            virtual bool composeType( base::DataSourceBase::shared_ptr dssource, base::DataSourceBase::shared_ptr dsresult) const {
                const internal::DataSource<PropertyBag>* pb = dynamic_cast< const internal::DataSource<PropertyBag>* > (dssource.get() );
                if ( !pb )
                    return false;
                typename internal::AssignableDataSource<T>::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( dsresult );
                if ( !ads )
                    return false;

                PropertyBag const& source = pb->rvalue();
                typename internal::AssignableDataSource<T>::reference_t result = ads->set();

                //result.resize( source.size() );
                if(result.count() != source.size()) {
                    log(Error) << "Refusing to compose C Arrays from a property list of different size. Use the same number of properties as the C array size." << endlog();
                    return false;
                }
                // recurse into items of this sequence:
                PropertyBag target( source.getType() );
                PropertyBag decomp;
                internal::ReferenceDataSource<T> rds(result);
                rds.ref(); // prevent dealloc.
                // we compose each item in this sequence and then update result with target's result.
                // 1. each child is composed into target (this is a recursive thing)
                // 2. we decompose result one-level deep and 'refresh' it with the composed children of step 1.
                if ( composePropertyBag(source, target) && typeDecomposition( &rds, decomp, false) && ( decomp.getType() == target.getType() ) && refreshProperties(decomp, target, true) ) {
                    assert(result.count() == source.size());
                    assert(source.size() == target.size());
                    assert(source.size() == decomp.size());
                    return true;
                }
                return false;
            }

        };
    }
}

#endif
