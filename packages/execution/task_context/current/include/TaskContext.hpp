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

#include <string>
#include <map>

namespace ORO_Execution
{
    class CommandProcessor;
    class ScriptingAccess;

    /**
     * A TaskContext groups the operations, events, datasources,
     * peer-tasks and processor a task has. It links related tasks
     * and allows to iterate over its peers.
     *
     * When a peer is added, the (script) programs of this task can access
     * the peer using peername.methodname() or peername.objname.methodname().
     * The commands of this TaskContext must be executed by this TaskContext's
     * CommandProcessor (which is what scripts take care of). The methods and datasources
     * can be queried by any peer TaskContext at any time.
     *
     * The ExecutionEngine::step() must be invoked separately from a
     * PeriodicTask or other ActivityInterface implementation, as long as
     * that Task is not started, this TaskContext will not accept any
     * commands.  In this way, the user of this class can determine
     * himself at which point and at which moment remote commands and
     * local programs can be executed.
     *
     * In order to disconnect this task from its peers, use disconnect(), which
     * will disconnect all the Data Flow Ports and remove this object from its
     * Peers.
     */
    class TaskContext
    {
        // non copyable
        TaskContext( TaskContext& );
    protected:
        std::string    _task_name;
    
        typedef std::map< std::string, TaskContext* > PeerMap;
        typedef std::vector< TaskContext* > Users;
        // map of the tasks we are using
        PeerMap         _task_map;
        // map of the tasks that are using us.
        Users         musers;

        ExecutionEngine ee;

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
         * Function where the user must insert his 'startup' code.
         * This function is called by the ExecutionEngine before it
         * starts its processors. If it returns \a false, the startup
         * of the TaskContext is aborted.  The default implementation is an
         * empty function which returns \a true.
         */
        virtual bool startup();

        /**
         * Function where the user must insert his 'application' code.
         * When the ExecutionEngine's Task is a PeriodicTask, this
         * function is called by the ExecutionEngine in each periodic
         * step after all command, event,... processors. When it's Task is a
         * TaskNonPeriodic, this function is called after an Event or
         * Command is received and executed.  It should not loop
         * forever, since no commands or events are processed when
         * this function executes.  The default implementation is an
         * empty function.
         */
        virtual void update();

        /**
         * Function where the user must insert his 'shutdown' code.
         * This function is called by the ExecutionEngine after it
         * stops its processors.  The default implementation is an
         * empty function.
         */
        virtual void shutdown();

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
         * Get the name of this TaskContext.
         */
        const std::string& getName()
        {
            return _task_name;
        }

        /**
         * Change the name of this TaskContext.
         */
        void setName(const std::string& n)
        {
            _task_name = n;
        }
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
         * Get a const pointer to the ExecutionEngine of this Task.
         * @see getExecutionEngine()
         */
        const ExecutionEngine* engine() const
        {
            return &ee;
        }

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
         * @see getExecutionEngine()
         */
        ExecutionEngine* engine()
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
        const AttributeRepository* attributes() const{
            return &attributeRepository;
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
        const EventService* events() const{
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
