/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  TypeConstructor.hpp

                        TypeConstructor.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef ORO_CORELIB_TYPE_BUILDER_HPP
#define ORO_CORELIB_TYPE_BUILDER_HPP

#include <vector>
#include "rtt/base/DataSourceBase.hpp"

namespace RTT
{ namespace types {

    /**
     * This interface describes how constructors work.
     */
    struct RTT_API TypeConstructor
    {
        virtual ~TypeConstructor();
        /**
         * Inspect args and return a type constructed with these args
         * if such a constructor exists.
         */
        virtual base::DataSourceBase::shared_ptr build(const std::vector<base::DataSourceBase::shared_ptr>& args) const = 0;

        /**
         * Automatic type conversion (float->double,... ). Fails by default.
         */
        virtual base::DataSourceBase::shared_ptr convert(base::DataSourceBase::shared_ptr arg) const;
    };
}}

#endif
