/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  BuildType.hpp 

                        BuildType.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_CORELIB_BUILD_TYPE_HPP
#define ORO_CORELIB_BUILD_TYPE_HPP

#include "Types.hpp"
#include "DataSourceAdaptor.hpp"
#include "DataSources.hpp"

namespace RTT
{
    namespace detail 
    {
        /**
         * A helper class to build a value of type T.
         */
        template<class T>
        struct BuildType
        {
            static typename AssignableDataSource<T>::shared_ptr Value(const T& value = T()) {
                typename AssignableDataSource<T>::shared_ptr _value;
#ifndef ORO_EMBEDDED
                assert( detail::DataSourceTypeInfo<T>::getTypeInfo() );
                DataSourceBase::shared_ptr dsb = detail::DataSourceTypeInfo<T>::getTypeInfo()->buildValue();
                if (dsb)
                    // First see if it is a predefined type (not unknown) and if so, build that one.
                    _value = AdaptAssignableDataSource<T>()(dsb);
                if ( _value ) {
                    _value->set( value );
                } else {
                    // this type is unknown, build a default one.
                    _value = new ValueDataSource<T>(value);
                }
#else
                _value = new ValueDataSource<T>(value);
#endif
                return _value;
            }
        };
    }
}

#endif
