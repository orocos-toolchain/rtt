#ifndef ORO_CONN_INPUT_ENDPOINT_HPP
#define ORO_CONN_INPUT_ENDPOINT_HPP

#include "internal/Channels.hpp"

namespace RTT
{
    template<typename T> class OutputPort;

    /** This is a channel element that represents the input endpoint of a
     * connection, i.e. the part that is connected to the OutputPort
     */
    template<typename T>
    class ConnInputEndpoint : public ChannelElement<T>
    {
        OutputPort<T>* port;

    public:
        ConnInputEndpoint(OutputPort<T>* port)
            : port(port) { }

        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool read(typename ChannelElement<T>::reference_t sample)
        { return false; }

        virtual void disconnect(bool forward)
        {
            if (forward)
            {
                this->port = NULL;
                ChannelElement<T>::disconnect(true);
            }
            else
            {
                OutputPort<T>* port = this->port;
                if (!port)
                    return;

                port->removeConnection(this);
            }
        }
    };

}

#endif

