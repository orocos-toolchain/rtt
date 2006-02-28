#ifndef ORO_EXECUTION_DATA_FLOW_INTERFACE_HPP
#define ORO_EXECUTION_DATA_FLOW_INTERFACE_HPP

#include <vector>
#include "PortInterface.hpp"

namespace ORO_Execution
{

    /**
     * The Interface of a TaskContext which exposes its data-flow ports.
     */
    class DataFlowInterface
    {
    public:
        typedef std::vector<PortInterface*> Ports;
        Ports ports;

        DataFlowInterface() {}
        ~DataFlowInterface() {
//             for ( Ports::iterator it(ports.begin()); it != ports.end(); ++it)
//                 delete *it;
        }

        /**
         * Add a Port to this task.
         * @internal When added, it is owned by the DataFlowInterface and will be
         * destroyed when removed.
         */
        void addPort(PortInterface* port) { ports.push_back( port ); }

        /**
         * Remove a Port from this task.
         */
        void removePort(const std::string& name) {
            for ( Ports::iterator it(ports.begin());
                 it != ports.end();
                 ++it)
                if ( (*it)->getName() == name )
                    ports.erase(it);
        }

        /**
         * Get all ports of this task.
         */
        Ports getPorts() const { return ports; }

        /**
         * Get a port.
         */
        PortInterface* getPort(const std::string& name) {
            for ( Ports::iterator it(ports.begin());
                 it != ports.end();
                 ++it)
                if ( (*it)->getName() == name )
                    return *it;
            return 0;
        }

        /**
         * Get a port of a specific type.
         */
        template< class Type>
        Type* getPortType(const std::string& name) {
            return dynamic_cast<Type*>( this->getPort(name) );
        }
    };

}

#endif
