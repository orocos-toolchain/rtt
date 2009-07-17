#ifndef CORBACONNECTION_HPP_
#define CORBACONNECTION_HPP_

//necessary to keep MSVC happy 
//avoids redefinitions in winsock2.h
/*#pragma push_macro("_WINSOCKAPI_")
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <windows.h>

#pragma pop_macro("_WINSOCKAPI_")*/
/////////////////////////////////////////

#include "../ConnectionInterface.hpp"
#include "DataFlowC.h"
#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>
#else
#include <omniORB4/CORBA.h>
#include <omniORB4/poa.h>
#endif


namespace RTT
{

    namespace Corba
    {

        class RTT_API CorbaConnection : public RTT::ConnectionInterface
        {
            std::string mname;
            AssignableExpression_var mdatachannel;
            BufferChannel_var mbufchannel;
            DataFlowInterface_var mdflow;
            /**
             * The local connection object, keeping track of
             * connections to the remote port.
             */
            ConnectionInterface::shared_ptr lcc;
            /**
             * Helper function to ask another port to create
             * a local connection object (\a lcc) for this
             * remote connection.
             */
            bool createConnection(PortInterface* p);
        public:
            CorbaConnection(const std::string& mname, DataFlowInterface_ptr dflow, PortableServer::POA_ptr);
            CorbaConnection(const std::string& mname, DataFlowInterface_ptr dflow, AssignableExpression_ptr datachannel, PortableServer::POA_ptr );
            CorbaConnection(const std::string& mname, DataFlowInterface_ptr dflow, BufferChannel_ptr bufchannel, PortableServer::POA_ptr);
            virtual ~CorbaConnection();
            virtual DataSourceBase::shared_ptr getDataSource() const;

            virtual BufferBase::shared_ptr getBuffer() const;

            virtual bool connect();

            virtual bool connected() const;

            virtual bool disconnect();

            virtual bool addPort(PortInterface* r);

            virtual bool removePort(PortInterface* r);

            virtual const RTT::TypeInfo* getTypeInfo() const;

            virtual int serverProtocol() const;
        };

    }

}

#endif /*CORBACONNECTION_HPP_*/
