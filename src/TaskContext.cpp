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
#include "internal/CommandInterface.hpp"

#include <string>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "internal/DataSource.hpp"
#include "Method.hpp"
#include "base/ChannelInterface.hpp"

#include "rtt-config.h"
#if !defined(ORO_EMBEDDED) && defined(OROPKG_EXECUTION_PROGRAM_PARSER)
#include "scripting/ParserScriptingAccess.hpp"
#include "scripting/ParserExecutionAccess.hpp"
#endif
#include "interface/MarshallingAccess.hpp"

#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
#include "extras/SequentialActivity.hpp"
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
#include "Activity.hpp"
#endif

namespace RTT
{

    using namespace boost;
    using namespace std;

    TaskContext::TaskContext(const std::string& name, TaskState initial_state /*= Stopped*/)
        :  TaskCore(name, initial_state)
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
           ,dataPorts(this)
#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
           ,our_act( new SequentialActivity( this->engine() ) )
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
           ,our_act( new Activity( this->engine() ) )
#endif
    {
        this->setup();
    }

    TaskContext::TaskContext(const std::string& name, ExecutionEngine* parent, TaskState initial_state /*= Stopped*/ )
        :  TaskCore(name, parent, initial_state)
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
           ,dataPorts(this)
#if defined(ORO_ACT_DEFAULT_SEQUENTIAL)
           ,our_act( parent ? 0 : new SequentialActivity( this->engine() ) )
#elif defined(ORO_ACT_DEFAULT_ACTIVITY)
           ,our_act( parent ? 0 : new Activity( this->engine() ) )
#endif
    {
        this->setup();
    }

    void TaskContext::setup()
    {
        // Work around for bug
        this->mevents.setEventProcessor( ee->events() );

        mdescription = "The interface of this TaskContext.";

        this->methods()
            ->addMethod( method("configure",&TaskContext::configure, this),
                         "Configure this TaskContext (read properties etc)." );
        this->methods()
            ->addMethod( method("isConfigured",&TaskContext::isConfigured, this),
                         "Is this TaskContext configured ?" );
        this->methods()
            ->addMethod( method("start",&TaskContext::start, this),
                         "Start the Execution Engine of this TaskContext (= activate + updateHook() )." );
        this->methods()
            ->addMethod( method("activate",&TaskContext::activate, this),
                         "Activate the Execution Engine of this TaskContext (= events and commands)." );
        this->methods()
            ->addMethod( method("stop",&TaskContext::stop, this),
                         "Stop the Execution Engine of this TaskContext." );
        this->methods()
            ->addMethod( method("isRunning",&TaskContext::isRunning, this),
                         "Is the Execution Engine executing this TaskContext ?" );
        this->methods()
            ->addMethod( method("getPeriod",&TaskContext::getPeriod, this),
                         "Get the configured execution period. -1.0: no thread associated, 0.0: non periodic, > 0.0: the period." );
        this->methods()
            ->addMethod( method("isActive",&TaskContext::isActive, this),
                         "Is the Execution Engine of this TaskContext processing events and commands ?" );
        this->methods()
            ->addMethod( method("inFatalError",&TaskContext::inFatalError, this),
                         "Check if this TaskContext is in the FatalError state." );
        this->methods()
            ->addMethod( method("warning",&TaskContext::warning, this),
                         "Enter the RunTimeWarning state." );
        this->methods()
            ->addMethod( method("inRunTimeWarning",&TaskContext::inRunTimeWarning, this),
                         "Check if this TaskContext is in the RunTimeWarning state." );
        this->methods()
            ->addMethod( method("getWarningCount",&TaskContext::getWarningCount, this),
                         "Check if the number of times the RunTimeWarning state has been entered." );
        this->methods()
            ->addMethod( method("error",&TaskContext::error, this),
                         "Enter the RunTimeError state." );
        this->methods()
            ->addMethod( method("inRunTimeError",&TaskContext::inRunTimeError, this),
                         "Check if this TaskContext is in the RunTimeError state." );
        this->methods()
            ->addMethod( method("getErrorCount",&TaskContext::getErrorCount, this),
                         "Check if the number of times the RunTimeError state has been entered." );
        this->methods()
            ->addMethod( method("cleanup",&TaskContext::cleanup, this),
                         "Reset this TaskContext to the PreOperational state (write properties etc)." );
        this->methods()
            ->addMethod( method("resetError",&TaskContext::resetError, this),
                         "Reset this TaskContext from the FatalError state." );
        this->methods()
            ->addMethod( method("update",&TaskContext::doUpdate, this),
                         "Execute (call) the update method directly.\n Only succeeds if the task isRunning() and allowed by the Activity executing this task." );

        this->methods()
            ->addMethod( method("trigger",&TaskContext::doTrigger, this),
                         "Trigger the update method for execution in the thread of this task.\n Only succeeds if the task isRunning() and allowed by the Activity executing this task." );

        // See TaskContext::start().
        updated_ports.reserve(0);

    }

        TaskContext::~TaskContext()
        {
            // We don't call stop() or cleanup() here since this is
            // the responsibility of the subclass. Calling these functions
            // here would only lead to calling invalid virtual functions.
            // [Rule no 1: Don't call virtual functions in a destructor.]
            // [Rule no 2: Don't call virtual functions in a constructor.]
            mattributes.clear();

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

        }

    void TaskContext::exportPorts()
    {
        // This function is only used by the IOComponent, probably in the wrong way,
        // since DataFlowInterface::addPort does the port object creation already.
        DataFlowInterface::Ports myports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = myports.begin();
             it != myports.end();
             ++it) {
            // Add the port to the method interface.
            if (this->getObject((*it)->getName()) == 0) {
                OperationInterface* ms = this->ports()->createPortObject((*it)->getName());
                if ( ms )
                    if ( this->addObject( ms ) == false )
                        delete ms;
            }
        }
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
            OutputPortInterface* this_w = dynamic_cast<OutputPortInterface*>(*it);
            InputPortInterface* other_r = dynamic_cast<InputPortInterface*>(peerport);
            if (this_w && other_r)
                failure = failure || !this_w->createConnection(*other_r);
            else
            {
                OutputPortInterface* other_w = dynamic_cast<OutputPortInterface*>(peerport);
                InputPortInterface* this_r = dynamic_cast<InputPortInterface*>(*it);
                if (other_w && this_r)
                    failure = failure || !other_w->createConnection(*this_r);
                else
                {
                    log(Debug)<< "Data flow direction incompatible between ports "
                        << getName() << "::" << (*it)->getName() << " and "
                        << peer->getName() << "::" << (*it)->getName() << endlog();
                }
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
                 || peer->hasPeer( mtask_name ) )
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

    bool TaskContext::addObject( OperationInterface *obj ) {
        if (OperationInterface::addObject(obj) == false)
            return false;
        obj->setEngine( this->engine() );
        return true;
    }

    void TaskContext::setActivity(RTT::ActivityInterface* new_act)
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
        new_act->run( this->engine() );
        our_act = ActivityInterface::shared_ptr( new_act );
    }

    ActivityInterface* TaskContext::getActivity()
    {
        if (this->engine()->getActivity() != our_act.get() )
            return this->engine()->getActivity();
        return our_act.get();
    }

    void TaskContext::clear()
    {
        this->removeObject("this");
        this->properties()->clear();
        this->ports()->clear();

        OperationInterface::clear();
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
                        Handle h = read_port->getNewDataOnPortEvent()->connect(boost::bind(&TaskContext::dataOnPort, this, _1), this->engine()->events());
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
        // Since this handler is executed in our thread, we are always running.
        if (find(updated_ports.begin(), updated_ports.end(), port) == updated_ports.end() )
            updated_ports.push_back(port);
        // this is in essence superfluous. We are already triggered.
        //this->getActivity()->trigger();
    }

    void TaskContext::updateHook()
    {
        updateHook(updated_ports);
        updated_ports.clear();
    }

    void TaskContext::updateHook(std::vector<PortInterface*> const& updated_ports)
    {
    }
}

