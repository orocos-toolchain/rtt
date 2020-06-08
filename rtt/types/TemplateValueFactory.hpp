/***************************************************************************
  tag: Peter Soetens Fri Mar 16 21:32:02 2012 +0100 TemplateValueFactory.hpp

                        TemplateValueFactory.hpp -  description
                           -------------------
    begin                : Fri Mar 16 2012
    copyright            : (C) 2012 Peter Soetens
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


#ifndef RTT_TEMPLATE_TYPE_FACTORY
#define RTT_TEMPLATE_TYPE_FACTORY

#include <cassert>

#include "ValueFactory.hpp"
#include "../Property.hpp"
#include "../Attribute.hpp"
#include "../Logger.hpp"
#include "../internal/DataSources.hpp"
#include "../internal/PartDataSource.hpp"
#include "../rtt-config.h"

namespace RTT
{
    namespace types {

        template<class T>
        class TemplateValueFactory 
            : public ValueFactory
        {
        public:
            typedef T DataType;
            base::AttributeBase* buildConstant(std::string name, base::DataSourceBase::shared_ptr dsb) const
            {
                typename internal::DataSource<DataType>::shared_ptr res =
                    boost::dynamic_pointer_cast< internal::DataSource<DataType> >( internal::DataSourceTypeInfo<DataType>::getTypeInfo()->convert(dsb));
                if ( res ) {
                    res->get();
                    //Logger::log() << Logger::Info << "Building "<<tname<<" Constant '"<<name<<"' with value "<< dsb->getTypeInfo()->toString(dsb) <<Logger::endl;
                    return new Constant<DataType>( name, res->rvalue() );
                }
                else
                    return 0;
            }

            base::AttributeBase* buildVariable(std::string name) const
            {
                // A variable starts its life as unbounded.
                //Logger::log() << Logger::Debug << "Building variable '"<<name <<"' of type " << tname <<Logger::endl;
                return new Attribute<DataType>( name, new internal::UnboundDataSource<internal::ValueDataSource<DataType> >() );
            }

            base::AttributeBase* buildAttribute( std::string name, base::DataSourceBase::shared_ptr in) const
            {
                typename internal::AssignableDataSource<DataType>::shared_ptr ds;
                if ( !in )
                    ds = new internal::ValueDataSource<DataType>();
                else
                    ds = internal::AssignableDataSource<DataType>::narrow( in.get() );
                if (!ds)
                    return 0;
                // An attribute is always bounded.
                //Logger::log() << Logger::Debug << "Building Attribute '"<< name <<"' of type " << tname <<Logger::endl;
                return new Attribute<DataType>( name, ds.get() );
            }

            base::AttributeBase* buildAlias(std::string name, base::DataSourceBase::shared_ptr in ) const
            {
                typename internal::DataSource<T>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<T> >( internal::DataSourceTypeInfo<T>::getTypeInfo()->convert(in) );
                if ( ! ds )
                    return 0;
                return new Alias( name, ds );
            }

            base::DataSourceBase::shared_ptr buildActionAlias(base::ActionInterface* action, base::DataSourceBase::shared_ptr in) const
            {
                typename internal::AssignableDataSource<T>::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( in ); // no type conversion is done.
                if ( ads )
                    return new internal::ActionAliasAssignableDataSource<T>(action, ads.get());

                typename internal::DataSource<T>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<T> >( in ); // no type conversion is done.
                if ( ! ds )
                    return 0;
                return new internal::ActionAliasDataSource<T>(action, ds.get());
            }

            virtual base::PropertyBase* buildProperty(const std::string& name, const std::string& desc, base::DataSourceBase::shared_ptr source = 0) const {
                if (source) {
                    typename internal::AssignableDataSource<DataType>::shared_ptr ad
                        = boost::dynamic_pointer_cast< internal::AssignableDataSource<DataType> >( source );
                    if (ad)
                        return new Property<DataType>(name, desc, ad );
                    else {
                        //log(Error) <<"Failed to build 'Property<"<< this->tname <<"> "<<name<<"' from given DataSourceBase. Returning default."<<endlog();
                    }
                }
                return new Property<DataType>(name, desc, DataType());
            }

            virtual base::DataSourceBase::shared_ptr buildValue() const {
                return new internal::ValueDataSource<DataType>();
            }
            virtual base::DataSourceBase::shared_ptr buildReference(void* ptr) const {
                return new internal::ReferenceDataSource<DataType>(*static_cast<DataType*>(ptr));
            }
            virtual base::DataSourceBase::shared_ptr buildPart(void* ptr, base::DataSourceBase::shared_ptr parent) const {
                return new internal::PartDataSource<DataType>(*static_cast<DataType*>(ptr), parent);
            }

        };

    }
}

#endif
