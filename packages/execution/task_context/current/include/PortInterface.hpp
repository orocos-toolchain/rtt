/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  PortInterface.hpp 

                        PortInterface.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
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
#include "DataSourceFactoryInterface.hpp"

namespace ORO_Execution
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
         * Inspect if this Port is connected with another Port.
         */
        virtual bool connected() const = 0;

        /**
         * Returns the connection in which this Port participates.
         * @retval 0 if not connected to another Port.
         */
        virtual ConnectionInterface::shared_ptr connection() const = 0;

        /**
         * Connect this port to a Connection.
         * @return true if connection was possible, false otherwise.
         */
        virtual bool connectTo( ConnectionInterface::shared_ptr conn ) = 0;

        /**
         * Connect to another Port, which has an existing Connection.
         * This method exists only for convenience only and is equivalent
         * to \a connectTo( \a other->connection() ).
         * If \a other does not yet participate in a Connection, this method fails.
         * @return true upon success, false otherwise.
         */
        virtual bool connectTo( PortInterface* other );

        /**
         * No longer participate in a connection.
         */
        virtual void disconnect() = 0;

        /**
         * Create a connection to another port.
         */
        virtual ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree);

        /**
         * Create accessor DataSources for this Port, for addition to a
         * DataSource interface.
         */
        virtual DataSourceFactoryInterface* createDataSources();
    };

}

#endif
