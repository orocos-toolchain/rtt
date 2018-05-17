/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ApplicationServer.cxx

                        ApplicationServer.cxx -  description
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


#include "ApplicationServer.hpp"
#include "../../Logger.hpp"

#if defined( CORBA_IS_TAO ) && defined( CORBA_TAO_HAS_MESSAGING )
#include <tao/TimeBaseC.h>
#include <tao/Messaging/Messaging.h>
#endif

namespace RTT
{namespace corba
{

    CORBA::ORB_var RTT_API ApplicationServer::orb;

    PortableServer::POA_var ApplicationServer::rootPOA;

    RTT_CORBA_API bool ApplicationServer::InitOrb(int argc, char* argv[], Seconds timeout ) {
        if ( !CORBA::is_nil(orb) )
            return false;

        try {
            // First initialize the ORB, that will remove some arguments...
            orb =
                CORBA::ORB_init (argc, const_cast<char**>(argv),
                                 "omniORB4");
            if(timeout >= 0.1e-7)
            {
#if defined( CORBA_IS_TAO ) && defined( CORBA_TAO_HAS_MESSAGING )
                // Set the timeout value as a TimeBase::TimeT (100 nanosecond units)
                // and insert it into a CORBA::Any.
                TimeBase::TimeT relative_rt_timeout = timeout * 1.0e7;
                CORBA::Any relative_rt_timeout_as_any;
                relative_rt_timeout_as_any <<= relative_rt_timeout;

                // Create the policy and put it in a policy list.
                CORBA::PolicyList policies;
                policies.length(1);
                policies[0] = 
                orb->create_policy (Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE,
                                    relative_rt_timeout_as_any);

                // Apply the policy at the ORB level using the ORBPolicyManager.
                CORBA::Object_var obj = orb->resolve_initial_references ("ORBPolicyManager");
                CORBA::PolicyManager_var policy_manager = CORBA::PolicyManager::_narrow (obj.in());
                policy_manager->set_policy_overrides (policies, CORBA::SET_OVERRIDE);
#else
                log(Error) << "Ignoring ORB timeout setting in non-TAO/Messaging build." <<endlog();
#endif // CORBA_IS_TAO
            }
            // Also activate the POA Manager, since we may get call-backs !
            CORBA::Object_var poa_object =
                orb->resolve_initial_references ("RootPOA");
            rootPOA =
                PortableServer::POA::_narrow (poa_object.in ());
            PortableServer::POAManager_var poa_manager =
                rootPOA->the_POAManager ();
            poa_manager->activate ();

            return true;
        }
        catch (CORBA::Exception &e) {
            log(Error) << "Orb Init : CORBA exception raised!" << Logger::nl;
            Logger::log() << CORBA_EXCEPTION_INFO(e) << endlog();
        }
        return false;
    }

}}
