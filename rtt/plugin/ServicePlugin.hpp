/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  ServicePlugin.hpp

                        ServicePlugin.hpp -  description
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


#ifndef ORO_SERVICEPLUGIN_HPP_
#define ORO_SERVICEPLUGIN_HPP_

#include <string>
#include "../TaskContext.hpp"

namespace RTT {
    namespace plugin {

    }
}

#define ORO_SERVICEPLUGIN_xstr(s) ORO_SERVICEPLUGIN_str(s)
#define ORO_SERVICEPLUGIN_str(s) #s


/**
 * You can use this macro to make any Service available as a
 * plugin.
 * @param SERVICE A class that inherits from Service and takes a TaskContext*
 * as argument in a constructor.
 * @param NAME A string being the name of the plugin.
 *
 * @note Do not use this macro inside a namespace !
 * For example: ORO_SERVICE_NAMED_PLUGIN ( KDL::KDLService, "kdl" )
 * where KDL::KDLService is a class name.
 */
#define ORO_SERVICE_NAMED_PLUGIN( SERVICE, NAME ) \
    extern "C" {\
        RTT_EXPORT bool loadRTTPlugin(RTT::TaskContext* tc);  \
        bool loadRTTPlugin(RTT::TaskContext* tc) {    \
            if (tc == 0) return true; \
            RTT::Service::shared_ptr sp( new SERVICE( tc ) ); \
            return tc->provides()->addService( sp ); \
        } \
        RTT_EXPORT RTT::Service::shared_ptr createService();  \
        RTT::Service::shared_ptr createService() {    \
            RTT::Service::shared_ptr sp( new SERVICE( 0 ) ); \
            return sp; \
        } \
        RTT_EXPORT std::string getRTTPluginName(); \
        std::string getRTTPluginName() { \
            return NAME; \
        } \
        RTT_EXPORT std::string getRTTTargetName(); \
        std::string getRTTTargetName() { \
            return OROCOS_TARGET_NAME; \
        } \
    }
/**
 * You can use this macro to make any Service available as a
 * plugin.
 * @param SERVICE A class that inherits from Service and takes a TaskContext*
 * as argument in a constructor. The name of the plugin is equal to SERVICE, but the
 * name of the service (ie Service::getName() ) may be different.
 *
 * @note Do not use this macro inside a namespace !
 * For example: ORO_SERVICE_PLUGIN ( KDL::KDLService )
 * where KDL::KDLService is a class name.
 */
#define ORO_SERVICE_PLUGIN( SERVICE ) \
    extern "C" {                      \
        RTT_EXPORT bool loadRTTPlugin(RTT::TaskContext* tc); \
        bool loadRTTPlugin(RTT::TaskContext* tc) {    \
            if (tc == 0) return true; \
            RTT::Service::shared_ptr sp( new SERVICE( tc ) ); \
            return tc->provides()->addService( sp ); \
        } \
        RTT_EXPORT RTT::Service::shared_ptr createService();  \
        RTT::Service::shared_ptr createService() {    \
            RTT::Service::shared_ptr sp( new SERVICE( 0 ) ); \
            return sp; \
        } \
        RTT_EXPORT std::string getRTTPluginName(); \
        std::string getRTTPluginName() { \
            return ORO_SERVICEPLUGIN_xstr(SERVICE); \
        } \
        RTT_EXPORT std::string getRTTTargetName(); \
        std::string getRTTTargetName() { \
            return OROCOS_TARGET_NAME; \
        } \
    }

#endif /* ORO_SERVICEPLUGIN_HPP_ */
