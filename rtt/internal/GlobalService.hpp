/***************************************************************************
  tag: Peter Soetens Fri Nov 19 17:41:42 2010 +0100 GlobalService.hpp

                        GlobalService.hpp -  description
                           -------------------
    begin                : Fri Nov 19 2010
    copyright            : (C) 2010 Peter Soetens
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



#ifndef ORO_GLOBALSERVICE_HPP_
#define ORO_GLOBALSERVICE_HPP_

#include "../Service.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{

    namespace internal
    {

        /**
         * A process-wide services that hosts services not local
         * to a single component.
         */
        class GlobalService: public RTT::Service
        {
            GlobalService();
        public:
            virtual ~GlobalService();
            RTT_API static Service::shared_ptr Instance();
            RTT_API static void Release();

            /**
             * Require that a certain service is loaded in the global service.
             * You may require the same \a servicename multiple times. This function
             * will each subsequent time return the same value as the first time
             * it was called with \a servicename.
             * @param servicename The name of the service to load
             * @return true if the service was loaded, false if the servicename was
             * unknown.
             */
            bool require(const std::string servicename);
        };

    }

}

#endif /* ORO_GLOBALSERVICE_HPP_ */
