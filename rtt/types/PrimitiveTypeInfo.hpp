/***************************************************************************
  tag: Peter Soetens Fri Jun 10 15:16:01 2011 +0200 PrimitiveTypeInfo.hpp

                        PrimitiveTypeInfo.hpp -  description
                           -------------------
    begin                : Fri Jun 10 2011
    copyright            : (C) 2011 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_PRIMITIVE_TYPEINFO_HPP
#define ORO_PRIMITIVE_TYPEINFO_HPP

#include "rtt/types/Types.hpp"
#include "rtt/Property.hpp"
#include "rtt/Attribute.hpp"
#include "rtt/Logger.hpp"
#include "rtt/types/TypeStreamSelector.hpp"
#include "rtt/types/TypeInfoGenerator.hpp"
#include "rtt/types/StreamFactory.hpp"
#include "rtt/types/TemplateValueFactory.hpp"
#include "rtt/rtt-config.h"

namespace RTT
{
    namespace types {

    /**
     * This template class allows primitive types, which are not sent over ports, to be added to Orocos.
     * The type is known to the type system, but you can not use it in flow ports.
     *
     * @param T The user class type.
     * @param use_ostream When set to \a true, the class will use operator<<(std::ostream&, T)
     * to write out the type to a stream. When set to \a false, the class will use this function
     * and write '( \a type \a name )' to a stream instead. Defaults to \a false. Set to \a true
     * if your class \a T has the above mentioned function.
     * @see TypeInfoRepository.
     * @see operator<<
     * @see StructTypeInfo, SequenceTypeInfo, CArrayTypeInfo, BoostArrayTypeInfo
     */
    template<typename T, bool use_ostream = false>
    class PrimitiveTypeInfo :
            public TypeInfoGenerator,
            public TemplateValueFactory<T>,
            public StreamFactory
    {
    protected:
        const std::string tname;
        boost::shared_ptr<PrimitiveTypeInfo<T, use_ostream> > mshared;
    public:
        /**
         * The given \a T parameter is the type of the DataSources.
         */
        typedef T DataType;

        /**
         * Setup Type Information for type \a name.
         * This causes a switch from 'unknown' type to basic
         * type information for type T.
         * @param name the 'Orocos' type name.
         *
         */
        PrimitiveTypeInfo(std::string name)
            : tname(name)
        {
        }

        virtual ~PrimitiveTypeInfo()
        {
        }

        boost::shared_ptr<PrimitiveTypeInfo<T, use_ostream> > getSharedPtr() {
            if (!mshared)
                mshared.reset(this);
            return mshared;
        }

        bool installTypeInfoObject(TypeInfo* ti) {
            // Install the factories for primitive types
            ti->setValueFactory( this->getSharedPtr() );
	    if (use_ostream)
	      ti->setStreamFactory( this->getSharedPtr() );

            // Install the type info object for T
            internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject = ti;
            ti->setTypeId( &typeid(T) );

            // Clean up reference to ourselves.
            mshared.reset();
            // Don't delete us, we're memory-managed.
            return false;
        }

        TypeInfo* getTypeInfoObject() const {
            return TypeInfoRepository::Instance()->getTypeInfo<T>();
        }

        virtual const std::string& getTypeName() const { return tname; }

        virtual std::ostream& write( std::ostream& os, base::DataSourceBase::shared_ptr in ) const {
            typename internal::DataSource<T>::shared_ptr d = boost::dynamic_pointer_cast< internal::DataSource<T> >( in );
            if ( d ) {
                types::TypeStreamSelector<T, use_ostream>::write( os, d->rvalue() );
            }
            return os;
        }

        virtual std::istream& read( std::istream& os, base::DataSourceBase::shared_ptr out ) const {
            typename internal::AssignableDataSource<T>::shared_ptr d = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( out );
            if ( d ) {
                types::TypeStreamSelector<T, use_ostream>::read( os, d->set() );
                d->updated(); // because use of set().
            }
            return os;
        }

        virtual bool isStreamable() const {
            return use_ostream;
        }

        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
            return false;
        }

        /**
         * A primitive type is decomposed into itself.
         */
        virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const
        {
            return source;
        }

        virtual bool decomposeType( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            return false;
        }
    };
}}

#endif
