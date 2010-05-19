/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  Typekit.hpp

                        Typekit.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_TYPEKIT_HPP
#define ORO_TYPEKIT_HPP

#include <string>
#include <vector>
#include "../rtt-config.h"
#include "rtt-types-fwd.hpp"
#include "../plugin/rtt-plugin-fwd.hpp"

namespace RTT
{ namespace types {

    /**
     * This class allows the Orocos Real-Time Typekit to be extended
     * with additional typekits (TypekitPlugin), such as for kinematics or any
     * library foreign to Orocos.
     * @ingroup CoreLib
     */
    class RTT_API Typekit
    {
        /** Loaded tools. */
        static std::vector<TypekitPlugin*> Tools;
        /** Loaded transports. */
        static std::vector<plugin::TransportPlugin*> Transports;
    public:
        /**
         * Import a plugin into Orocos.
         * One may try to load the same plugin multiple
         * times from different places. Only the first
         * import will succeed. Multiple imports will not
         * be considered as an error.
         */
        static void Import( TypekitPlugin& tkp );
        /**
         * Import a transport plugin into Orocos.
         * One may try to load the same transport plugin multiple
         * times from different places. Only the first
         * import will succeed. Multiple imports will not
         * be considered as an error.
         */
        static void Import( plugin::TransportPlugin& tr );

        /**
         * Get the names of all imported tools.
         */
        static std::vector<std::string> getTools();
        /**
         * Get the names of all imported transports.
         */
        static std::vector<std::string> getTransports();

        /**
         * Check if a tool with given name was already imported.
         */
        static bool hasTool( const std::string& toolname );
        /**
         * Check if a transport with given name was already imported.
         */
        static bool hasTransport( const std::string& transportname );
    };

}}

/**
 * Once you defined your TypekitPlugin class,
 * you can use this macro to make it available as a
 * plugin.
 * @note Do not use this macro inside a namespace !
 * For example: ORO_TYPEKIT_PLUGIN ( KDL::KDLTypekit )
 * where KDL::KDLTypekit is a *classname*, derived from
 * RTT::types::TypekitPlugin.
 */
#define ORO_TYPEKIT_PLUGIN( TYPEKIT ) \
    namespace RTT { class TaskContext; } \
    extern "C" {                      \
        bool loadRTTPlugin(RTT::TaskContext* tc) { \
            TYPEKIT tk; \
            if (tc == 0) { \
                RTT::types::Typekit::Import( tk ); \
                return true; \
            } \
            return false; \
        } \
    std::string getRTTPluginName() { \
        TYPEKIT tk; \
        return tk.getName(); \
    } \
    std::string getRTTTargetName() { \
        return OROCOS_TARGET_NAME; \
    } \
}


#endif
