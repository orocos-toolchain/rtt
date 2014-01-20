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
#include "plugin/PluginLoader.hpp"

#include <string>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "internal/DataSource.hpp"
#include "internal/mystd.hpp"
#include "internal/MWSRQueue.hpp"
#include "OperationCaller.hpp"

#include "rtt-config.h"

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
        :  TaskCore( initial_state)
           ,portqueue( new MWSRQueue<PortInterface*>(64) )
           ,tcservice(new Service(name,this) ), tcrequests( new ServiceRequester(name,this) )
#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
           ,our_act( new SequentialActivity( this->engine() ) )
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
           ,our_act( new Activity( this->engine(), name ) )
#endif
    {
        TaskCore::setName(name);
        this->setup();
    }

    TaskContext::TaskContext(const std::string& name, ExecutionEngine* parent, TaskState initial_state /*= Stopped*/ )
        :  TaskCore(parent, initial_state)
           ,portqueue( new MWSRQueue<PortInterface*>(64) )
           ,tcservice(new Service(name,this) ), tcrequests( new ServiceRequester(name,this) )
#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
           ,our_act( parent ? 0 : new SequentialActivity( this->engine() ) )
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
           ,our_act( parent ? 0 : new Activity( this->engine(), name ) )
#endif
    {
        TaskCore::setName(name);
        this->setup();
    }

    void TaskContext::setup()
    {
        tcservice->setOwner(this);
        // from Service
        provides()->doc("The interface of this TaskContext.");

        this->addOperation("configure", &TaskContext::configure, this, ClientThread).doc("Configure this TaskContext (= configureHook() ).");
        this->addOperation("isConfigured", &TaskContext::isConfigured, this, ClientThread).doc("Is this TaskContext configured ?");
        this->addOperation("start", &TaskContext::start, this, ClientThread).doc("Start this TaskContext (= startHook() + updateHook() ).");
        this->addOperation("activate", &TaskContext::activate, this, ClientThread).doc("Activate the Execution Engine of this TaskContext.");
        this->addOperation("stop", &TaskContext::stop, this, ClientThread).doc("Stop this TaskContext (= stopHook() ).");
        this->addOperation("isRunning", &TaskContext::isRunning, this, ClientThread).doc("Is this TaskContext started ?");
        this->addOperation("getPeriod", &TaskContext::getPeriod, this, ClientThread).doc("Get the configured execution period. -1.0: no thread associated, 0.0: non periodic, > 0.0: the period.");
        this->addOperation("setPeriod", &TaskContext::setPeriod, this, ClientThread).doc("Set the execution period in seconds.").arg("s", "Period in seconds.");
        this->addOperation("getCpuAffinity", &TaskContext::getCpuAffinity, this, ClientThread).doc("Get the configured cpu affinity.");
        this->addOperation("setCpuAffinity", &TaskContext::setCpuAffinity, this, ClientThread).doc("Set the cpu affinity.").arg("cpu", "Cpu mask.");
        this->addOperation("isActive", &TaskContext::isActive, this, ClientThread).doc("Is the Execution Engine of this TaskContext active ?");
        this->addOperation("inFatalError", &TaskContext::inFatalError, this, ClientThread).doc("Check if this TaskContext is in the FatalError state.");
        this->addOperation("error", &TaskContext::error, this, ClientThread).doc("Enter the RunTimeError state (= errorHook() ).");
        this->addOperation("inRunTimeError", &TaskContext::inRunTimeError, this, ClientThread).doc("Check if this TaskContext is in the RunTimeError state.");
        this->addOperation("cleanup", &TaskContext::cleanup, this, ClientThread).doc("Reset this TaskContext to the PreOperational state ( =cleanupHook() ).");
        this->addOperation("update", &TaskContext::update, this, ClientThread).doc("Execute (call) the update method directly.\n Only succeeds if the task isRunning() and allowed by the Activity executing this task.");

        this->addOperation("trigger", &TaskContext::trigger, this, ClientThread).doc("Trigger the update method for execution in the thread of this task.\n Only succeeds if the task isRunning() and allowed by the Activity executing this task.");
        this->addOperation("loadService", &TaskContext::loadService, this, ClientThread).doc("Loads a service known to RTT into this component.").arg("service_name","The name with which the service is registered by in the PluginLoader.");
        // activity runs from the start.
        if (our_act)
            our_act->start();
    }

        TaskContext::~TaskContext()
        {
            if (our_act)
                our_act->stop();
            // We don't call stop() or cleanup() here since this is
            // the responsibility of the subclass. Calling these functions
            // here would only lead to calling invalid virtual functions.
            // [Rule no 1: Don't call virtual functions in a destructor.]
            // [Rule no 2: Don't call virtual functions in a constructor.]
            tcservice->clear();

            tcrequests->clear();

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
            delete portqueue;
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

            // Skip if they have the same type
            if((dynamic_cast<OutputPortInterface*>(*it) && dynamic_cast<OutputPortInterface*>(peerport)) ||
               (dynamic_cast<InputPortInterface*>(*it) &&  dynamic_cast<InputPortInterface*>(peerport)))
              {
                log(Debug)<< (*it)->getName() << " and " << peerport->getName() << " have the same type" << endlog();
                continue;
              }

            // Try to find a way to connect them
            if ( !(*it)->connectTo( peerport ) ) {
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
        bool success = true;
        const std::string& location = this->getName();
        Logger::In in( location.c_str()  );

        vector<string> myreqs = this->requires()->getRequesterNames();
        vector<string> peerreqs = peer->requires()->getRequesterNames();

        this->requires()->connectTo( peer->provides() );
        for (vector<string>::iterator it = myreqs.begin();
             it != myreqs.end();
             ++it) {
            ServiceRequester::shared_ptr sr = this->requires(*it);
            if ( !sr->ready() ) {
                if (peer->provides()->hasService( *it ))
                    success = sr->connectTo( peer->provides(*it) ) && success;
                else {
                    log(Debug)<< "Peer Task "<<peer->getName() <<" provides no Service " << *it << endlog();
                }
            }
        }

        peer->requires()->connectTo( this->provides() );
        for (vector<string>::iterator it = peerreqs.begin();
                it != peerreqs.end();
                ++it) {
            ServiceRequester::shared_ptr sr = peer->requires(*it);
            if ( !sr->ready() ) {
                if (this->provides()->hasService(*it))
                    success = sr->connectTo( this->provides(*it) ) && success;
                else
                    log(Debug)<< "This Task provides no Service " << *it << " for peer Task "<<peer->getName() <<"."<< endlog();
            }
        }
        return success;
    }

    bool TaskContext::prepareProvide(const std::string& name) {
         return tcservice->hasService(name) || plugin::PluginLoader::Instance()->loadService(name, this);
    }

    bool TaskContext::loadService(const std::string& service_name) {
        if ( provides()->hasService(service_name))
            return true;
        return PluginLoader::Instance()->loadService(service_name, this);
    }

    void TaskContext::addUser( TaskContext* peer )
    {
        if (peer)
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
            if ( !peer || _task_map.count( alias ) != 0 )
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
                 || peer->hasPeer( this->getName() ) )
                return false;
            this->addPeer ( peer );
            peer->addPeer ( this );
            return true;
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

    bool TaskContext::setActivity(ActivityInterface* new_act)
    {
        if (this->isRunning())
            return false;
        if ( new_act == 0) {
#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
            new_act = new SequentialActivity();
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
            new_act = new Activity();
#endif
        }
        new_act->stop();
        if(our_act){
            our_act->stop();
        }
        new_act->run( this->engine() );
        our_act = ActivityInterface::shared_ptr( new_act );
        our_act->start();
        return true;
    }

    void TaskContext::forceActivity(ActivityInterface* new_act)
    {
    	if (!new_act)
    		return;
    	new_act->stop();
        if(our_act){
            our_act->stop();
        }
        our_act.reset( new_act );
        our_act->run( this->engine() );
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
        tcservice->clear();
        tcrequests->clear();
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
        if ( this->isRunning() )
            return false;
#ifdef ORO_SIGNALLING_PORTS
        ports()->setupHandles();
#endif
        return TaskCore::start(); // calls startHook()
    }

    bool TaskContext::stop()
    {
        if ( !this->isRunning() )
            return false;
        if (TaskCore::stop()) { // calls stopHook()
#ifdef ORO_SIGNALLING_PORTS
            ports()->cleanupHandles();
#endif
            return true;
        }
        return false;
    }

    void TaskContext::dataOnPort(PortInterface* port)
    {
        if ( this->dataOnPortHook(port) ) {
            portqueue->enqueue( port );
            this->getActivity()->trigger();
        }
    }

    bool TaskContext::dataOnPortHook( base::PortInterface* ) {
        return this->isRunning();
    }

    void TaskContext::dataOnPortCallback(InputPortInterface* port, TaskContext::SlotFunction callback) {
        // user_callbacks will only be emitted from updateHook().
        MutexLock lock(mportlock);
        user_callbacks[port] = callback;
    }

    void TaskContext::dataOnPortRemoved(PortInterface* port) {
        MutexLock lock(mportlock);
        UserCallbacks::iterator it = user_callbacks.find(port);
        if (it != user_callbacks.end() ) {
            user_callbacks.erase(it);
        }
    }

    void TaskContext::prepareUpdateHook()
    {
        MutexLock lock(mportlock);
        PortInterface* port = 0;
        while ( portqueue->dequeue( port ) == true ) {
            UserCallbacks::iterator it = user_callbacks.find(port);
            if (it != user_callbacks.end() )
                it->second(port); // fire the user callback
        }
    }
}

