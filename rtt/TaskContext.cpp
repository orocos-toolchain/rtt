/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TaskContext.cxx

                        TaskContext.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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



#include "TaskContext.hpp"
#include "base/ActionInterface.hpp"

#include <string>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "internal/DataSource.hpp"
#include "internal/mystd.hpp"
#include "Method.hpp"

#include "rtt-config.h"
#include "marsh/MarshallingAccess.hpp"
#include "scripting/ScriptingAccess.hpp"

#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
#include "extras/SequentialActivity.hpp"
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
#include "Activity.hpp"
#endif

namespace RTT
{

    using namespace boost;
    using namespace std;
    using namespace detail;

    TaskContext::TaskContext(const std::string& name, TaskState initial_state /*= Stopped*/)
        :  TaskCore( initial_state), ServiceProvider(name, this), ServiceRequester(name, this)
           ,portqueue(64)
           ,dataPorts(this)
#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
           ,our_act( new SequentialActivity( this->engine() ) )
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
           ,our_act( new Activity( this->engine(), name ) )
#endif
    {
        this->setup();
    }

    TaskContext::TaskContext(const std::string& name, ExecutionEngine* parent, TaskState initial_state /*= Stopped*/ )
        :  TaskCore(parent, initial_state), ServiceProvider(name, this), ServiceRequester(name, this)
           ,portqueue(64)
           ,dataPorts(this)
#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
           ,our_act( parent ? 0 : new SequentialActivity( this->engine() ) )
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
           ,our_act( parent ? 0 : new Activity( this->engine(), name ) )
#endif
    {
        this->setup();
    }

    void TaskContext::setup()
    {
        // Temporarily until plugins are implemented:
        new ScriptingAccess(this);
        new MarshallingAccess(this);

        // from ServiceProvider
        mdescription = "The interface of this TaskContext.";

        this->addOperation("configure", &TaskContext::configure, this, ClientThread).doc("Configure this TaskContext (read properties etc).");
        this->addOperation("isConfigured", &TaskContext::isConfigured, this, ClientThread).doc("Is this TaskContext configured ?");
        this->addOperation("start", &TaskContext::start, this, ClientThread).doc("Start the Execution Engine of this TaskContext (= activate + updateHook() ).");
        this->addOperation("activate", &TaskContext::activate, this, ClientThread).doc("Activate the Execution Engine of this TaskContext (= events and commands).");
        this->addOperation("stop", &TaskContext::stop, this, ClientThread).doc("Stop the Execution Engine of this TaskContext.");
        this->addOperation("isRunning", &TaskContext::isRunning, this, ClientThread).doc("Is the Execution Engine executing this TaskContext ?");
        this->addOperation("getPeriod", &TaskContext::getPeriod, this, ClientThread).doc("Get the configured execution period. -1.0: no thread associated, 0.0: non periodic, > 0.0: the period.");
        this->addOperation("isActive", &TaskContext::isActive, this, ClientThread).doc("Is the Execution Engine of this TaskContext processing events and commands ?");
        this->addOperation("inFatalError", &TaskContext::inFatalError, this, ClientThread).doc("Check if this TaskContext is in the FatalError state.");
        this->addOperation("error", &TaskContext::error, this, ClientThread).doc("Enter the RunTimeError state.");
        this->addOperation("inRunTimeError", &TaskContext::inRunTimeError, this, ClientThread).doc("Check if this TaskContext is in the RunTimeError state.");
        this->addOperation("cleanup", &TaskContext::cleanup, this, ClientThread).doc("Reset this TaskContext to the PreOperational state (write properties etc).");
        this->addOperation("resetError", &TaskContext::resetError, this, ClientThread).doc("Reset this TaskContext from the FatalError state.");
        this->addOperation("update", &TaskContext::update, this, ClientThread).doc("Execute (call) the update method directly.\n Only succeeds if the task isRunning() and allowed by the Activity executing this task.");

        this->addOperation("trigger", &TaskContext::trigger, this, ClientThread).doc("Trigger the update method for execution in the thread of this task.\n Only succeeds if the task isRunning() and allowed by the Activity executing this task.");

        // See TaskContext::start().
        updated_ports.reserve(0);

        // activity runs from the start.
        if (our_act)
            our_act->start();
    }

        TaskContext::~TaskContext()
        {
            // We don't call stop() or cleanup() here since this is
            // the responsibility of the subclass. Calling these functions
            // here would only lead to calling invalid virtual functions.
            // [Rule no 1: Don't call virtual functions in a destructor.]
            // [Rule no 2: Don't call virtual functions in a constructor.]
            AttributeRepository::clear();

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

    bool TaskContext::connectPorts( TaskContext* peer )
    {
        bool failure = false;
        const std::string& location = this->getName();
        Logger::In in( location.c_str()  );

        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {

            // Then try to get the peer port's connection
            PortInterface* peerport = peer->ports()->getPort( (*it)->getName() );
            if ( !peerport ) {
                log(Debug)<< "Peer Task "<<peer->getName() <<" has no Port " << (*it)->getName() << endlog();
                continue;
            }

            // Try to find a way to connect them
            if ( !(*it)->connectTo(*peerport) ) {
                log(Debug)<< "Data flow incompatible between ports "
                          << getName() << "." << (*it)->getName() << " and "
                          << peer->getName() << "." << (*it)->getName() << endlog();
                failure = true;
            }
        }
        return !failure;
    }

    bool TaskContext::connectServices( TaskContext* peer )
    {
        bool failure = false;
        const std::string& location = this->getName();
        Logger::In in( location.c_str()  );

        vector<string> myreqs = this->getRequestNames();
        vector<string> peerreqs = peer->getRequestNames();

        for (vector<string>::iterator it = myreqs.begin();
             it != myreqs.end();
             ++it) {
            ServiceRequester* sr = this->requires(*it);
            if ( !sr->ready() ) {
                if (peer->hasService( *it ))
                    sr->connectTo( peer->provides(*it) );
                else {
                    log(Debug)<< "Peer Task "<<peer->getName() <<" provides no Service " << *it << endlog();
                }
            }
        }
        for (vector<string>::iterator it = peerreqs.begin();
                it != peerreqs.end();
                ++it) {
            ServiceRequester* sr = peer->requires(*it);
            if ( !sr->ready() ) {
                if (this->hasService(*it))
                    sr->connectTo( this->provides(*it) );
                else
                    log(Debug)<< "This Task provides no Service " << *it << " for peer Task "<<peer->getName() <<"."<< endlog();
            }
        }
        return !failure;
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
                 || peer->hasPeer( mname ) )
                return false;
            this->addPeer ( peer );
            peer->addPeer ( this );
            return true;
        }

    void TaskContext::reconnect()
    {
        Logger::In in( this->getName().c_str()  );
        log(Info) << "Starting reconnection..."<<endlog();
        // first disconnect all our ports
        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {
            (*it)->disconnect();
        }
        // reconnect again to our peers and ask our 'users' to reconnect as well.
        for( PeerMap::iterator it = _task_map.begin(); it != _task_map.end(); ++it)
            this->connectPorts(it->second);

        log(Info) << "Reconnection done."<<endlog();
    }

    void TaskContext::disconnect() {
        Logger::In in( this->getName().c_str()  );
        // disconnect all our ports
        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {
            (*it)->disconnect();
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

    void TaskContext::setActivity(ActivityInterface* new_act)
    {
        if (this->isActive())
            return;
        if ( new_act == 0) {
#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
            new_act = new SequentialActivity();
#elseif defined(ORO_ACT_DEFAULT_ACTIVITY)
            new_act = new Activity();
#endif
        }
        our_act->stop();
        new_act->run( this->engine() );
        our_act = ActivityInterface::shared_ptr( new_act );
        our_act->start();
    }

    ActivityInterface* TaskContext::getActivity()
    {
        if (this->engine()->getActivity() != our_act.get() )
            return this->engine()->getActivity();
        return our_act.get();
    }

    void TaskContext::clear()
    {
        ServiceProvider::clear();
        this->ports()->clear();
    }

    bool TaskContext::ready()
    {
        return true;
    }

    bool connectPorts(TaskContext* A, TaskContext* B) {
        return A->connectPorts(B);
    }

    bool connectPeers(TaskContext* A, TaskContext* B) {
        return A->connectPeers(B);
    }

    bool TaskContext::start()
    {
        size_t port_count = 0;
        if ( this->isRunning() )
            return false;
        // Workaround for multiple registrations in 1.8.x:
        if ( updated_ports.capacity() != 0) {
            // do not register event handles a second time.
            // This belongs more during the configure() step than during the start() step.
            // should addEventPort cause the TC to drop back to Unconfigured or should
            // the code below be executed during addEventPort ?
            return TaskCore::start();
        }
        const DataFlowInterface::Ports& ports = this->ports()->getEventPorts();
        for (DataFlowInterface::Ports::const_iterator it = ports.begin(); it != ports.end(); ++it)
            {
                InputPortInterface* read_port = dynamic_cast<InputPortInterface*>(*it);
                if (read_port)
                    {
                        Handle h = read_port->getNewDataOnPortEvent()->connect(boost::bind(&TaskContext::dataOnPort, this, _1) );
                        if (h) {
                            port_count++;
                            log(Info) << getName() << " will be triggered when new data is available on " << (*it)->getName() << endlog();
                        } else {
                            log(Error) << getName() << " can't connect to event of " << (*it)->getName() << endlog();
                            return false;
                        }
                    }
            }
        updated_ports.reserve(port_count);
        return TaskCore::start(); // calls startHook()
    }

    void TaskContext::dataOnPort(PortInterface* port)
    {
        portqueue.enqueue( port );
        this->getActivity()->trigger();
    }

    void TaskContext::updateHook()
    {
        // When the user overrides this method, this code will never be called, so the portqueue will fill up
        // and remain as such.
        PortInterface* port = 0;
        while ( portqueue.dequeue( port ) == true ) {
            if (find(updated_ports.begin(), updated_ports.end(), port) == updated_ports.end() )
                updated_ports.push_back(port);
        }
        updateHook(updated_ports);
        updated_ports.clear();
    }

    void TaskContext::updateHook(std::vector<PortInterface*> const& updated_ports)
    {
    }

    bool TaskContext::isPortUpdated(base::PortInterface const& port) const
    {
        return find(updated_ports.begin(), updated_ports.end(), &port) != updated_ports.end();
    }
}

