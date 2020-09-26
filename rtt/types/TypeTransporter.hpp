/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:25 CET 2008  TypeTransporter.hpp

                        TypeTransporter.hpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_TYPE_TRANSPORTER_HPP
#define ORO_TYPE_TRANSPORTER_HPP

#include "rtt/base/DataSourceBase.hpp"
#include "rtt/base/BufferBase.hpp"
#include "rtt/base/ChannelElementBase.hpp"

namespace RTT
{

    namespace types {

        /**
         * This interface defines the function a transport protocol
         * must support in order to allow Orocos components to
         * remotely communicate one data type.
         *
         * Typically, a transport will have more functions than these
         * basic functions to implements it's transport-specific
         * needs for each data type.
         */
        class RTT_API TypeTransporter
        {
		public:
            virtual ~TypeTransporter() {}

            /**
             * Creates a streaming channel element for reading or writing over this transport.
             * It returns a ChannelElementBase that provides the implementation of sending or receiving
             * data through the transport. Both sender and receiver find each other using the channel_id
             * argument. Transports that do not support streaming may return null
             *
             * @param port The port for which this channel is setup.
             * @param channel_id If the transport receives a non-empty channel_id, it will create a channel that connects to this id.
             * If channel id is empty, it will be filled in with a unique identifier that identifies this channel.
             * This allows the local caller to connect to the
             * remote channel in a second invocation of createRemoteChannel.
             * @param is_sender Set to true in case you will write() to this channel element, set it to false
             * in case you will read() from this channel element.
             * @return null in case streaming is not supported by this transport or a valid channel element otherwise.
             *
             */
            virtual base::ChannelElementBase::shared_ptr createStream(base::PortInterface* port, const ConnPolicy& policy, bool is_sender) const = 0;
        };
    }
}

#endif
