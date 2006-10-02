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
 
 

#include "rtt/TaskContext.hpp"
#include <rtt/CommandInterface.hpp>

#include <string>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "rtt/DataSource.hpp"
#include "rtt/Method.hpp"
#include "rtt/ConnectionInterface.hpp"

#include <pkgconf/os.h>
#include <pkgconf/system.h>
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
#include "rtt/ParserScriptingAccess.hpp"
#include "rtt/ParserExecutionAccess.hpp"
#endif
#include "rtt/MarshallingAccess.hpp"

namespace RTT
{
    
    using namespace boost;
    using namespace std;

    TaskContext::TaskContext(const std::string& name)
        :  TaskCore(name)
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
           ,mscriptAcc(new ParserScriptingAccess(this))
#else
           ,mscriptAcc(new ScriptingAccess(this))
#endif
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
           ,mengAcc( new ParserExecutionAccess(this ) )
#else
           ,mengAcc( new ExecutionAccess(this ) )
#endif
           ,marshAcc( new MarshallingAccess(this) )
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
           ,eventService( ee )
#endif
    {
        this->setup();
    }

    TaskContext::TaskContext(const std::string& name, ExecutionEngine* parent )
        :  TaskCore(name, parent)
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
           ,mscriptAcc(new ParserScriptingAccess(this))
#else
           ,mscriptAcc(new ScriptingAccess(this))
#endif
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
           ,mengAcc( new ParserExecutionAccess(this ) )
#else
           ,mengAcc( new ExecutionAccess(this ) )
#endif
           ,marshAcc( new MarshallingAccess(this) )
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
           ,eventService( ee )
#endif
    {
        this->setup();
    }

    void TaskContext::setup()
    {
        mdescription = "The interface of this TaskContext.";

        this->methods()
            ->addMethod( method("start",&TaskContext::start, this),
                         "Start the Execution Engine of this TaskContext." );
        this->methods()
            ->addMethod( method("stop",&TaskContext::stop, this),
                         "Stop the Execution Engine of this TaskContext." );
        this->methods()
            ->addMethod( method("isRunning",&TaskContext::isRunning, this),
                         "Is the Execution Engine of this TaskContext started ?" );
        this->methods()
            ->addMethod( method("update",&TaskContext::doUpdate, this),
                         "Execute (call) the update method directly.\n Only succeeds if the task isRunning() and allowed by the Activity executing this task." );

        this->methods()
            ->addMethod( method("trigger",&TaskContext::doTrigger, this),
                         "Trigger the update method for execution in the thread of this task.\n Only succeeds if the task isRunning() and allowed by the Activity executing this task." );
    }

        TaskContext::~TaskContext()
        {
            attributeRepository.clear();
            delete mscriptAcc;
            delete mengAcc;
            delete marshAcc;

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

            Objects::const_iterator it = mobjects.begin();
            while ( it != mobjects.end() ) {
                delete *it;
                ++it;
            }
        }

    void TaskContext::exportPorts()
    {
        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {
            if ( this->getObject( (*it)->getName() ) == 0 ) {
                // Add the port to the method interface.
                OperationInterface* ms =  (*it)->createPortObject();
                if ( ms )
                    this->addObject( ms );
            }
        }
    }

    bool TaskContext::connectPorts( TaskContext* peer )
    {
        bool failure = false;
        const std::string& location = this->getName();
        Logger::In in( location.c_str()  );
        // connect our write ports to the read ports of 'peer'.
        // and vice versa.
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

            // Detect already connected ports.
            if ( peerport->connected() && (*it)->connected() )
                continue;

            // NOTE: all code below can be replaced by a single line:
            // peerport->connectTo( *it ) || (*it)->connectTo(peerport);
            // but that has less informational log messages.

            // Our port is connected thus peerport is not connected.
            if ( (*it)->connected() ) {
                // ask peer to connect to us:
                if ( peerport->connectTo( *it ) ) {
                    Logger::log() <<Logger::Info<< "Connected Port " << (*it)->getName()
                                  << " of peer Task "<<peer->getName() << " to existing connection." << Logger::endl;
                }
                else {
                    Logger::log() <<Logger::Error<< "Failed to connect Port " << (*it)->getName()
                                  << " of peer Task "<<peer->getName() << " to existing connection." << Logger::endl;
                    failure = true;
                }
                continue;
            }

            // Peer port is connected thus our port is not connected.
            if ( peerport->connected() ) {
                if ( (*it)->connectTo( peerport ) ) {
                    Logger::log() <<Logger::Info<< "Added Port " << (*it)->getName()
                                  << " to existing connection of peer Task "<<peer->getName() << "." << Logger::endl;
                }
                else {
                    Logger::log() <<Logger::Error<< "Not connecting Port " << (*it)->getName()
                                  << " to existing connection of peer Task "<<peer->getName() << "." << Logger::endl;
                    failure = true;
                }
                continue;
            }

            // Last resort: both not connected: create new connection.
            if ( !(*it)->connectTo( peerport ) ) {
                // real error msg will be produced by factory itself.
                log(Warning)<< "Failed to connect Port " << (*it)->getName() << " of " << getName() << " to peer Task "<<peer->getName() <<"." << Logger::endl;
                failure = true;
            } else {
                // all went fine, add peerport as well, will always succeed:
                Logger::log() <<Logger::Info<< "Connected Port " << (*it)->getName() << " to peer Task "<<peer->getName() <<"." << Logger::endl;
            }
        }
        return !failure;
    }

    const std::string& TaskContext::getName() const
    {
        return TaskCore::getName();
    }

    const std::string& TaskContext::getDescription() const
    {
        return mdescription;
    }

    void TaskContext::setDescription(const std::string&  d)
    {
        mdescription = d;
    }

    bool TaskContext::doUpdate()
    {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->execute();
    }

    bool TaskContext::doTrigger()
    {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->trigger();
    }

    bool TaskContext::start() {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->start();
    }
        
    bool TaskContext::stop() {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->stop();
    }
  
    bool TaskContext::isRunning() const {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->isRunning();
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
                 || peer->hasPeer( mtask_name ) )
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
            this->removeObject( (*it)->getName() );
        }
        // reconnect again to our peers and ask our 'users' to reconnect as well.
        for( PeerMap::iterator it = _task_map.begin(); it != _task_map.end(); ++it)
            this->connectPorts(it->second);
        this->exportPorts();

        for( Users::iterator it = musers.begin(); it != musers.end(); ++it) {
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
            this->removeObject( (*it)->getName() );
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
                           select1st<PeerMap::value_type>() );
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

    bool TaskContext::addObject( OperationInterface *obj ) {
        if ( getObject( obj->getName() ) )
            return false;
        mobjects.push_back(obj);
        return true;
    }

    OperationInterface* TaskContext::getObject(const std::string& obj_name )
    {
        if (obj_name == "this")
            return this;
        Objects::const_iterator it = mobjects.begin();
        while ( it != mobjects.end() )
            if ( (*it)->getName() == obj_name )
                return *it;
            else
                ++it;
        return 0;
    }

    std::vector<std::string> TaskContext::getObjectList() const
    {
        std::vector<std::string> res;
        res.push_back("this");
        std::transform(mobjects.begin(), mobjects.end(),
                       std::back_inserter( res ), boost::bind(&OperationInterface::getName,_1));
        return res;
    }

    void TaskContext::clear()
    {
        this->attributes()->clear();
        this->properties()->clear();
        this->methods()->clear();
        this->commands()->clear();
        this->events()->clear();
        this->ports()->clear();

        Objects::const_iterator it = mobjects.begin();
        while ( it != mobjects.end() ) {
            delete *it;
            ++it;
        }
        mobjects.clear();
    }

    bool TaskContext::removeObject(const std::string& obj_name ) {
        if (obj_name == "this") {
            this->methods()->clear();
            this->commands()->clear();
            return true;
        }
        OperationInterface* tgt = getObject(obj_name);
        if (tgt) {
            mobjects.erase( find(mobjects.begin(), mobjects.end(), tgt) );
            delete tgt;
            return true;
        }
        return false;
    }

    bool connectPorts(TaskContext* A, TaskContext* B) { 
        return A->connectPorts(B);
    }

    bool connectPeers(TaskContext* A, TaskContext* B) {
        return A->connectPeers(B);
    }
    
}

