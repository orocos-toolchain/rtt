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
#include "type_discovery.hpp"

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
        class StructTypeInfo: public TemplateTypeInfo<T, has_ostream>
        {
        public:
            StructTypeInfo(std::string name) :
                TemplateTypeInfo<T, has_ostream> (name)
            {
            }

            virtual std::vector<std::string> getMemberNames() const {
                // only discover the parts of this struct:
                type_discovery in;
                T t; // boost can't work without a value.
                in.discover( t );
                return in.mnames;
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                typename internal::AssignableDataSource<T>::shared_ptr adata = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( item );
                if ( adata ) {
                    type_discovery in( item );
                    in.discover( adata->set() );
                    log(Debug) << "Returning part: " << name << endlog();
                    return in.getMember(name);
                }
                typename internal::DataSource<T>::shared_ptr data = boost::dynamic_pointer_cast< internal::DataSource<T> >( item );
                if ( data ) {
                    adata = new internal::ValueDataSource<T>( data->get() );
                    type_discovery in( adata );
                    in.discover( adata->set() );
                    log(Debug) << "Returning copy of part: " << name << endlog();
                    return in.getConstMember(name);
                }
                log(Error) << "Wrong call to type info function " + this->getTypeName() << "'s getMember() can not process "<< item->getTypeName() <<endlog();
                return base::DataSourceBase::shared_ptr();
            }
        };
    }
}

#endif
