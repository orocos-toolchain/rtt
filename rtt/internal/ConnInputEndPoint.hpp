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
            //this->disconnect(false); // inform port (if any) we're gone.
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
            // Call the base class first
            base::ChannelElement<T>::disconnect(forward);

            OutputPort<T>* port = this->port;
            if (port && !forward)
            {
                this->input  = 0;
                this->output = 0;
                this->port   = 0;
                port->removeConnection( cid );
            }
        }
    };

}}

#endif

