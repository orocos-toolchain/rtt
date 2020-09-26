/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  StdTypeInfo.hpp

                        StdTypeInfo.hpp -  description
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


#ifndef ORO_STD_TYPE_INFO_HPP
#define ORO_STD_TYPE_INFO_HPP

#include "rtt/rtt-config.h"
#include "rtt/types/Types.hpp"
#include "rtt/types/TemplateTypeInfo.hpp"

namespace RTT
{
    namespace types {

        /**
         * Type information for standard C types or structs of which
         * no introspection is required. No boost::serialization function
         * is required to use this class.
         *
         * @see StructTypeInfo
         */
        template<class T>
        struct StdTypeInfo
        : public TemplateTypeInfo<T,true>
        {
            StdTypeInfo(const char* type)
            : TemplateTypeInfo<T,true>(type)
              {}

            virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
                // First, try a plain update.
                if ( result->update( source.get() ) )
                    return true;
                return false;
            }

            /**
             * A primitive type is decomposed into itself.
             */
            virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const {
                return source;
            }
        };
    }
}

#endif
