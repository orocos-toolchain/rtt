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

#include "Factories.hpp"
#include "AttributeRepository.hpp"
#include "EventService.hpp"
#include "DataFlowInterface.hpp"
#include "ExecutionEngine.hpp"
#include "ScriptingAccess.hpp"
#include "TaskCore.hpp"
#include <corelib/PropertyBag.hpp>

#include <string>
#include <map>

namespace ORO_Execution
{
    class CommandProcessor;
    class ScriptingAccess;
    class TaskObject;

    /**
     * A TaskContext exports the commands, methods, events, properties and ports
     * a task has. Furthermore, it allows to visit its peer tasks.
     *
     * @section exec TaskContext interface
     * When a command is exported, one can access it using commands(). A similar
     * mechanism is available for properties(), methods(), events() and ports().
     * The commands of this TaskContext are executed by its
     * ExecutionEngine.
     *
     * @section exec Executing a TaskContext
     * In order to run the ExecutionEngine, the ExecutionEngine must
     * be invoked from an ActivityInterface implementation. As long as
     * there is no activity or the activity is not started, this
     * TaskContext will not accept any commands, nor process events,
     * nor execute programs or state machines.  In this way, the user
     * of this class can determine himself at which point and at which
     * moment commands and programs can be executed.
     *
     * @section exec Connecting TaskContexts
     * TaskContexts are connected using the unidirectional addPeer() or bidirectional
     * connectPeers() methods. These methods setup data connections and allow
     * 'peer' TaskContexts to use each other's interface.
     * In order to disconnect this task from its peers, use disconnect(), which
     * will disconnect all the Data Flow Ports and remove this object from its
     * Peers.
     */
    class TaskContext
        : public TaskCore
    {
        // non copyable
        TaskContext( TaskContext& );
    protected:
    
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

        

        void connectDataFlow( TaskContext* peer );
        void exportPorts();

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

        /**
         * Queue a command.
         * @return True if the Processor accepted the command.
         */
        bool executeCommand( CommandInterface* c);

        /**
         * Queue a command. If the Processor is not running or not accepting commands
         * this will fail and return zero.
         * @return The command id the processor returned.
         */
        int queueCommand( CommandInterface* c);

        /**
         * Add a one-way connection from this task to a peer task.
         * @param peer The peer to add.
         * @param alias An optional alias (another name) for the peer.
         * defaults to \a peer->getName()
         */
        bool addPeer( TaskContext* peer, std::string alias = "" );

        /**
         * Remove a one-way connection from this task to a peer task.
         */
        void removePeer( const std::string& name );

        /**
         * Remove a one-way connection from this task to a peer task.
         */
        void removePeer( TaskContext* peer );

        /**
         * Add a two-way connection from  this task to a peer task.
         */
        bool connectPeers( TaskContext* peer );

        /**
         * Disconnect this TaskContext from it's peers.
         * All its Data Flow Ports are disconnected from the connections but
         * the connections themselves may continue to exist to serve other TaskContexts.
         * This method invokes removePeer() as well on the peers listed in this->getPeerList().
         */
        void disconnect();

        /**
         * Reconnect the data ports of this task, without
         * removing the peer relationship. Use this if you
         * changed a port name of an already connected task.
         */
        void reconnect();

        /**
         * Remove a two-way connection from this task to a peer task.
         */
        void disconnectPeers( const std::string& name );

        /**
         * Return a standard container which contains all the Peer names
         * of this TaskContext.
         */
        PeerList getPeerList() const;

        /**
         * Return true if it knows a peer by that name.
         */
        bool hasPeer( const std::string& peer_name ) const;

        /**
         * Get a pointer to a peer of this task.
         * @return null if no such peer.
         */
        TaskContext* getPeer(const std::string& peer_name ) const;

        /** 
         * Add a new TaskObject to this TaskContext.
         * 
         * @param obj This object becomes owned by this TaskContext.
         * 
         * @return true if it cuold be added, false if such
         * object already exists.
         */
        bool addObject( OperationInterface *obj );

        /** 
         * Get a pointer to a previously added TaskObject
         * 
         * @param obj_name The name of the TaskObject
         * 
         * @return the pointer
         */
        OperationInterface* getObject(const std::string& obj_name );

        /** 
         * Get a list of all the object names of this TaskContext.
         * 
         * @return a list of string names.
         */
        ObjectList getObjectList() const;

        /** 
         * Remove and delete a previously added TaskObject.
         * 
         * @param obj_name The name of the TaskObject
         * 
         * @return true if found and removed, false otherwise.
         */
        bool removeObject(const std::string& obj_name );

        /**
         * Get a const pointer to the ExecutionEngine of this Task.
         * @see engine()
         */
        const ExecutionEngine* getExecutionEngine() const
        {
            return &ee;
        }

        /**
         * Get a pointer to the ExecutionEngine of this Task.
         * @see engine()
         */
        ExecutionEngine* getExecutionEngine()
        {
            return &ee;
        }

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
         * Returns the Processor of this task.
         * @deprecated by getExecutionEngine()
         */
        ExecutionEngine* getProcessor()
        {
            return &ee;
        }
#if 0
        /**
         * The Commands of this TaskContext.
         */
        GlobalCommandFactory* commands() {
            return &commandFactory;
        }

        /**
         * The Commands of this TaskContext.
         */
        const GlobalCommandFactory* commands() const{
            return &commandFactory;
        }

        /**
         * The Methods of this TaskContext.
         */
        GlobalMethodFactory* methods() {
            return &methodFactory;
        }

        /**
         * The Methods of this TaskContext.
         */
        const GlobalMethodFactory* methods() const{
            return &methodFactory;
        }
#endif
        /**
         * The DataSources of this TaskContext.
         */
        GlobalDataSourceFactory* datasources() {
            return &dataFactory;
        }

        /**
         * The DataSources of this TaskContext.
         */
        const GlobalDataSourceFactory* datasources() const{
            return &dataFactory;
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
        ORO_CoreLib::PropertyBag* properties() {
            return attributeRepository.properties();
        }

        /**
         * The properties of this TaskContext.
         */
        const ORO_CoreLib::PropertyBag* properties() const {
            return attributeRepository.properties();
        }

        /**
         * The task-local events ( 'signals' ) of this TaskContext.
         */
        EventService* events() {
            return &eventService;
        }

        /**
         * The task-local events ( 'signals' ) of this TaskContext.
         */
        const EventService* events() const {
            return &eventService;
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
         * The Command Factory of this TaskContext.
         * @deprecated by commands()
         */
        GlobalCommandFactory    commandFactory;
        /**
         * The DataSource Factory of this TaskContext.
         * @deprecated by datasources()
         */
        GlobalDataSourceFactory dataFactory;
        /**
         * The Method Factory of this TaskContext.
         * @deprecated by methods()
         */
        GlobalMethodFactory     methodFactory;

        /**
         * The task-local values ( attributes ) of this TaskContext.
         * @deprecated by attributes()
         */
        AttributeRepository     attributeRepository;

        /**
         * The task-local events ( 'signals' ) of this TaskContext.
         * @deprecated by events()
         */
        EventService            eventService;

        /**
         * The task-local ports.
         */
        DataFlowInterface dataPorts;
    };
}

#endif
