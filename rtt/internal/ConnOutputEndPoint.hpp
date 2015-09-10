/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnOutputEndPoint.hpp

                        ConnOutputEndPoint.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Sylvain Joyeux
    email                : sylvain.joyeux@m4x.org

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_CONN_OUTPUT_ENDPOINT_HPP
#define ORO_CONN_OUTPUT_ENDPOINT_HPP

#include "Channels.hpp"
#include "ConnID.hpp"

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
    class ConnOutputEndpoint : public base::MultipleInputsChannelElement<T>
    {
    private:
        InputPort<T>* port;

    public:
        typedef base::MultipleInputsChannelElement<T> Base;
        typedef boost::intrusive_ptr<ConnOutputEndpoint<T> > shared_ptr;

        /**
         * Creates the connection end that represents the output and attach
         * it to the input.
         * @param port The start point.
         * @param output_id Each connection must be identified by an ID that
         * represents the other end. This id is passed to the input port \a port.
         * @return
         */
        ConnOutputEndpoint(InputPort<T>* port)
            : port(port)
        {
        }

        ~ConnOutputEndpoint()
        {
        }

        /** Called by the connection factory to check that the connection is
         * properly set up. It is called when the channel is complete, so we can
         * register ourselves on the port side now
         *
         * Before that, the channel might not be complete and therefore having
         * the input port read on it would lead to crashes
         */
        bool inputReady()
        {
            return Base::inputReady();
        }

        /** Writes a new sample on this connection
         * This should only be called if this endpoint has a buffer output,
         * in which case the base class's write implementation will return true
         * and the port is signalled. Otherwise, return false, as other type of
         * connections are supposed to have a data storage element. */
        virtual bool write(typename Base::param_t sample)
        {
            if (Base::write(sample)) {
                return signal();
            }
            return false;
        }

        virtual bool disconnect(bool forward, const base::ChannelElementBase::shared_ptr& channel)
        {
            // Call the base class: it does the common cleanup
            if (Base::disconnect(forward, channel)) {
                // Our buffer output may be gone now...
                InputPort<T>* port = this->port;
                if (port && forward)
                {
                    port->removeConnection(channel.get());
                }
                return true;
            }
            return false;
        }

        virtual bool signal()
        {
            InputPort<T>* port = this->port;
#ifdef ORO_SIGNALLING_PORTS
            if (port && port->new_data_on_port_event)
                (*port->new_data_on_port_event)(port);
#else
            if (port )
                port->signal();
#endif
            return true;
        }

        using Base::data_sample;
        virtual bool data_sample(typename base::ChannelElement<T>::param_t sample)
        {
            typename base::ChannelElement<T>::shared_ptr output = this->getOutput();
            if (output) {
                output->data_sample(sample);
            }
            return true;
        }

        virtual base::PortInterface* getPort() const {
            return this->port;
        }

        virtual base::ChannelElementBase::shared_ptr getOutputEndPoint()
        {
            return this;
        }
    };

}}

#endif

