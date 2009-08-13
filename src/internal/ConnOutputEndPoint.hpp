#ifndef ORO_CONN_OUTPUT_ENDPOINT_HPP
#define ORO_CONN_OUTPUT_ENDPOINT_HPP

#include "Channels.hpp"

namespace RTT
{
    template<typename T> class InputPort;

    /** This is a channel element that represents the output endpoint of a
     * connection, i.e. the part that is connected to the InputPort
     */
    template<typename T>
    class ConnOutputEndpoint : public ChannelElement<T>
    {
        InputPort<T>* port;

    public:
        ConnOutputEndpoint(InputPort<T>* port)
            : port(port)
        {
            port->setInputChannel(this);
        }

        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool write(typename ChannelElement<T>::param_t sample)
        { return false; }

        virtual void disconnect(bool forward)
        {
            InputPort<T>* port = this->port;
            this->port = 0;

            if (forward)
            {
                if (port)
                    port->clearInputChannel();
            }
            else
                ChannelElement<T>::disconnect(false);
        }

        virtual bool signal() const
        {
            InputPort<T>* port = this->port;
            if (port && port->new_data_on_port_event)
                (*port->new_data_on_port_event)(port);
            return true;
        }
    };

}

#endif

