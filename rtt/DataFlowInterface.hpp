/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  DataFlowInterface.hpp

                        DataFlowInterface.hpp -  description
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


#ifndef ORO_EXECUTION_DATA_FLOW_INTERFACE_HPP
#define ORO_EXECUTION_DATA_FLOW_INTERFACE_HPP

#include <vector>
#include <map>
#include <string>
#include "base/InputPortInterface.hpp"
#include "base/OutputPortInterface.hpp"
#include "rtt-fwd.hpp"

namespace RTT
{

    /**
     * The Interface of a TaskContext which exposes its data-flow ports.
     * @ingroup Ports
     */
    class RTT_API DataFlowInterface
    {
    public:
        /**
         * A sequence of pointers to ports.
         */
        typedef std::vector<base::PortInterface*> Ports;

        /**
         * A sequence of names of ports.
         */
        typedef std::vector<std::string> PortNames;

        /**
         * Construct the DataFlow interface of a Service.
         * @param parent If not null, a Service will be added
         * to \a parent  for each port added to this interface.
         */
        DataFlowInterface(Service* parent = 0 );

        ~DataFlowInterface();

        /**
         * Name and add a Port to the interface of this task and
         * add a Service with the same name of the port.
         * @param name The name to give to the port.
         * @param port The port to add.
         */
        base::PortInterface& addPort(const std::string& name, base::PortInterface& port) {
            port.setName(name);
            return addPort(port);
        }

        /**
         * Add a Port to the interface of this task and
         * add a Service with the same name of the port.
         * If a port or service with the name already exists, addPort
         * will replace them with \a port and log a warning.
         * @param port The port to add.
         * @return \a port
         */
        base::PortInterface& addPort(base::PortInterface& port);

        /**
         * Name and add an Event triggering Port to the interface of this task and
         * add a Service with the same name of the port.
         * @param name The name to give to the port.
         * @param port The port to add.
         * @param callback (Optional) provide a function which will be called asynchronously
         * when new data arrives on this port. You can add more functions by using the port
         * directly using base::PortInterface::getNewDataOnPort().
         */
        base::InputPortInterface& addEventPort(const std::string& name, base::InputPortInterface& port, base::InputPortInterface::NewDataOnPortEvent::SlotFunction callback = base::InputPortInterface::NewDataOnPortEvent::SlotFunction() ) {
            port.setName(name);
            return addEventPort(port,callback);
        }

        /**
         * Add an Event triggering Port to the interface of this task and
         * add a Service with the same name of the port.
         * When data arrives on this port your TaskContext will be woken up
         * and updateHook will be executed.
         * @param port The port to add.
         * @param callback (Optional) provide a function which will be called
         * when new data arrives on this port. The callback function will
         * be called in sequence with updateHook(), so asynchronously with
         * regard to the arrival of data on the port.
         * @return \a port
         */
        base::InputPortInterface& addEventPort(base::InputPortInterface& port, base::InputPortInterface::NewDataOnPortEvent::SlotFunction callback = base::InputPortInterface::NewDataOnPortEvent::SlotFunction() );

        /**
         * Remove a Port from this interface.
         * This will remove all connections and callbacks
         * assosiated with this port.
         * @param port The port to remove.
         */
        void removePort(const std::string& name);

        /**
         * Get all ports of this interface.
         * @return A sequence of pointers to ports.
         */
        Ports getPorts() const;

        /**
         * Get all port names of this interface.
         * @return A sequence of strings containing the port names.
         * @deprecated by getNames()
         */
        PortNames getPortNames() const;

        /**
         * Get an added port.
         * @param name The port name
         * @return a pointer to a port or null if it does not exist.
         */
        base::PortInterface* getPort(const std::string& name) const;

        /**
         * Get the description of an added Port.
         *
         * @param name The port name
         *
         * @return The description or "" if it does not exist.
         */
        std::string getPortDescription(const std::string& name) const;

        /**
         * Returns the component this interface belongs to.
         */
        TaskContext* getOwner() const;

        /**
         * Add a Port to this task without registering a service for it.
         * If a port with the same name already exists, addPort
         * will replace it with \a port and log a warning.
         * @return \a port
         */
        base::PortInterface& addLocalPort(base::PortInterface& port);

        /**
         * Add an Event triggering Port to this task without
         * registering a service for it.
         * When data arrives on this port your TaskContext will be woken up
         * and updateHook will be executed.
         * @param port The port to add.
         * @param callback (Optional) provide a function which will be called
         * when new data arrives on this port. The callback function will
         * be called in sequence with updateHook(), so asynchronously with
         * regard to the arrival of data on the port.
         * @return \a port
         */
        base::InputPortInterface& addLocalEventPort(base::InputPortInterface& port,
                base::InputPortInterface::NewDataOnPortEvent::SlotFunction callback = base::InputPortInterface::NewDataOnPortEvent::SlotFunction() );

        /**
         * Get a port of a specific type.
         */
        template< class Type>
        Type* getPortType(const std::string& name) {
            return dynamic_cast<Type*>( this->getPort(name) );
        }

        /**
         * Remove all added ports from this interface and
         * all associated TaskObjects.
         */
        void clear();

        /**
         * Called by TaskContext::start() to setup all triggers of EventPorts.
         */
        void setupHandles();
        /**
         * Called by TaskContext::stop() to remove all triggers of EventPorts.
         */
        void cleanupHandles();
    protected:
        /**
         * Create a Service through which one can access a Port.
         * @param name The port name
         */
        Service* createPortObject(const std::string& name);

        /**
         * All our ports.
         */
        Ports mports;
        /**
         * The parent Service. May be null in exceptional cases.
         */
        Service* mservice;
        /**
         * These handles contain the links from an event port's signal to
         * the TaskContext::dataOnPort method.
         */
        typedef std::vector< Handle > Handles;
        Handles handles;


    };

}

#endif
