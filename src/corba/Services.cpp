/***************************************************************************
  tag: FMTC  do nov 2 13:06:21 CET 2006  Services.cpp

                        Services.cpp -  description
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


#include "Services.hpp"
#include "ApplicationServer.hpp"
#include "Logger.hpp"
#include "ControlTaskProxy.hpp"

// See comment below.
#if 0
// The TAO implementation of a single event channel.
#include <orbsvcs/CosEvent/CEC_EventChannel.h>
#include <orbsvcs/CosEvent/CEC_Default_Factory.h>
#endif

namespace RTT
{ namespace Corba {

    bool Services::MayShutdown = true;

    CosNaming::NamingContext_var Services::naming_context_;

    CosNaming::NamingContext_ptr Services::getNamingContext()
    {
        if ( CORBA::is_nil( naming_context_ ) ) {
            CORBA::Object_var naming_obj =
                ApplicationServer::orb->resolve_initial_references ("NameService" );

            // Need to check return value for errors.
            if (CORBA::is_nil (naming_obj)) {
                log(Error) << "Could not locate Naming service." <<endlog();
                ACE_THROW (CORBA::UNKNOWN ());
            }

            naming_context_ =
                CosNaming::NamingContext::_narrow (naming_obj.in () );
            log(Info) << "Corba Naming Service found." <<endlog();
        }
        return CosNaming::NamingContext::_duplicate( naming_context_.in() );
    }

    /*
     * This code worked but since event channels are for now no longer used,
     * it is removed from the Services class.
     */
#if 0
    CosNotifyChannelAdmin::EventChannelFactory_ptr Services::getEventChannelFactory()
    {
        if ( CORBA::is_nil( notify_factory_ ) ) {
            // we need the naming service:
            getNamingContext();

            CosNaming::Name name (1);
            name.length (1);
            name[0].id = CORBA::string_dup ("NotifyEventChannelFactory");

            CORBA::Object_var obj;
            try {
                obj = naming_context_->resolve (name);
            } catch (...) {
                log(Error) << "Exception during resolving of Notification service." <<endlog();
            }

            if (CORBA::is_nil( obj ) ) {
                log(Error) << "Could not locate Notification service." <<endlog();
                return 0;
            }

            log(Info) << "Corba Notification Service found." <<endlog();
            notify_factory_ =
                CosNotifyChannelAdmin::EventChannelFactory::_narrow (obj.in () );
        }
        return CosNotifyChannelAdmin::EventChannelFactory::_duplicate( notify_factory_.in() );
    }

    CosEventChannelAdmin::EventChannel_ptr Services::createChannel()
    {
        log(Info) << "Creating Event Channel." << endlog();
#if 0
        // the Notification service is unusable since it does not implement the Pull interfaces !!!
        CosNotification::QoSProperties initial_qos;
        CosNotification::AdminProperties initial_admin;
        CosNotifyChannelAdmin::ChannelID id;

        getEventChannelFactory();
        try {
            if ( notify_factory_ )
                return notify_factory_->create_channel(initial_qos, initial_admin, id);
        } catch (...) {
            log(Error) << "Exception thrown while calling create_channel of Notification Service."<< endlog();
        }
        log(Error) << "No Event Channel created." << endlog();
        return 0;
#endif

        // create a default channel, use the Root POA.
        TAO_CEC_EventChannel_Attributes attributes(ControlTaskProxy::ProxyPOA(), ControlTaskProxy::ProxyPOA() );

        TAO_CEC_EventChannel* ec = new TAO_CEC_EventChannel( attributes, new TAO_CEC_Default_Factory(), 1 );

        log(Info) << "Created a TAO_CEC_EventChannel." << endlog();
        // implicitly activate it.
        return ec->_this();
    }
#endif

}}
