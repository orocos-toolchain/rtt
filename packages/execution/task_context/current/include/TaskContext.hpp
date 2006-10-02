/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TaskContext.hpp 

                        TaskContext.hpp -  description
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
 
 
#ifndef ORO_TASK_CONTEXT_HPP
#define ORO_TASK_CONTEXT_HPP

#include "AttributeRepository.hpp"

#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB_EVENTS
#include "EventService.hpp"
#endif

#include "DataFlowInterface.hpp"
#include "ExecutionEngine.hpp"
#include "ScriptingAccess.hpp"
#include "ExecutionAccess.hpp"
#include "MarshallingAccess.hpp"
#include "TaskCore.hpp"
#include "PropertyBag.hpp"

#include <string>
#include <map>

namespace RTT
{
    class CommandProcessor;
    class ScriptingAccess;
    class TaskObject;
    class EventService;
    class ExecutionAccess;
    class MarshallingAccess;

    /**
     * A TaskContext exports the commands, methods, events, properties and ports
     * a task has. Furthermore, it allows to visit its peer tasks.
     *
     * @par TaskContext interface
     * When a command is exported, one can access it using commands(). A similar
     * mechanism is available for properties(), methods(), events() and ports().
     * The commands of this TaskContext are executed by its
     * ExecutionEngine.
     *
     * @par Executing a TaskContext
     * In order to run the ExecutionEngine, the ExecutionEngine must
     * be invoked from an ActivityInterface implementation. As long as
     * there is no activity or the activity is not started, this
     * TaskContext will not accept any commands, nor process events,
     * nor execute programs or state machines.  In this way, the user
     * of this class can determine himself at which point and at which
     * moment commands and programs can be executed.
     *
     * @par Connecting TaskContexts
     * TaskContexts are connected using the unidirectional addPeer() or bidirectional
     * connectPeers() methods. These methods only allow
     * 'peer' TaskContexts to use each other's interface. Use connectPorts()
     * to setup the data connections between data ports.
     * In order to disconnect this task from its peers, use disconnect(), which
     * will disconnect all the Data Flow Ports and remove this object from its
     * Peers.
     */
    class TaskContext
        : public OperationInterface,
          public TaskCore
    {
        // non copyable
        TaskContext( TaskContext& );
    protected:
        std::string mdescription;
    
        typedef std::map< std::string, TaskContext* > PeerMap;
        typedef std::vector< TaskContext* > Users;
        typedef std::vector< OperationInterface* > Objects;
        /// map of the tasks we are using
        PeerMap         _task_map;
        /// map of the tasks that are using us.
        Users         musers;
        /// the TaskObjects.
        Objects mobjects;

        ScriptingAccess* mscriptAcc;

        ExecutionAccess* mengAcc;

        MarshallingAccess* marshAcc;
        /**
         * Inform this TaskContext that \a user is using
         * our services.
         */
        void addUser(TaskContext* user);

        /**
         * Inform this TaskContext that \a user is no longer using
         * our services.
         */
        void removeUser(TaskContext* user);

        void setup();
    public:
        typedef std::vector< std::string > PeerList;
        typedef std::vector< std::string > ObjectList;

        /**
         * Create a TaskContext visible with \a name.
         * It's ExecutionEngine will be newly constructed with private 
         * ExecutionEngine processing its commands, events,
         * programs and state machines.
         */
        TaskContext( const std::string& name );

        /**
         * Create a TaskContext visible with \a name. Its commands
         * programs and state machines are processed by \a parent.
         * Use this constructor to share execution engines among task contexts, such that
         * the execution of their functionality is serialised (executed in the same thread).
         */
        TaskContext(const std::string& name, ExecutionEngine* parent );

        virtual ~TaskContext();

        virtual const std::string& getName() const;

        virtual const std::string& getDescription() const;

        virtual void setDescription(const std::string& descr);

        /**
         * Call this function to force a TaskContext to export its
         * Data Flow ports as scripting objects. This is done by the
         * component itself when a peer connection is made, but in
         * case you want to access the Data Flow ports of a
         * stand-alone component from the scripting interface,
         * you need to call this function.
         */
        void exportPorts();

        /**
         * @name Script Methods
         * The standard script methods of a TaskContext are for starting 
         * and stopping its ExecutionEngine. The methods map to the
         * ActivityInterface which executes the ExecutionEngine.
         * @{
         */
        /**
         * Start is a method which starts the execution engine's task.
         * It can not be a command because if the engine is not running,
         * it does not accept commands. Also, RunnableInterface::initialize()
         * is then called in the context of the caller.
         * You can override this method to do something else or in addition
         * to starting the Processor.
         * @return false if the engine was not assigned to a ActivityInterface
         * or if initialize() returned false or it was already started.
         */
        virtual bool start();
        
        /**
         * Stop is a method which stops the execution engine's task.
         * RunnableInterface::finalize()
         * is called in the context of the caller.
         * You can override this method to do something else or in addition
         * to stopping the engine.
         * @return false if the engine was not assigned to a ActivityInterface
         * or if it was not running.
         */
        virtual bool stop();
  
        /**
         * DataSource to inspect if this Task is running.
         * Defaults to this->getProcessor()->getActivity()
         * && this->getProcessor()->getActivity()->isRunning()
         */
        virtual bool isRunning() const;

        /**
         * Invoke this method to \a execute
         * the ExecutionEngine and the update() method. This method maps to
         * the 'update()' method in the scripting language.
         * @retval false if this->engine()->getActivity()->execute() == false
         * @retval true otherwise.
         */
        virtual bool doUpdate();

        /**
         * Invoke this method to \a trigger the thread of this TaskContext to execute
         * its ExecutionEngine and the update() method. This method maps to
         * the 'trigger()' method in the scripting language.
         * @retval false if this->engine()->getActivity()->trigger() == false
         * @retval true otherwise.
         */
        virtual bool doTrigger();
        /**
         *@}
         */

        /**
         * Add a one-way connection from this task to a peer task.
         * @param peer The peer to add.
         * @param alias An optional alias (another name) for the peer.
         * defaults to \a peer->getName()
         */
        virtual bool addPeer( TaskContext* peer, std::string alias = "" );

        /**
         * Remove a one-way connection from this task to a peer task.
         */
        virtual void removePeer( const std::string& name );

        /**
         * Remove a one-way connection from this task to a peer task.
         */
        virtual void removePeer( TaskContext* peer );

        /**
         * Add a two-way connection from this task to a peer task.
         */
        virtual bool connectPeers( TaskContext* peer );

        /**
         * Add a data flow connection from this task's ports to a peer's ports.
         */
        virtual bool connectPorts( TaskContext* peer );

        /**
         * Disconnect this TaskContext from it's peers.
         * All its Data Flow Ports are disconnected from the connections but
         * the connections themselves may continue to exist to serve other TaskContexts.
         * This method invokes removePeer() as well on the peers listed in this->getPeerList().
         */
        virtual void disconnect();

        /**
         * Reconnect the data ports of this task, without
         * removing the peer relationship. Use this if you
         * changed a port name of an already connected task.
         */
        virtual void reconnect();

        /**
         * Remove a two-way connection from this task to a peer task.
         */
        virtual void disconnectPeers( const std::string& name );

        /**
         * Return a standard container which contains all the Peer names
         * of this TaskContext.
         */
        virtual PeerList getPeerList() const;

        /**
         * Return true if it knows a peer by that name.
         */
        virtual bool hasPeer( const std::string& peer_name ) const;

        /**
         * Get a pointer to a peer of this task.
         * @return null if no such peer.
         */
        virtual TaskContext* getPeer(const std::string& peer_name ) const;

        /** 
         * Add a new TaskObject to this TaskContext.
         * 
         * @param obj This object becomes owned by this TaskContext.
         * 
         * @return true if it cuold be added, false if such
         * object already exists.
         */
        virtual bool addObject( OperationInterface *obj );

        /** 
         * Get a pointer to a previously added TaskObject
         * 
         * @param obj_name The name of the TaskObject
         * 
         * @return the pointer
         */
        virtual OperationInterface* getObject(const std::string& obj_name );

        /** 
         * Get a list of all the object names of this TaskContext.
         * 
         * @return a list of string names.
         */
        virtual ObjectList getObjectList() const;

        /** 
         * Remove and delete a previously added TaskObject.
         * 
         * @param obj_name The name of the TaskObject
         * 
         * @return true if found and removed, false otherwise.
         */
        virtual bool removeObject(const std::string& obj_name );

        /** 
         * Clear the complete interface of this Component.
         * This method removes all objects and all methods, commands,
         * events, properties and ports from the interface of this TaskContext.
         */
        void clear();

        /**
         * Get access to high level controls for
         * programs, state machines and scripting
         * statements.
         */
        ScriptingAccess* scripting()
        {
            return mscriptAcc;
        }

        /**
         * Get access to high level controls for
         * programs, state machines and scripting
         * statements.
         */
        const ScriptingAccess* scripting() const
        {
            return mscriptAcc;
        }

        /**
         * Get access to high level controls for
         * controlling programs and state machines.
         * It is the implementation of the 'engine'
         * TaskObject.
         */
        ExecutionAccess* execution()
        {
            return mengAcc;
        }

        /**
         * Get access to high level controls for
         * controlling programs and state machines.
         * It is the implementation of the 'engine'
         * TaskObject.
         */
        const ExecutionAccess* execution() const
        {
            return mengAcc;
        }

        /**
         * Get access to high level controls for
         * (de)serializing properties to/from an XML format.
         */
        MarshallingAccess* marshalling()
        {
            return marshAcc;
        }

        /**
         * Get access to high level controls for
         * (de)serializing properties to/from an XML format.
         */
        const MarshallingAccess* marshalling() const
        {
            return marshAcc;
        }

        /**
         * The command interface of this task context.
         */
        CommandRepository* commands()
        {
            return &comms;
        }

        /**
         * The command interface of this task context.
         */
        const CommandRepository* commands() const
        {
            return &comms;
        }

        /**
         * The method interface of this task context.
         */
        MethodRepository* methods()
        {
            return &meths;
        }

        /**
         * The method interface of this task context.
         */
        const MethodRepository* methods() const
        {
            return &meths;
        }

        /**
         * The task-local values ( attributes and properties ) of this TaskContext.
         */
        AttributeRepository* attributes() {
            return &attributeRepository;
        }

        /**
         * The task-local values ( attributes and properties ) of this TaskContext.
         */
        const AttributeRepository* attributes() const {
            return &attributeRepository;
        }

        /**
         * The properties of this TaskContext.
         */
        PropertyBag* properties() {
            return attributeRepository.properties();
        }

        /**
         * The properties of this TaskContext.
         */
        const PropertyBag* properties() const {
            return attributeRepository.properties();
        }

        /**
         * The task-local events ( 'signals' ) of this TaskContext.
         */
        EventService* events() {
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
            return &eventService;
#else
            return 0;
#endif
        }

        /**
         * The task-local events ( 'signals' ) of this TaskContext.
         */
        const EventService* events() const {
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
            return &eventService;
#else
            return 0;
#endif
        }

        /**
         * Get the Data flow ports of this task.
         */
        DataFlowInterface* ports() {
            return &dataPorts;
        }
        
        /**
         * Get the Data flow ports of this task.
         */
        const DataFlowInterface* ports() const {
            return &dataPorts;
        }

    private:
        /**
         * The task-local values ( attributes ) of this TaskContext.
         */
        AttributeRepository     attributeRepository;

#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
        /**
         * The task-local events ( 'signals' ) of this TaskContext.
         */
        EventService            eventService;
#endif

        /**
         * The task-local ports.
         */
        DataFlowInterface dataPorts;

        CommandRepository comms;

        MethodRepository meths;
    };

    /**
     * Connect the Data Flow Ports of A and B in both
     * directions, by matching port names.
     * @see TaskContext::connectPorts
     */
    bool connectPorts(TaskContext* A, TaskContext* B);

    /**
     * Set up the Execution Flow (who knows who)
     * between A and B in both directions. Both will be able to 
     * use each other's interface.
     * @see TaskContext::connectPeers
     */
    bool connectPeers(TaskContext* A, TaskContext* B);
}

#endif
