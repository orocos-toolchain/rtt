/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  PortInterface.hpp 

                        PortInterface.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
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
 
 
#ifndef ORO_EXECUTION_PORT_INTERFACE_HPP
#define ORO_EXECUTION_PORT_INTERFACE_HPP

#include <string>
#include "ConnectionTypes.hpp"
#include "ConnectionInterface.hpp"
#include "TaskObject.hpp"

namespace RTT
{

    /** 
     * A Port of a task is used to connect it with other tasks
     * using a Connector. The Connector can be a Buffer or a
     * DataObject.
     */
    class PortInterface
    {
    protected:
        std::string portname;
        PortInterface(const std::string& name);
    public:
        /**
         * This enum classifies if a port is inbound
         * outbound or both.
         */
        enum PortType { ReadPort, WritePort, ReadWritePort };
 
        virtual ~PortInterface();

        /**
         * Get the name of this Port.
         */
        const std::string& getName() const { return portname;}

        /**
         * Change the name of this unconnected Port.
         * One can only change the name when it is not yet connected.
         * @retval true if !this->connected(), the name has changed.
         * @retval false if this->connected(), the name has not been changed.
         */
        bool setName(const std::string& name);

        /**
         * Returns true if this Port is ready to be used.
         * Identical to connected().
         */
        bool ready() const;

        /**
         * Get the PortType of this port.
         */
        virtual PortType getPortType() const = 0;

        /**
         * Inspect if this Port is currently connected with another Port.
         */
        virtual bool connected() const = 0;

        /**
         * Returns the connection in which this Port currently participates.
         * It is possible that this port is part of a dormant connection,
         * ( the connection object is not in the connected() state),
         * in that case this method will return null, until the dormant
         * connection becomes connected().
         * @retval null if not connected to another Port.
         */
        virtual ConnectionInterface::shared_ptr connection() const = 0;

        /**
         * Connect this port to a Connection.
         * If the connection is in the connected() state, this port
         * will participate in that connection, otherwise, the port
         * will become connected once \a conn becomes connected().
         * @return true if connection was possible, false otherwise.
         */
        virtual bool connectTo( ConnectionInterface::shared_ptr conn ) = 0;

        /**
         * Connect to another Port and create a new connection if necessary.
         * - If this port is already connected, this method returns false.
         * - If the other port has a connection, this method is equivalent to
         * 'this->connectTo( other->connection() )'.
         * - If the other port is
         * not connected, a new connection is created and both ports are connected.
         * @return true upon success, false otherwise.
         */
        virtual bool connectTo( PortInterface* other );

        /**
         * No longer participate in a connection.
         */
        virtual void disconnect() = 0;

        /**
         * Create a clone of this port with the same name
         */
        virtual PortInterface* clone() const = 0;

        /**
         * Create the anti-clone (inverse port) of this port with the same name
         * A port for reading will return a new port for writing and
         * vice versa.
         */
        virtual PortInterface* antiClone() const = 0;

        /**
         * Create a connection object from this port to another port.
         */
        virtual ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree);

        /**
         * Create a new connection object to which this port is subscribed.
         */
        virtual ConnectionInterface::shared_ptr createConnection(ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree);

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual TaskObject* createPortObject();

    };

}

#endif
