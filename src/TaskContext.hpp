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

#include "interface/AttributeRepository.hpp"

#include "rtt-config.h"
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
#include "interface/EventService.hpp"
#endif

#include "interface/DataFlowInterface.hpp"
#include "ExecutionEngine.hpp"
#include "scripting/ScriptingAccess.hpp"
#include "interface/ExecutionAccess.hpp"
#include "interface/MarshallingAccess.hpp"
#include "base/TaskCore.hpp"
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
     * You can define the interface of a TaskContext by adding communication primitives.
     * These are commands(), properties(), methods(), events(), attributes() and ports().
     * Setting up the interface is explained at length in the Orocos Component Builder's
     * Manual.
     *
     * @par TaskContext state behaviour
     * When a TaskContext is created it defaults to the 'Stopped' state or
     * the 'PreOperational' state. If it is 'Stopped', it can be start()'ed as
     * soon as an activity object is attached to it. If it is 'PreOperational',
     * it must first be configure()'d before it can be started. You can choose
     * between both using the constructor.
     * @see TaskState (in base::TaskCore) for a detailed explanation.
     *
     * @par Executing a TaskContext
     * In order to run the ExecutionEngine, the ExecutionEngine must
     * be run by an base::ActivityInterface implementation. As long as
     * there is no activity or the activity is not started, this
     * TaskContext will not accept any commands, nor process events,
     * nor execute programs or state machines.  In this way, the user
     * of this class can determine himself at which point and at which
     * moment commands and programs can be executed.
     *
     * @par Connecting TaskContexts
     * TaskContexts are connected using the unidirectional addPeer() or bidirectional
     * RTT::connectPeers() methods. These methods only allow
     * 'peer' TaskContexts to use each other's interface. Use RTT::connectPorts()
     * to setup the data connections between data ports.
     * In order to disconnect this task from its peers, use disconnect(), which
     * will disconnect all the Data Flow Ports and remove this object from its
     * Peers.
     */
    class RTT_API TaskContext
        : public interface::OperationInterface,
          public base::TaskCore
    {
    public:
        /**
         * A list of Peer TaskContext names.
         */
        typedef std::vector< std::string > PeerList;
        /**
         * A list of internal internal::TaskObject names.
         */

        /**
         * Create a TaskContext.
         * It's ExecutionEngine will be newly constructed with private
         * processing of commands, events, programs and state machines.
         * @param name The name of this component.
         * @param initial_state Provide the \a PreOperational parameter flag here
         * to force users in calling configure(), before they call start().
         */
        TaskContext( const std::string& name, TaskState initial_state = Stopped );

        /**
         * Create a TaskContext.
         * Its commands programs and state machines are processed by \a parent.
         * Use this constructor to share execution engines among task contexts, such that
         * the execution of their functionality is serialised (executed in the same thread).
         * @param name The name of this component.
         * @param initial_state Provide the \a PreOperational parameter flag here
         * to force users in calling configure(), before they call start().
         */
        TaskContext(const std::string& name, ExecutionEngine* parent, TaskState initial_state = Stopped );

        virtual ~TaskContext();

        virtual const std::string& getName() const;

        virtual const std::string& getDescription() const;

        virtual void setDescription(const std::string& descr);

        virtual interface::OperationInterface* getParent() { return this; }

        /**
         * This method is ignored by the TaskContext.
         */
        virtual void setParent(interface::OperationInterface*) { }

        /**
         * This method is ignored by the TaskContext.
         * @see base::TaskCore::setExecutionEngine for (re-)setting a new
         * ExecutionEngine, which is a base class of TaskContext.
         */
        virtual void setEngine(ExecutionEngine*) { }

        /**
         * Call this function to force a TaskContext to export its
         * Data Flow ports as scripting objects.
         * @deprecated Do not use this function, it is no longer
         * required.
         */
        void exportPorts();

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
         * Add a new internal::TaskObject to this TaskContext.
         *
         * @param obj This object becomes owned by this TaskContext.
         *
         * @return true if it cuold be added, false if such
         * object already exists.
         */
        virtual bool addObject( interface::OperationInterface *obj );

        /**
         * Sets the activity of this TaskContext. The
         * activity is owned by the TaskContext and you should
         * not hold a pointer to it after this call. Use
         * getActivity() lateron to retrieve a safe pointer to it.
         * @param new_act The new activity for this TaskContext,
         * which becomes owned by this TaskContext.
         */
        void setActivity( base::ActivityInterface* new_act );

        /**
         * Get a pointer to the activity running this
         * component.
         * @return Our activity.
         */
        base::ActivityInterface* getActivity();

        /**
         * Clear the complete interface of this Component.
         * This method removes all objects and all methods, commands,
         * events, properties and ports from the interface of this TaskContext.
         */
        virtual void clear();

        /**
         * Checks the validity of this TaskContext.
         * When this method returns false, the TaskContext
         * should not be used and various methods may throw
         * exceptions. Use this in case the TaskContext could be
         * a remote object. Will always return true when this
         * TaskContext is local.
         */
        virtual bool ready();

        bool start();

        /**
         * Hook called in the Running state.
         *
         * This default implementation calls updateHook(updated_ports)
         */
        void updateHook();

        /**
         * This method gets called when new data is available on some input ports. The ports
         * are listed as argument to the method
         *
         * The default implementation does nothing;
         */
        virtual void updateHook(std::vector<base::PortInterface*> const& updated_ports);

        /**
         * Get access to high level controls for
         * programs, state machines and scripting
         * statements.
         */
        scripting::ScriptingAccess* scripting()
        {
            return mscriptAcc;
        }

        /**
         * Get access to high level controls for
         * programs, state machines and scripting
         * statements.
         */
        const scripting::ScriptingAccess* scripting() const
        {
            return mscriptAcc;
        }

        /**
         * Get access to high level controls for
         * controlling programs and state machines.
         * It is the implementation of the 'engine'
         * TaskObject.
         */
        interface::ExecutionAccess* execution()
        {
            return mengAcc;
        }

        /**
         * Get access to high level controls for
         * controlling programs and state machines.
         * It is the implementation of the 'engine'
         * TaskObject.
         */
        const interface::ExecutionAccess* execution() const
        {
            return mengAcc;
        }

        /**
         * Get access to high level controls for
         * (de)serializing properties to/from an XML format.
         */
        interface::MarshallingAccess* marshalling()
        {
            return marshAcc;
        }

        /**
         * Get access to high level controls for
         * (de)serializing properties to/from an XML format.
         */
        const interface::MarshallingAccess* marshalling() const
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
        interface::DataFlowInterface* ports() {
            return &dataPorts;
        }

        /**
         * Get the Data flow ports of this task.
         */
        const interface::DataFlowInterface* ports() const {
            return &dataPorts;
        }

    private:
        // non copyable
        TaskContext( TaskContext& );
    protected:
        std::string mdescription;

        typedef std::map< std::string, TaskContext* > PeerMap;
        typedef std::vector< TaskContext* > Users;
        typedef std::vector< interface::OperationInterface* > Objects;
        /// map of the tasks we are using
        PeerMap         _task_map;
        /// map of the tasks that are using us.
        Users         musers;
        /// the TaskObjects.
        Objects mobjects;

        scripting::ScriptingAccess* mscriptAcc;

        interface::ExecutionAccess* mengAcc;

        interface::MarshallingAccess* marshAcc;

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

    protected:
        std::vector< base::PortInterface* > updated_ports;
        /**
         * This callback is called each time data arrived on an
         * event port.
         */
        void dataOnPort(base::PortInterface*);
    private:
        /**
         * The task-local ports.
         */
        interface::DataFlowInterface dataPorts;

        /**
         * This pointer tracks our activity which is set by
         * setActivity. By default, a extras::SequentialActivity is assigned.
         */
        base::ActivityInterface::shared_ptr our_act;
    };

    /**
     * Connect the Data Flow Ports of A and B in both
     * directions, by matching port names.
     * @see TaskContext::connectPorts
     */
    RTT_API bool connectPorts(TaskContext* A, TaskContext* B);

    /**
     * Set up the Execution Flow (who knows who)
     * between A and B in both directions. Both will be able to
     * use each other's interface.
     * @see TaskContext::connectPeers
     */
    RTT_API bool connectPeers(TaskContext* A, TaskContext* B);
}

#endif
