/***************************************************************************
  tag: Peter Soetens Fri Mar 16 21:32:02 2012 +0100 TypeInfoGenerator.hpp

                        TypeInfoGenerator.hpp -  description
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


#ifndef RTT_TYPEINFO_GENERATOR_HPP
#define RTT_TYPEINFO_GENERATOR_HPP

#include <string>
#include "rtt/rtt-config.h"
#include "rtt/types/rtt-types-fwd.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * All generator classes inherit from this object
         * in order to allow them to be added to the
         * TypeInfoRepository. This class is solely
         * meant for installing TypeInfo objects, so a helper
         * class to the type system which has no function
         * once the type registration is done.
         */
        class RTT_API TypeInfoGenerator
        {
        public:
            virtual ~TypeInfoGenerator() {}

            /**
             * Return the type name for which this generator
             * generates type info features. This name will be
             * aliased by the TypeInfo object.
             */
            virtual const std::string& getTypeName() const = 0;
            
            /**
             * Installs the type info object in the global data source type info handler
             * and adds any additional features to the type info object.
             * This method will be called by the TypeInfoRepository, in order to register this
             * type's factories into the TypeInfo object.
             * @param ti A valid TypeInfo object into which new features
             * may be installed
             * @return true if this object may be deleted, false if not.
             * @post When true is returned, this instance is still valid and
             * the caller (TypeInfoRepository) will delete it. When false is returned,
             * the validity is undefined and the instance will not be used
             * anymore by the caller.
             */
            virtual bool installTypeInfoObject(TypeInfo* ti) = 0;

            /**
             * Returns the TypeInfo object of this type, or null
             * if none exists yet.
             * @return All generators should return here TypeInfoRepository::Instance()->getTypeInfo<T>();
             */
            virtual TypeInfo* getTypeInfoObject() const = 0;
        };
    }
}

#endif
