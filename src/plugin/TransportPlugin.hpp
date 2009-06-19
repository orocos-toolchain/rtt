/***************************************************************************
 Copyright (c) 2009 S Roderick <xxxkiwi DOT xxxnet AT macxxx DOT comxxx>
                               (remove the x's above)

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


#ifndef ORO_TRANSPORTPLUGIN_HPP
#define ORO_TRANSPORTPLUGIN_HPP 1

#include <string>
#include "Types.hpp"

namespace RTT
{

    /**
     * A class which registers TransportProtocol instances to types.
     * Use the ORO_TOOLKIT_PLUGIN macro to have the plugin framework 
     * automatically load all types supported by this plugin (using 
     * registerTransport() below).
     */
    class TransportPlugin
    {
    public:
        virtual ~TransportPlugin() {}

        /**
         * Add a transport for the given type to the TypeInfo
         * instance.
         * @param type_name The name of the type to transport
         * @param ti The typ to which transports may be added.
         * @return false if no transport was added, true otherwise.
         */
        virtual bool registerTransport(std::string type_name, TypeInfo* ti) = 0;

        /**
         * Returns the (protocol) name of this transport.
         * e.g. "CORBA"
         */
        virtual std::string getTransportName() const = 0;

        /**
         * Each plugin must have a unique name.
         * e.g. "CorbaKDL"
         */
        virtual std::string getName() const = 0;
    };

}

#endif
