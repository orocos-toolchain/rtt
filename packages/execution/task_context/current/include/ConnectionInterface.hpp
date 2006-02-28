#ifndef ORO_EXECUTION_CONNECTION_INTERFACE_HPP
#define ORO_EXECUTION_CONNECTION_INTERFACE_HPP

#include <boost/intrusive_ptr.hpp>
#include <os/fosi.h>

namespace ORO_Execution
{
    class ConnectionInterface;
}

void intrusive_ptr_add_ref( ORO_Execution::ConnectionInterface* p );
void intrusive_ptr_release( ORO_Execution::ConnectionInterface* p );

namespace ORO_Execution
{
    class PortInterface;

    /**
     * An Object which manages and maintains a communication connection
     * between two or more Ports.
     * @see PortInterface
     */
    struct ConnectionInterface
    {
    protected:
        friend void ::intrusive_ptr_add_ref( ConnectionInterface* p );
        friend void ::intrusive_ptr_release( ConnectionInterface* p );
        atomic_t refcount;
    public:
        
        typedef boost::intrusive_ptr<ConnectionInterface> shared_ptr;

        ConnectionInterface();
        virtual ~ConnectionInterface();

        /**
         * Connect all readers and writers.
         */
        virtual bool connect() = 0;

        /**
         * Get Connection status.
         */
        virtual bool connected() const = 0;

        /**
         * Disconnect all readers and writers.
         */
        virtual bool disconnect() = 0;

        virtual bool addReader(PortInterface* r) = 0;

        virtual bool removeReader(PortInterface* r) = 0;

        virtual bool addWriter(PortInterface* r) = 0;

        virtual bool removeWriter(PortInterface* r) = 0;
    };
}

#endif
