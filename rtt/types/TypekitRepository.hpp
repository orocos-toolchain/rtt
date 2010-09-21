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

namespace RTT
{ namespace types {

    /**
     * This class allows the RTT to be extended
     * with additional typekits (TypekitPlugin), such as for kinematics or any
     * library foreign to Orocos.
     * @ingroup CoreLib
     */
    class RTT_API TypekitRepository
    {
        /** Loaded typekits. */
        static std::vector<TypekitPlugin*> Typekits;
        /** Loaded transports. */
        static std::vector<TransportPlugin*> Transports;
    public:
        /**
         * Import a typekit plugin into the type system of this process.
         *
         * One may try to load the same plugin multiple
         * times from different places. Only the first
         * import will succeed. Multiple imports will not
         * be considered as an error, but will be silently
         * freed.
         *
         * @param tkp A typekit plugin that becomes owned
         * by the TypekitRepository.
         */
        static void Import( TypekitPlugin* tkp );
        /**
         * Import a transport plugin into the type system of this process.
         *
         * One may try to load the same transport plugin multiple
         * times from different places. Only the first
         * import will succeed. Multiple imports will not
         * be considered as an error, but will be silently
         * freed.
         *
         * @param tr A transport plugin that becomes owned
         * by the TypekitRepository.
         */
        static void Import( TransportPlugin* tr );

        /**
         * Get the names of all imported typekits.
         */
        static std::vector<std::string> getTypekits();
        /**
         * Get the names of all imported transports.
         */
        static std::vector<std::string> getTransports();

        /**
         * Check if a typekit with given name was already imported.
         */
        static bool hasTypekit( const std::string& typekitname );
        /**
         * Check if a transport with given name was already imported.
         * The name should have the form 'PROTOCOL://plugin', for example:
         * "CORBA://rtt-types"
         */
        static bool hasTransport( const std::string& transportname );

        /**
         * Removes all Typekits and transports from memory.
         */
        static void Release();
    };

}}

#endif
