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
#include "PortInterface.hpp"

namespace ORO_Execution
{

    /**
     * The Interface of a TaskContext which exposes its data-flow ports.
     */
    class DataFlowInterface
    {
    public:
        typedef std::vector<PortInterface*> Ports;
        Ports ports;

        DataFlowInterface() {}
        ~DataFlowInterface() {
//             for ( Ports::iterator it(ports.begin()); it != ports.end(); ++it)
//                 delete *it;
        }

        /**
         * Add a Port to this task.
         * @internal When added, it is owned by the DataFlowInterface and will be
         * destroyed when removed.
         */
        void addPort(PortInterface* port) { ports.push_back( port ); }

        /**
         * Remove a Port from this task.
         */
        void removePort(const std::string& name) {
            for ( Ports::iterator it(ports.begin());
                 it != ports.end();
                 ++it)
                if ( (*it)->getName() == name )
                    ports.erase(it);
        }

        /**
         * Get all ports of this task.
         */
        Ports getPorts() const { return ports; }

        /**
         * Get all port names of this task.
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
         * Get a port.
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
         * Get a port of a specific type.
         */
        template< class Type>
        Type* getPortType(const std::string& name) {
            return dynamic_cast<Type*>( this->getPort(name) );
        }
    };

}

#endif
