#ifndef ORO_CORBA_SERVICES_I_HPP
#define ORO_CORBA_SERVICES_I_HPP

#include "orbsvcs/CosNotifyChannelAdminS.h"
#include "orbsvcs/CosNotifyCommC.h"
#include "orbsvcs/CosNamingC.h"
#include "Services.hpp"
#include "ServicesC.h"
#include "ServicesS.h"

#include "../Logger.hpp"
#include "ControlTaskServer.hpp"
#include <string>

/**
 * Implementation.
 */
class RTT_Corba_ServiceInterface_i
    : public virtual POA_RTT::Corba::ServiceInterface,
      public virtual PortableServer::RefCountServantBase
{
public:
    virtual char* getLogMessage()
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
    {
        std::string ret = RTT::log().getLogLine();
        return CORBA::string_dup( ret.c_str() );
    }

    virtual CORBA::Boolean requestShutdown()
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
    {
        if ( RTT::Corba::Services::MayShutdown == false)
            return false;
        RTT::Corba::ControlTaskServer::ShutdownOrb(false);
        return true;
    }

    virtual CosNaming::NamingContext_ptr getNamingContext()
        ACE_THROW_SPEC ((
                         CORBA::SystemException
                         ))
    {
        return RTT::Corba::Services::getNamingContext();
    }

};

#endif
