/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  ApplicationServer.hpp

                        ApplicationServer.hpp -  description
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


#ifndef ORO_APPLICATION_SERVER_HPP
#define ORO_APPLICATION_SERVER_HPP

#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/PortableServer/PortableServer.h>
#else
#include <omniORB4/poa.h>
#endif
#include "../../Time.hpp"

namespace RTT
{namespace corba
{
    /**
     * A class which an provides ORB to
     * the application process.
     */
    struct ApplicationServer
    {
        /**
         * The orb of this process.
         */
        static CORBA::ORB_var orb;

        /**
         * The root POA of this process.
         */
        static PortableServer::POA_var rootPOA;

        /**
         * Invoke this method once to initialise the Orb which will
         * run the task servers.
	 * @param orb_timeout timeout value for each remote call, expressed in seconds.
	 * The resolution is up to 100 nano seconds. Anything smaller will be interpreted
	 * as a zero.
         */
        RTT_CORBA_API static bool InitOrb(int argc, char* argv[], Seconds orb_timeout=0 );

    };
}}

#endif
