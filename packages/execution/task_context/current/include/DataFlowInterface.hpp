/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  DataFlowInterface.hpp 

                        DataFlowInterface.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_DATA_FLOW_INTERFACE_HPP
#define ORO_EXECUTION_DATA_FLOW_INTERFACE_HPP

#include <vector>
#include <map>
#include <string>
#include "PortInterface.hpp"

namespace RTT
{

    /**
     * The Interface of a TaskContext which exposes its data-flow ports.
     */
    class DataFlowInterface
    {
        typedef std::map<PortInterface*, std::string> Descr;
        Descr descriptions;
        std::vector<PortInterface*> ports;
    public:
        /**
         * A sequence of pointers to ports.
         */
        typedef std::vector<PortInterface*> Ports;

        DataFlowInterface() {}
        ~DataFlowInterface() {
        }

        /**
         * Add a Port to this task. It is only added to the C++
         * interface and can not be used in scripting.
         * @param port The port to add.
         * @return true if the port could be added, false if already added.
         *
         * @internal When added, it is owned by the DataFlowInterface and will be
         * destroyed when removed.
         */
        bool addPort(PortInterface* port) {
            if ( find(ports.begin(), ports.end(), port) != ports.end() )
                return false;
            ports.push_back( port );
            return true;
        }

        /**
         * Add a Port to the interface of this task. It is added to
         * both the C++ interface and the scripting interface.
         * @param port The port to add.
         * @param description A user readable description of this port.
         */
        bool addPort(PortInterface* port, std::string description) {
            if (this->addPort(port) == false)
                return false;
            descriptions[port] = description;
            return true;
        }

        /**
         * Remove a Port from this interface.
         * @param port The port to remove.
         */
        void removePort(const std::string& name) {
            for ( Ports::iterator it(ports.begin());
                 it != ports.end();
                 ++it)
                if ( (*it)->getName() == name ) {
                    ports.erase(it);
                    return;
                }
        }

        /**
         * Get all ports of this interface.
         * @return A sequence of pointers to ports.
         */
        Ports getPorts() const { return ports; }

        /**
         * Get all port names of this interface.
         * @return A sequence of strings containing the port names.
         */
        std::vector<std::string> getPortNames() const { 
            std::vector<std::string> res;
            for ( Ports::const_iterator it(ports.begin());
                 it != ports.end();
                 ++it)
                res.push_back( (*it)->getName() );
            return res;
        }


        /**
         * Get an added port.
         * @param name The port name
         * @return a pointer to a port or null if it does not exist.
         */
        PortInterface* getPort(const std::string& name) const {
            for ( Ports::const_iterator it(ports.begin());
                 it != ports.end();
                 ++it)
                if ( (*it)->getName() == name )
                    return *it;
            return 0;
        }

        /** 
         * Get the description of an added Port.
         * 
         * @param name The port name
         * 
         * @return The description or "" if it does not exist.
         */
        std::string getPortDescription(const std::string& name) const {
            for ( Descr::const_iterator it(descriptions.begin());
                  it != descriptions.end();
                  ++it)
                if ( it->first->getName() == name )
                    return it->second;
            return "";
        }


        /**
         * Get a port of a specific type.
         */
        template< class Type>
        Type* getPortType(const std::string& name) {
            return dynamic_cast<Type*>( this->getPort(name) );
        }
    };

}

#endif
