
#include "execution/PortInterface.hpp"


namespace ORO_Execution
{

    PortInterface::PortInterface(const std::string& name) : portname(name) {}

    PortInterface::~PortInterface() {}

    bool PortInterface::setName(const std::string& name)
    { 
        if ( !connected() ) {
            portname = name;
            return true;
        }
        return false;
    }

    bool PortInterface::connectTo( PortInterface* other ) {
        if ( other->connection() == 0 )
            return false;
        return this->connectTo( other->connection() );
    }

    ConnectionInterface::shared_ptr PortInterface::createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type )
    {
        return ConnectionInterface::shared_ptr();
    }

    DataSourceFactoryInterface* PortInterface::createDataSources() {
        return 0;
    }

}
