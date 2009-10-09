#ifndef ORO_CONN_OUTPUT_ENDPOINT_HPP
#define ORO_CONN_OUTPUT_ENDPOINT_HPP

#include "Channels.hpp"

namespace RTT
{ namespace internal {

    /** This is a channel element that represents the output endpoint of a
     * connection, i.e. the part that is connected to the InputPort.
     * In the RTT, connections are always created from input towards output.
     * So this class is typically first created of the channel element chain
     * and attached to the input port. Then we build further towards the
     * outputport. Imagine a spider attaching a thread at one wall and
     * moving along to the other side of the wall.
     */
    template<typename T>
    class ConnOutputEndpoint : public base::ChannelElement<T>
    {
        InputPort<T>* port;

    public:
        /**
         * Creates the connection end that represents the output and attach
         * it to the input.
         * @param port The start point.
         * @param output_id Each connection must be identified by an ID that
         * represents the other end. This id is passed to the input port \a port.
         * @return
         */
        ConnOutputEndpoint(InputPort<T>* port, base::PortID* output_id = 0)
            : port(port)
        {
            port->startConnection(output_id, this );
        }

        ~ConnOutputEndpoint()
        {
        }
        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool write(typename base::ChannelElement<T>::param_t sample)
        { return false; }

        virtual void disconnect(bool forward)
        {
            InputPort<T>* port = this->port;
            this->port = 0;

            if (forward)
            {
                if (port)
                    port->removeConnection(this);
            }
            else
                base::ChannelElement<T>::disconnect(false);
        }

        virtual bool signal()
        {
            InputPort<T>* port = this->port;
            if (port && port->new_data_on_port_event)
                (*port->new_data_on_port_event)(port);
            return true;
        }

        virtual bool data_sample(typename base::ChannelElement<T>::param_t sample)
        {
            return true;
        }

    };

}}

#endif

