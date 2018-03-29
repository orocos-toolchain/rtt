/***************************************************************************
  tag: psoetens Thu May 18 08:18:55 2006 +0000 DataSourceTypeInfo.inl

                        DataSourceTypeInfo.inl -  description
                           -------------------
    begin                : Thu May 18 2006
    copyright            : (C) 2006 Peter Soetens
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


#ifndef DATASOURCE_TYPE_INFO_INL
#define DATASOURCE_TYPE_INFO_INL

#include "../types/TypeInfo.hpp"
#include "../types/TypeInfoRepository.hpp"

namespace RTT
{
    namespace internal {
        template< class T>
        const std::string& DataSourceTypeInfo<T>::getType() {
	    return getTypeInfo()->getTypeName();
	}

        template< class T>
        const std::string& DataSourceTypeInfo<T>::getTypeName() {
	    return getTypeInfo()->getTypeName();
	}

        template< class T>
        const std::string& DataSourceTypeInfo<T>::getQualifier() {
            return DataSourceTypeInfo<UnknownType>::getQualifier();
        }

        template< class T>
        const types::TypeInfo* DataSourceTypeInfo<T>::getTypeInfo() {
            types::TypeInfo* ret;
            ret = types::TypeInfoRepository::Instance()->getTypeInfo<T>();
            if (!ret)
                return DataSourceTypeInfo<UnknownType>::getTypeInfo();
            return ret;
        }

        template< class T>
        types::TypeInfo* DataSourceTypeInfo<T>::TypeInfoObject = 0;

		template< class T>
		const types::TypeInfo* DataSourceTypeInfo<T*>::getTypeInfo() {
            types::TypeInfo* ret;
            ret = types::TypeInfoRepository::Instance()->getTypeInfo<T*>();
            if (!ret)
                return DataSourceTypeInfo<UnknownType>::getTypeInfo();
            return ret;
		}

        template< class T>
        types::TypeInfo* DataSourceTypeInfo<T*>::TypeInfoObject = 0;

		template< class T>
		const types::TypeInfo* DataSourceTypeInfo< types::carray<T> >::getTypeInfo() {
			// return carray type info
            if (!TypeInfoObject) {
                TypeInfoObject = types::TypeInfoRepository::Instance()->getTypeInfo< types::carray<T> >();
                if (!TypeInfoObject)
                    return DataSourceTypeInfo<UnknownType>::getTypeInfo();
            }
            return TypeInfoObject;
		}

        template< class T>
        types::TypeInfo* DataSourceTypeInfo< types::carray<T> >::TypeInfoObject = 0;

    }

}

#endif
