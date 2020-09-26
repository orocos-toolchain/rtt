/***************************************************************************
  tag: Peter Soetens Fri Nov 19 17:41:42 2010 +0100 GlobalService.cpp

                        GlobalService.cpp -  description
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


#include "rtt/internal/GlobalService.hpp"
#include "rtt/plugin/PluginLoader.hpp"

#include "rtt/os/StartStopManager.hpp"

namespace RTT
{

    // Cleanup the GlobalService on termination of the process
    // This is important if global services have ports that need to be disconnected.
    namespace {
        os::CleanupFunction release_global_service(&internal::GlobalService::Release);
    }

    namespace internal
    {
        static Service::shared_ptr mserv;

        GlobalService::GlobalService()
            : Service( "GlobalService" )
        {
            addOperation("require", &GlobalService::require, this)
                    .doc("Require that a certain service is loaded in the global service.")
                    .arg("service_name","The name of the service to load globally.");
        }

        GlobalService::~GlobalService()
        {
        }

        Service::shared_ptr GlobalService::Instance() {
            if ( !mserv ) {
                mserv.reset( new GlobalService() );
            }
            return mserv;
        }
        void GlobalService::Release() {
            mserv.reset();
        }

        bool GlobalService::require(const std::string servicename) {
            return hasService(servicename) || plugin::PluginLoader::Instance()->loadService(servicename, 0); // load globally.
        }
    }

}
