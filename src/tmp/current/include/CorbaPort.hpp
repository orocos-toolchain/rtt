 
#ifndef ORO_CORBA_PORT_HPP
#define ORO_CORBA_PORT_HPP

#include <string>
#include "../PortInterface.hpp"
#include "DataFlowC.h"
#include "orbsvcs/CosEventChannelAdminC.h"
#include "orbsvcs/CosEventCommC.h"

namespace RTT
{ namespace Corba {

    /** 
     * A port used by ControlTaskProxy objects to connect
     * a corba data or buffer channel as an Orocos Port.
     */
    class CorbaPort
        : public PortInterface
    {
        DataFlowInterface_var mdata;
        ConnectionInterface::shared_ptr dc;
    public:
        CorbaPort(const std::string& name, DataFlowInterface_ptr dflow, PortableServer::POA_ptr )
            : PortInterface(name),
              mdata( DataFlowInterface::_duplicate(dflow) )
        {
        }

        AssignableExpression_ptr getDataChannel()
        {
            return mdata->createDataChannel( this->getName().c_str() );
        }

        BufferChannel_ptr getBufferChannel()
        {
            return mdata->createBufferChannel( this->getName().c_str() );
        }

        /**
         * Get the PortType of this port.
         */
        virtual PortType getPortType() const {
            return PortType(int(mdata->getPortType( this->getName().c_str() )));
        }

        virtual bool connected() const {
            return mdata->isConnected( this->getName().c_str() );
        }

        virtual ConnectionInterface::shared_ptr connection() const {
            return dc;
        }

        using PortInterface::connectTo;

        virtual bool connectTo( ConnectionInterface::shared_ptr conn ) {
            dc = conn;
            return true;
        }

        virtual void disconnect() {
            dc = 0;
        }

        virtual PortInterface* clone() const {
            return new CorbaPort( this->getName(), mdata.in(), 0 );
        }

        virtual PortInterface* antiClone() const {
            return new CorbaPort( this->getName(), mdata.in(), 0 );
        }

        /**
         * Create a connection to a local port.
         */
        virtual ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree) 
        {
            // prevent infinite call-back:
            if ( dynamic_cast<CorbaPort*>(other) ) {
                log() << "Can not connect two remote ports." <<endlog(Error);
                log() << "One must be local and one must be remote." <<endlog(Error);
                return 0;
            }

            // if the other is local, its connection factory will setup the connection.
            return other->createConnection(this, con_type);
        }

        /**
         * Do not create a connection without a peer port.
         */
        virtual ConnectionInterface::shared_ptr createConnection(ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree) 
        {
            Logger::In in("CorbaPort");
            log() << "Can not create remote port connection without local port." <<endlog(Error);
            return 0;
        }
    };

}}

#endif
