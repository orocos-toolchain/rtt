/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TaskContext.hpp 

                        TaskContext.hpp -  description
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
 
 
#ifndef ORO_TASK_CONTEXT_HPP
#define ORO_TASK_CONTEXT_HPP

#include "AttributeRepository.hpp"

#include "rtt-config.h"
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
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
     * @par TaskContext state behaviour
     * When a TaskContext is created it defaults to the 'Stopped' state or
     * the 'PreOperational' state. If it is 'Stopped', it can be started as 
     * soon as an activity object is attached to it. If it is 'PreOperational',
     * it must first be configured before it can be started.
     * @see TaskState for a detailed explanation.
     *
     * @par Executing a TaskContext
     * In order to run the ExecutionEngine, the ExecutionEngine must
     * be run by an ActivityInterface implementation. As long as
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
    public:
        /**
         * A list of Peer TaskContext names.
         */
        typedef std::vector< std::string > PeerList;
        /**
         * A list of internal TaskObject names.
         */

        /**
         * Describes the different states a component can have.
         * When a TaskContext is being constructed, it is in the
         * \a Init state. After the construction ends, the
         * component arrives in the \a PreOperational (additional
         * configuration required) or the \a Stopped (ready to run)
         * state. Invoking \a start() will make a transition to the
         * \a Running state and \a stop() back to the \a Stopped state.
         *
         * In order to check if these transitions are allowed, hook functions
         * are executed, which can be filled in by the component builder.
         * - A transition from \a PreOperational to \a Stopped is checked
         * by calling the \a configureHook() method. If this method returns \a true,
         * the transition is made, otherwise, the state remains \a PreOperational.
         * - A transition from \a Stopped to \a Running is checked by calling
         * the \a startHook() method. If this method returns \a true,
         * the transition is made, otherwise, the state remains \a Stopped.
         * - A transition from \a Running to \a Stopped is always allowed
         * and the \a stopHook() method is called to inform the component of
         * this transtion.
         * - A transition from \a Stopped to \a PreOperational is always allowed
         * and the \a cleanupHook() method is called to inform the component of
         * this transtion.
         *
         */
        enum TaskState { Init,           //! The state during component construction.
                         PreOperational, //! The state indicating additional configuration is required.
                         Stopped,        //! The state indicating the component is ready to run.
                         Running         //! The state indicating the component is running.
        }; 
        
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

        virtual OperationInterface* getParent() { return this; }

        /**
         * This method is ignored by the TaskContext.
         */
        virtual void setParent(OperationInterface*) { }

        /**
         * This method is ignored by the TaskContext.
         * @see TaskCore::setExecutionEngine for (re-)setting a new
         * ExecutionEngine, which is a base class of TaskContext.
         */
        virtual void setEngine(ExecutionEngine*) { }

        /**
         * Returns the current state of the TaskContext.
         */
        TaskState getTaskState() const;

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
         *
         * The standard script methods of a TaskContext are for
         * configuration and starting and stopping its
         * ExecutionEngine.  @{
         */

        /**
         * This method instructs the component to (re-)read configuration data
         * and try to enter the \a Stopped state. This can only succeed
         * if the component is not running and \a configureHook() returns true.
         */
        virtual bool configure();

        /**
         * Implement this method such that it contains the code which
         * will be executed when \a configure() is called. The default
         * implementation is an empty function which returns \a true.
         *
         * @retval true to indicate that configuration succeeded and
         * the Stopped state may be entered.
         * @retval false to indicate that configuration failed and the
         * Preoperational state is entered.
         */
        virtual bool configureHook();

        /**
         * This method starts the execution engine of this component.
         * This function calls \a startHook(), which must return \a true in order to
         * allow this component to run.
         * You can override this method to do something else or in addition
         * to starting the ExecutionEngine.
         * @return false if the engine was not assigned to an ActivityInterface
         * or if startHook() returned false or it was already started.
         */
        virtual bool start();
        
        /**
         * This method stops the execution engine of this component.
         * You can override this method to do something else or in addition
         * to stopping the engine. This function calls cleanupHook() as well.
         * @return false if the engine was not running.
         */
        virtual bool stop();

        /**
         * This method instructs a stopped component to enter the
         * pre-operational state again. It calls cleanupHook().
         * @return true if the component was in the stopped state.
         */
        virtual bool cleanup();

        /**
         * Implement this method such that it contains the code which
         * will be executed when \a cleanup() is called. The default
         * implementation is an empty function.
         */
        virtual void cleanupHook();
  
        /**
         * Inspect if the component is in the Running state.
         */
        virtual bool isRunning() const;

        /**
         * Inspect if the component is configured, i.e. in
         * the Stopped or Running state.
         */
        virtual bool isConfigured() const;

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
         * The properties of this TaskContext.
         */
        PropertyBag* properties() {
            return mattributes.properties();
        }

        /**
         * The properties of this TaskContext.
         */
        const PropertyBag* properties() const {
            return mattributes.properties();
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

        TaskState mTaskState;
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

    private:
        /**
         * The task-local ports.
         */
        DataFlowInterface dataPorts;
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
