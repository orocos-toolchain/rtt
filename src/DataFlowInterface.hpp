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
#include "PortInterface.hpp"

namespace RTT
{

    /**
     * The Interface of a TaskContext which exposes its data-flow ports.
     * @ingroup Ports
     */
    class DataFlowInterface
    {
        typedef std::vector<std::pair<PortInterface*,std::string> > PortStore;
        PortStore mports;
        OperationInterface* mparent;
    public:
        /**
         * A sequence of pointers to ports.
         */
        typedef std::vector<PortInterface*> Ports;

        /**
         * A sequence of names of ports.
         */
        typedef std::vector<std::string> PortNames;

        /**
         * Construct the DataFlow interface of a TaskContext.
         * @param parent If not null, a TaskObject will be added
         * to \a parent  for each port added to this interface.
         */
        DataFlowInterface(OperationInterface* parent = 0);

        ~DataFlowInterface();

        /**
         * Add a Port to this task. It is only added to the C++
         * interface and can not be used in scripting.
         * @param port The port to add.
         * @return true if the port could be added, false if already added.
         *
         */
        bool addPort(PortInterface* port);

        /**
         * Add a Port to the interface of this task. It is added to
         * both the C++ interface and the scripting interface.
         * @param port The port to add.
         * @param description A user readable description of this port.
         */
        bool addPort(PortInterface* port, std::string description);

        /**
         * Remove a Port from this interface.
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
         * Get all port names of this interface.
         * @return A sequence of strings containing the port names.
         */
        PortNames getNames() const;

        /**
         * Get an added port.
         * @param name The port name
         * @return a pointer to a port or null if it does not exist.
         */
        PortInterface* getPort(const std::string& name) const;

        /** 
         * Get the description of an added Port.
         * 
         * @param name The port name
         * 
         * @return The description or "" if it does not exist.
         */
        std::string getPortDescription(const std::string& name) const;

        /**
         * Create a Task Object through which one can access a Port.
         * This is required to access ports from the scripting interface.
         * @param name The port name
         * @deprecated Do not use this function. It is no longer required.,
         * the objects are directly added to the parent TaskContext in \a addPort.
         */
        OperationInterface* createPortObject(const std::string& name);

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
    };

}

#endif
