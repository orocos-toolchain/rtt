/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  PortInterface.cxx 

                        PortInterface.cxx -  description
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
 
 

#include "PortInterface.hpp"


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
                if ( other->connectTo( ci ) && ci->connect() ) {
                    return true;
                }
                // failed, cleanup.
                this->disconnect();
                return false;
            }
            ci = other->createConnection();
            if (ci) {
                if ( this->connectTo( ci ) && ci->connect() ){
                    return true;
                }
                // failed, cleanup.
                other->disconnect();
                return false;
            }

            // if here, both were readers or both refused to create a connection.
            Logger::In in("PortInterface::connectTo");
            log(Warning) << "Creating a temporary default Writer." << endlog();
            // create a writer:
            PortInterface* aclone  = other->antiClone();
            // clumsy way of working:
            ci = aclone->createConnection();
            // this can still fail
            if (!ci) {
                delete aclone;
                aclone = this->antiClone();
                ci = aclone->createConnection();
                if (!ci) {
                    log(Warning) <<"Complete failure to connect read ports. Neither port wants to create a connection."<< endlog();
                    delete aclone;
                    return false;
                }
            }
            ci->removeWriter(aclone);
            delete aclone;
            
            // finally a connection object !

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

    TaskObject* PortInterface::createPortObject() {
        return 0;
    }

}
