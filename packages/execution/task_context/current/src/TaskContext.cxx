/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TaskContext.cxx 

                        TaskContext.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
 

#include "execution/TaskContext.hpp"
#include <corelib/CommandInterface.hpp>
#include <corelib/CompletionProcessor.hpp>

#include "execution/Factories.hpp"

#include <string>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "execution/DataSource.hpp"
#include "execution/ConnectionInterface.hpp"

#include <pkgconf/os.h>
#include <pkgconf/system.h>
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
#include "execution/ParserScriptingAccess.hpp"
#endif

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    using namespace boost;
    using namespace std;

    TaskContext::TaskContext(const std::string& name)
        :  TaskCore(name),
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
           mscriptAcc(new ParserScriptingAccess(this)),
#else
           mscriptAcc(new ScriptingAccess(this)),
#endif
           commandFactory( &ee ),
           eventService( &ee )
    {
        // I'll only add  this line if there is  a good reason for
        // (there  isn't) for  now, it's  confusing to  see 'this'
        // being  listed as  peer.   while there  is  also a  this
        // object.  //_task_map[ "this" ] = this;
    }

    TaskContext::TaskContext(const std::string& name, ExecutionEngine* parent )
        :  TaskCore(name, parent),
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
           mscriptAcc(new ParserScriptingAccess(this)),
#else
           mscriptAcc(new ScriptingAccess(this)),
#endif
           commandFactory( &ee ),
           eventService( &ee )
    {
    }


        TaskContext::~TaskContext()
        {
            attributeRepository.clear();
            delete mscriptAcc;

            // remove from all users.
            while( !musers.empty() ) {
                musers.front()->removePeer(this);
            }
            // since we are destroyed, be sure that the peer no longer
            // has a 'user' pointer to us.
            while ( !_task_map.empty() ) {
                _task_map.begin()->second->removeUser(this);
                _task_map.erase( _task_map.begin() );
            }
            // Do not call this->disconnect() !!!
            // Ports are probably already destructed by user code.
        }

    void TaskContext::exportPorts()
    {
        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {
            if ( this->datasources()->getObjectFactory( (*it)->getName() ) == 0 ) {
                // Add the port to the method interface.
                DataSourceFactoryInterface* ms =  (*it)->createDataSources();
                if ( ms )
                    this->datasources()->registerObject( (*it)->getName(), ms );
            }
        }
    }

    void TaskContext::connectDataFlow( TaskContext* peer )
    {
        const std::string& location = this->getName();
        Logger::In in( location.c_str()  );
        // connect our write ports to the read ports of 'peer'.
        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {
            // Then try to get the peer port's connection
            PortInterface* peerport = peer->ports()->getPort( (*it)->getName() );
            if ( !peerport ) {
                Logger::log() <<Logger::Debug<< "Peer Task "<<peer->getName() <<" has no Port " << (*it)->getName() << Logger::endl;
                continue;
            }

            // Detect already connected port.
            if ( (*it)->connection() ) {
                // ask peer to connect to us:
                if ( peerport->connectTo( *it ) ) {
                    Logger::log() <<Logger::Info<< "Connected Port " << (*it)->getName()
                                  << "of peer Task "<<peer->getName() << " to existing connection." << Logger::endl;
                }
                else
                    Logger::log() <<Logger::Error<< "Failed to connect Port " << (*it)->getName()
                                  << " of peer Task "<<peer->getName() << " to existing connection." << Logger::endl;
                continue;
            }
            // OK, not yet connected, try to connect.

            // detect existing peer port connection.
            if ( peerport->connection() ) {
                if ( (*it)->connectTo( peerport ) ) {
                    Logger::log() <<Logger::Info<< "Added Port " << (*it)->getName()
                                  << " to existing connection of peer Task "<<peer->getName() << "." << Logger::endl;
                }
                else
                    Logger::log() <<Logger::Error<< "Failed to connect Port " << (*it)->getName()
                                  << " to existing connection of peer Task "<<peer->getName() << "." << Logger::endl;
                continue;
            }
                
            // Last resort: create new connection.
            ConnectionInterface::shared_ptr con = (*it)->createConnection( peerport );
            if ( !con ) {
                // real error msg will be produced by factory itself.
                Logger::log() <<Logger::Debug<< "Failed to connect Port " << (*it)->getName() << " to peer Task "<<peer->getName() <<"." << Logger::endl;
            } else {
                con->connect();
                Logger::log() <<Logger::Info<< "Connected Port " << (*it)->getName() << " to peer Task "<<peer->getName() <<"." << Logger::endl;
            }
        }
    }

        bool TaskContext::executeCommand( CommandInterface* c)
        {
            return ee.getCommandProcessor()->process( c ) != 0;
        }

        int TaskContext::queueCommand( CommandInterface* c)
        {
            return ee.getCommandProcessor()->process( c );
        }

    void TaskContext::addUser( TaskContext* peer )
    {
        musers.push_back(peer);
    }

    void TaskContext::removeUser( TaskContext* peer )
    {
        Users::iterator it = find(musers.begin(), musers.end(), peer);
        if ( it != musers.end() )
            musers.erase(it);
    }

        bool TaskContext::addPeer( TaskContext* peer, std::string alias )
        {
            if ( alias.empty() )
                alias = peer->getName();
            if ( _task_map.count( alias ) != 0 )
                return false;
            _task_map[ alias ] = peer;
            peer->addUser( this );
            this->connectDataFlow(peer);
            this->exportPorts();
            peer->exportPorts();
            return true;
        }

        void TaskContext::removePeer( const std::string& name )
        {
            PeerMap::iterator it = _task_map.find( name );
            if ( _task_map.end() != it ) {
                it->second->removeUser( this );
                _task_map.erase( _task_map.find( name ) );
            }
        }

        void TaskContext::removePeer( TaskContext* peer )
        {
            for( PeerMap::iterator it = _task_map.begin(); it != _task_map.end(); ++it)
                if ( it->second == peer ) {
                    peer->removeUser( this );
                    _task_map.erase( it );
                    return;
                }
        }

        bool TaskContext::connectPeers( TaskContext* peer )
        {
            if ( _task_map.count( peer->getName() ) != 0
                 || peer->hasPeer( _task_name ) )
                return false;
            this->addPeer ( peer );
            peer->addPeer ( this );
            return true;
        }

    void TaskContext::reconnect()
    {
        Logger::In in( this->getName().c_str()  );
        Logger::log() << Logger::Info << "Starting reconnection..."<<Logger::endl;
        // first disconnect all our ports
        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {
            (*it)->disconnect();
            this->datasources()->unregisterObject( (*it)->getName() );
        }
        // reconnect again to our peers and ask our 'users' to reconnect as well.
        for( PeerMap::iterator it = _task_map.begin(); it != _task_map.end(); ++it)
            this->connectDataFlow(it->second);
        this->exportPorts();

        for( Users::iterator it = musers.begin(); it != musers.end(); ++it) {
            (*it)->connectDataFlow(this);
            (*it)->exportPorts();
        }

        Logger::log() << Logger::Info << "Reconnection done."<<Logger::endl;
    }

    void TaskContext::disconnect() {
        Logger::In in( this->getName().c_str()  );
        // disconnect all our ports
        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {
            (*it)->disconnect();
            this->datasources()->unregisterObject( (*it)->getName() );
        }

        // remove from all users.
        while( !musers.empty() ) {
            musers.front()->removePeer(this);
        }

        while ( !_task_map.empty() ) {
            _task_map.begin()->second->removeUser(this);
            _task_map.erase( _task_map.begin() );
        }
    }

        void TaskContext::disconnectPeers( const std::string& name )
        {
            if ( _task_map.end() != _task_map.find( name ) ) {
                TaskContext* peer = _task_map.find(name)->second;
                this->removePeer(peer);
                peer->removePeer(this);
            }
        }

        std::vector<std::string> TaskContext::getPeerList() const
        {
            std::vector<std::string> res;
            std::transform(_task_map.begin(), _task_map.end(),
                           std::back_inserter( res ),
                           ORO_std::select1st<PeerMap::value_type>() );
            return res;
        }

        bool TaskContext::hasPeer( const std::string& peer_name ) const
        {
            return _task_map.count( peer_name ) == 1;
        }

        TaskContext* TaskContext::getPeer(const std::string& peer_name ) const
        {
            if (this->hasPeer( peer_name ) )
                return _task_map.find(peer_name)->second;
            return 0;
        }

}

