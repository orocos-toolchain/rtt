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
#ifndef ORO_EMBEDDED  
        try {
#endif
            return this->connected() && this->connection()->getDataSource()->evaluate();
#ifndef ORO_EMBEDDED            
        } catch(...)
        {}
        return false;
#endif
    }

    bool PortInterface::connectTo( PortInterface* other ) {
        if ( this->ready() || !other )
            return false;

        // The aim of this function is to create a connection between
        // this and other at all costs.
        
        /*
           L->connectTo(R);
            1. If both ports are not connected:
              1a) if L is a writer:
                  * create a local server, instruct the remote port to create a proxy
                     which connects to this server.
              1b) if L is a reader:
                   1ba) if R is a writer:
                        * create a remote server, instruct the local port to create a
                           proxy which connects to this server.
                    1bb) if R is a reader:
                        * create a local default server (very ugly with buffers), instruct
                          the remote port to create a proxy which connects to this server
            2. If one of both ports is connected:
               2a) if L is connected
               * create a server on the local side, instruct the remote port to create a
                  proxy which connects to this server
               2b) if R is connected
               *  create a remote server, instruct the local port to create a proxy which
                  connects to this server.
            3. if both ports are connected:
               * fail.
           R->connectTo(L);
            
            The idea is that connections are created from writer to reader, no matter 
            which one is remote or local, because the writer has the initialisation 
            and/or buffer capacity. So where the writer is, the server is created and
            the reader creates a proxy. Once one of remote/local is connected, it is
            easy, always create a proxy to it.
         * 
         */

        // if both are not connected, create a new one:
        if ( !other->ready() ) {
            if ( this->getPortType() != ReadPort ) { // we are a writer
                ConnectionInterface::shared_ptr ci = this->createConnection(); //creates a 'server'
                if (ci) {
                    ci->addPort( other );
                    ci->connect();
                    assert(ci->connected());
                    return true;
                }
                this->disconnect();
                return false;
            }
            if (other->getPortType() != ReadPort ) { // other can write
                ConnectionInterface::shared_ptr ci = other->createConnection(); // creates a 'server'
                if (ci) {
                    ci->addPort(this);
                    ci->connect();
                    assert(ci->connected());
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
            PortInterface* aclone = 0;
            ConnectionInterface::shared_ptr ci;
            // only try this with local ports...
            if ( other->serverProtocol() == 0 ) {
                aclone  = other->antiClone();
                // clumsy way of working:
                ci = aclone->createConnection();
                // this can still fail
                if (!ci) {
                    delete aclone;
                    aclone = 0;
                }
            }
            if ( this->serverProtocol() == 0) {
                aclone = this->antiClone();
                ci = aclone->createConnection();
            }
            if (!ci) {
                log(Warning) <<"Complete failure to connect read ports. Neither port wants to create a connection."<< endlog();
                delete aclone;
                return false;
            }
            // remove clone from new connection.
            ci->removePort(aclone);
            delete aclone;
            
            // finally a connection object !
            // 1. connect other.
            if ( other->connectTo( ci ) == false )
                return false;

            // 2. connect this.
            if ( this->connectTo( ci ) )
                return ci->connect();
            // failed (type mismatch), cleanup.
            other->disconnect();
            return false;
        }
        return this->connectTo( other->connection() );
    }

    ConnectionInterface::shared_ptr PortInterface::createConnection(ConnectionTypes::ConnectionType con_type )
    {
        return ConnectionInterface::shared_ptr();
    }

    ConnectionInterface::shared_ptr PortInterface::createConnection(BufferBase::shared_ptr )
    {
        return ConnectionInterface::shared_ptr();
    }

    ConnectionInterface::shared_ptr PortInterface::createConnection(DataSourceBase::shared_ptr )
    {
        return ConnectionInterface::shared_ptr();
    }

    TaskObject* PortInterface::createPortObject() {
        return 0;
    }

    int PortInterface::serverProtocol() const {
        return 0;
    }
}
