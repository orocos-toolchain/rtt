/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  ConnectionInterface.hpp 

                        ConnectionInterface.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_CONNECTION_INTERFACE_HPP
#define ORO_EXECUTION_CONNECTION_INTERFACE_HPP

#include <boost/intrusive_ptr.hpp>
#include <os/fosi.h>

namespace ORO_Execution
{
    class ConnectionInterface;
}

void intrusive_ptr_add_ref( ORO_Execution::ConnectionInterface* p );
void intrusive_ptr_release( ORO_Execution::ConnectionInterface* p );

namespace ORO_Execution
{
    class PortInterface;

    /**
     * An Object which manages and maintains a communication connection
     * between two or more Ports.
     * @see PortInterface
     */
    struct ConnectionInterface
    {
    protected:
        friend void ::intrusive_ptr_add_ref( ConnectionInterface* p );
        friend void ::intrusive_ptr_release( ConnectionInterface* p );
        atomic_t refcount;
    public:
        
        typedef boost::intrusive_ptr<ConnectionInterface> shared_ptr;

        ConnectionInterface();
        virtual ~ConnectionInterface();

        /**
         * Connect all readers and writers.
         */
        virtual bool connect() = 0;

        /**
         * Get Connection status.
         */
        virtual bool connected() const = 0;

        /**
         * Disconnect all readers and writers.
         */
        virtual bool disconnect() = 0;

        virtual bool addReader(PortInterface* r) = 0;

        virtual bool removeReader(PortInterface* r) = 0;

        virtual bool addWriter(PortInterface* r) = 0;

        virtual bool removeWriter(PortInterface* r) = 0;
    };
}

#endif
