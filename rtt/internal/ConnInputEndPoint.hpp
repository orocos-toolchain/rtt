/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnInputEndPoint.hpp

                        ConnInputEndPoint.hpp -  description
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


#ifndef ORO_CONN_INPUT_ENDPOINT_HPP
#define ORO_CONN_INPUT_ENDPOINT_HPP

#include "Channels.hpp"
#include "PortConnectionLock.hpp"

#include "../Logger.hpp"

namespace RTT
{ namespace internal {

    /** This is a channel element that represents the input endpoint of a
     * connection, i.e. the part that is connected to the OutputPort
     */
    template<typename T>
    class ConnInputEndpoint : public base::MultipleOutputsChannelElement<T>
    {
    private:
        OutputPort<T>* port;

    public:
        typedef base::MultipleOutputsChannelElement<T> Base;
        typedef boost::intrusive_ptr<ConnInputEndpoint<T> > shared_ptr;

        ConnInputEndpoint(OutputPort<T>* port)
            : port(port) { }

        ~ConnInputEndpoint()
        {}

        virtual bool inputReady(base::ChannelElementBase::shared_ptr const&) {
            return true;
        }

        using Base::disconnect;

        virtual bool disconnect(const base::ChannelElementBase::shared_ptr& channel, bool forward)
        {
            // Lock port connections if the request is coming from the remote end (forward == false)
            PortConnectionLock lock((channel && !forward) ? port : 0);

            // Call the base class: it does the common cleanup
            if (!Base::disconnect(channel, forward)) {
//                log(Error) << "Failed to disconnect an output channel from a ConnInputEndPoint for port " << port->getName() << "." << endlog();
                return false;
            }

            // This method is called recursively from MultipleOutputsChannelElementBase::disconnectSingleOutputChannel()
            // if the removed output was the last one. In this case the call to the base class above already removed the
            // shared buffer and we can exit here to avoid a dead-lock on locking the mutex twice.
            if (!channel && !forward) return true;

            // Otherwise (forward == true) lock port connections now.
            PortConnectionLock lock2(!lock ? port : 0);

            // If this was the last connection, remove the buffer, too.
            // For forward == false this was already done by the base class.
            if (!this->connected() && forward) {
                if (getSharedBuffer()) {
                    log(Debug) << "Removing the shared output buffer for port " << port->getName() << " in ConnInputEndPoint::disconnect()." << endlog();
                }
                Base::disconnect(0, !forward /*= false*/);
            }

            // Disconnect from a shared connection
            SharedConnectionBase::shared_ptr shared_connection = port->getSharedConnection();
            if (shared_connection && (channel == shared_connection)) {
                log(Debug) << "Port " << port->getName() << " disconnected from shared connection " << shared_connection->getName() << endlog();
                port->setDefaultSharedConnection();
                shared_connection.reset();
            }

            // If the request is coming from the remote end (forward == false),
            // also notify the ConnectionManager.
            if (port && channel && !forward)
            {
                port->getManager()->removeConnection(channel.get(), /* disconnect = */ false);
            }

            return true;
        }

        virtual base::PortInterface* getPort() const {
            return this->port;
        }

        virtual base::ChannelElementBase::shared_ptr getInputEndPoint()
        {
            return this;
        }

        virtual typename base::ChannelElement<T>::shared_ptr getSharedBuffer()
        {
            return this->getInput();
        }

        typename base::ChannelElement<T>::shared_ptr getWriteEndpoint()
        {
            typename base::ChannelElement<T>::shared_ptr buffer = getSharedBuffer();
            if (buffer) {
                return buffer;
            } else {
                return this;
            }
        }

        std::string getElementName() const {
            return std::string("ConnInputEndpoint");
        }

    };

}}

#endif

