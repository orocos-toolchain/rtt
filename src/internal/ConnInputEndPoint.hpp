#ifndef ORO_CONN_INPUT_ENDPOINT_HPP
#define ORO_CONN_INPUT_ENDPOINT_HPP

#include "Channels.hpp"

namespace RTT
{ namespace internal {

    /** This is a channel element that represents the input endpoint of a
     * connection, i.e. the part that is connected to the OutputPort
     */
    template<typename T>
    class ConnInputEndpoint : public base::ChannelElement<T>
    {
        OutputPort<T>* port;
        ConnID* cid;

    public:
        ConnInputEndpoint(OutputPort<T>* port, ConnID* id)
            : port(port), cid(id) { }

        ~ConnInputEndpoint()
        {
            delete cid;
        }

        /** Reads a new sample from this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual FlowStatus read(typename base::ChannelElement<T>::reference_t sample)
        { return NoData; }

        virtual bool inputReady() {
            return true;
        }

        virtual void disconnect(bool forward)
        {
            if (forward)
            {
                this->port = NULL;
                base::ChannelElement<T>::disconnect(true);
            }
            else
            {
                OutputPort<T>* port = this->port;
                if (!port)
                    return;

                port->removeConnection( cid );
            }
        }
    };

}}

#endif

