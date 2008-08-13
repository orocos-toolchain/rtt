#ifndef CORBACONNECTION_HPP_
#define CORBACONNECTION_HPP_

#include "../ConnectionInterface.hpp"
#include "DataFlowC.h"
#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>


namespace RTT
{

    namespace Corba
    {

        class CorbaConnection : public RTT::ConnectionInterface
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

            virtual int serverProtocol();
        };

    }

}

#endif /*CORBACONNECTION_HPP_*/
