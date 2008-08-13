/***************************************************************************
  tag: FMTC  do nov 2 13:06:17 CET 2006  POAUtility.h

                        POAUtility.h -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
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


#ifndef RTT_CORBA_POAUTILITY_H_
#define RTT_CORBA_POAUTILITY_H_

#include "../rtt-config.h"
#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>



class POAUtility {

public:
        
    static PortableServer::POA_ptr
    create_basic_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread,
	CORBA::Boolean                    isPersistent
    );


    static PortableServer::POA_ptr
    create_service_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread
    );


    static PortableServer::POA_ptr
    create_servant_activator_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread,
	CORBA::Boolean                    isPersistent
    );


    static PortableServer::POA_ptr
    create_servant_locator_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread,
	CORBA::Boolean                    isPersistent
    );


    static PortableServer::POA_ptr
    create_default_servant_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread
    );

};

#endif
