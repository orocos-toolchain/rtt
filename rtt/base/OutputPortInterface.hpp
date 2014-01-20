/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  OutputPortInterface.hpp

                        OutputPortInterface.hpp -  description
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


#ifndef ORO_OUTPUT_PORT_INTERFACE_HPP
#define ORO_OUTPUT_PORT_INTERFACE_HPP

#include "PortInterface.hpp"
#include "../internal/ConnectionManager.hpp"
#include "DataSourceBase.hpp"

namespace RTT
{ namespace base {

    /**
     * The base class of each OutputPort. It contains the connection management code, which is
     * independent of the actual data being transmitted.
     */
    class RTT_API OutputPortInterface : public PortInterface
    {
    protected:
        internal::ConnectionManager cmanager;

        /**
         * Upcall to OutputPort.
         */
        virtual bool connectionAdded(ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy) = 0;

        OutputPortInterface(OutputPortInterface const& orig);
    public:
        /**
         * Adds a new connection to this output port and initializes the connection if required by \a policy.
         * Use with care. Allows you to add any arbitrary connection to this output port. It is your responsibility
         * to do any further bookkeeping, such as informing the input that a new output has been added.
         */
        virtual bool addConnection(internal::ConnID* port_id, ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy);

        OutputPortInterface(std::string const& name);

        virtual ~OutputPortInterface();

        /**
         * Returns true if this port records the last written value.
         */
        virtual bool keepsLastWrittenValue() const = 0;

        /**
         * Change the setting for keeping the last written value.
         * Setting this to false will clear up any unneeded storage.
         * If set, this port can initialize new connections with a data sample and
         * allows real-time data transport of dynamically sized objects
         * over its newly created connections.
         * @see OutputPort::OutputPort.
         */
        virtual void keepLastWrittenValue(bool new_flag) = 0;

        /**
         * Returns a Data source that stores the last written value, or
         * a null pointer if this port does not keep its last written value.
         */
        virtual DataSourceBase::shared_ptr getDataSource() const = 0;

        virtual void disconnect();

        /** Returns true if there is at least one channel registered in this
         * port's list of outputs
         */
        virtual bool connected() const;

        /**
         * Write this port using the value stored in source.
         */
        virtual void write(DataSourceBase::shared_ptr source);

        /** Connects this write port to the given read port, using a single-data
         * policy with the given locking mechanism
         */
        bool createDataConnection( InputPortInterface& sink, int lock_policy = ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using a buffered
         * policy, with the buffer of the given size and the given locking
         * mechanism
         */
        bool createBufferConnection( InputPortInterface& sink, int size, int lock_policy = ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using as policy
         * the default policy of the sink port
         */
        bool createConnection( InputPortInterface& sink );

        /** Connects this write port to the given read port, using the
         * given connection policy.
         */
        virtual bool createConnection( InputPortInterface& sink, ConnPolicy const& policy ) = 0;

        /** Removes the channel that connects this port to \c port */
        virtual bool disconnect(PortInterface* port);

        /** Removes the connection associated with this channel, and the channel
         * as well
         */
        virtual bool removeConnection(internal::ConnID* cid);

        virtual bool connectTo(PortInterface* other, ConnPolicy const& policy);

        virtual bool connectTo(PortInterface* other);

        virtual const internal::ConnectionManager* getManager() const { return &cmanager; }

        /** Sends a write tracepoint */
        void traceWrite();
    };
}}

#endif
