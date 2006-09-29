/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  PortInterface.cxx 

                        PortInterface.cxx -  description
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
 
 

#include "rtt/PortInterface.hpp"


namespace RTT
{

    PortInterface::PortInterface(const std::string& name) : portname(name) {}

    PortInterface::~PortInterface() {}

    bool PortInterface::setName(const std::string& name)
    { 
        if ( !connected() ) {
            portname = name;
            return true;
        }
        return false;
    }

    bool PortInterface::ready() const {
        return this->connected();
    }

    bool PortInterface::connectTo( PortInterface* other ) {
        if ( this->connected() || !other )
            return false;

        // The aim of this function is to create a connection between
        // this and other at all costs.

        // if both are not connected, create a new one:
        if ( other->connection() == 0 ) {
            ConnectionInterface::shared_ptr ci = this->createConnection();
            if (ci) {
                if ( other->connectTo( ci ) )
                    return ci->connect();
                // failed, cleanup.
                this->disconnect();
                return false;
            }
            ci = other->createConnection();
            if (ci) {
                if ( this->connectTo( ci ) )
                    return ci->connect();
                // failed, cleanup.
                other->disconnect();
                return false;
            }
            // if here, both were readers.
            Logger::In in("PortInterface::connectTo");
            log(Warning) << "User requests to connect two Read ports. Creating a temporary default Writer." << endlog();
            // create a writer:
            PortInterface* aclone  = other->antiClone();
            // clumsy way of working:
            ci = aclone->createConnection();
            // a writer must create a connection.
            assert( ci );
            ci->removeWriter(aclone);
            delete aclone;
            
            if ( this->connectTo( ci ) )
                return ci->connect();
            // failed (type mismatch), cleanup.
            other->disconnect();
            return false;
        }
        return this->connectTo( other->connection() );
    }

    ConnectionInterface::shared_ptr PortInterface::createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type )
    {
        return ConnectionInterface::shared_ptr();
    }

    ConnectionInterface::shared_ptr PortInterface::createConnection(ConnectionTypes::ConnectionType con_type )
    {
        return ConnectionInterface::shared_ptr();
    }

    OperationInterface* PortInterface::createPortObject() {
        return 0;
    }

}
