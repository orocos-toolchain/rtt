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


#include "rtt-config.h"
#include "interface/ServiceProvider.hpp"
#include "interface/ServiceRequester.hpp"
#include "interface/DataFlowInterface.hpp"
#include "ExecutionEngine.hpp"
#include "base/TaskCore.hpp"
#include <boost/make_shared.hpp>

#include <string>
#include <map>

namespace RTT
{
    /**
     * The TaskContext is the C++ representation of an Orocos component.
     * It defines which services it provides and requires and which ports are inputs and
     * outputs. It can be configured through the means of properties.
     *
     * @par TaskContext interface
     * You can define the interface of a TaskContext by adding interface primitives.
     * These are (data flow) ports, (configuration) properties and operations (functions).
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
     * TaskContext will not accept any asynchronous invocations, nor process events,
     * nor execute programs or state machines.
     *
     * @par Connecting TaskContexts
     * TaskContexts are connected using the unidirectional addPeer() or bidirectional
     * RTT::connectPeers() methods. These methods only allow
     * 'peer' TaskContexts to use each other's interface. Use RTT::connectPorts()
     * to setup the data connections between data ports.
     * In order to disconnect this task from its peers, use disconnect(), which
     * will disconnect all the Data Flow Ports and remove this object from its
     * Peers.
     *
     * In day-to-day use, TaskContexts are connected using the OCL::DeploymentComponent
     * and an XML file or script.
     */
    class RTT_API TaskContext
        : public base::TaskCore
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
         * Connects all requires/provides services of this component to these of a peer.
         */
        virtual bool connectServices( TaskContext* peer);

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
         * @begingroup tc-serv TaskContext Service Interface
         */

        virtual const std::string& getName() { return tcservice->getName(); }

        /**
         * Returns this ServiceProvider.
         * @return a shared pointer from this.
         */
        interface::ServiceProvider::shared_ptr provides() { return tcservice; }

        /**
         * Returns a sub-service provider which resorts under
         * this service provider.
         * @param service_name The name of the sub-service.
         */
        interface::ServiceProvider::shared_ptr provides(const std::string& service_name) { return tcservice->provides(service_name); }

        interface::ServiceRequester* requires() { return tcrequests; }

        interface::ServiceRequester* requires(const std::string& service_name) {
            return tcrequests->requires(service_name);
        }

        PropertyBag* properties() { return this->provides()->properties(); }

        /**
         * Add an operation object to the interface. This version
         * of addOperation exports an Operation object to the
         * public interface of this component.
         *
         * @param op The operation object to add.
         *
         * @return true if it could be added.
         */
        template<class Signature>
        Operation<Signature>& addOperation( Operation<Signature>& op )
        {
            return tcservice->addOperation(op);
        }

        /**
         * Returns a function signature from a C++ member function
         */
        template<class FunctionT>
        struct GetSignature {
            typedef typename internal::UnMember< typename boost::remove_pointer<FunctionT>::type >::type Signature;
        };

        // UnMember serves to remove the member function pointer from the signature of func.
        template<class Func, class Service>
        Operation< typename GetSignature<Func>::Signature >&
        addOperation( const std::string name, Func func, Service* serv = 0, ExecutionThread et = ClientThread )
        {
            return tcservice->addOperation(name,func, serv,et);
        }

        /**
         * Get a previously added operation for
         * use in a C++ Method object. Store the result of this
         * function in a Method<\a Signature> object.
         *
         * @param name The name of the operation to retrieve.
         * @param Signature The function signature of the operation, for
         * example: getOperation<int(double)>("name");
         *
         * @return true if it could be found, false otherwise.
         */
        template<class Signature>
        boost::shared_ptr<base::DisposableInterface> getOperation( std::string name )
        {
            return tcservice->getOperation<Signature>(name);
        }

        /**
         * Sets the activity of this TaskContext. The
         * activity is owned by the TaskContext and you should
         * not hold a pointer to it after this call. Use
         * getActivity() lateron to retrieve a safe pointer to it.
         * @param new_act The new activity for this TaskContext,
         * which becomes owned by this TaskContext.
         * @return false if this->isRunning(). You can not change the
         * activity of a TaskContext once it is running.
         * @note This function may not be called from the current
         * ExecutionEngine thread (OwnThread), another thread (ClientThread) must call this function.
         */
        bool setActivity( base::ActivityInterface* new_act );

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
    protected:

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

    protected:
        /**
         * This method allows to test in updateHook() if a specific port has
         * triggered this particular update.
         *
         * This works only in updateHook(), and allows only to test ports that
         * have been added to the data flow interface using
         * DataFlowInterface::addEventPort.
         */
        bool isPortUpdated(base::PortInterface const& port) const;

    public:
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

        /**
         * Use this method to be able to make Method calls to services provided by this component.
         *
         * For example: getProvider<Scripting>("scripting")->loadPrograms("file.ops");
         *
         * @param name The name of the service to get, must have been added with
         * addService.
         * @param ServiceType the ServiceRequester type to use. Must have a constructor that takes TaskContext* as argument.
         * @return a shared ServiceRequester object which allows to call the operations provided by service \a name.
         */
        template<class ServiceType>
        boost::shared_ptr<ServiceType> getProvider(const std::string& name) {
            if (!provides()->hasService(name)) return boost::shared_ptr<ServiceType>();
            LocalServices::iterator it = localservs.find(name);
            if (  it != localservs.end() ) {
                return boost::dynamic_pointer_cast<ServiceType>(it->second);
            }
            boost::shared_ptr<ServiceType> st = boost::make_shared<ServiceType>(this);
            st->connectTo( provides(name) );
            localservs[name] = st;
            return st;
        }

    private:
        // non copyable
        TaskContext( TaskContext& );
    protected:

        typedef std::map< std::string, TaskContext* > PeerMap;
        typedef std::vector< TaskContext* > Users;
        /// map of the tasks we are using
        PeerMap         _task_map;
        /// map of the tasks that are using us.
        Users         musers;

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
        typedef std::vector< base::PortInterface* > PortList;
        PortList updated_ports;
        internal::AtomicQueue<base::PortInterface*> portqueue;

        /**
         * This callback is called each time data arrived on an
         * event port.
         */
        void dataOnPort(base::PortInterface*);

        typedef std::map<std::string, boost::shared_ptr<interface::ServiceRequester> > LocalServices;
        LocalServices localservs;

        interface::ServiceProvider::shared_ptr tcservice;
        interface::ServiceRequester*           tcrequests;
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
