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
#include "plugin/PluginLoader.hpp"
#include "interface/Service.hpp"
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
     * the 'PreOperational' state. If it is 'Stopped', it can be start()'ed. If it is 'PreOperational',
     * it must first be configure()'d before it can be started. You can choose
     * between both using the constructor.
     * @see TaskState (in base::TaskCore) for a detailed explanation.
     *
     * @par Executing a TaskContext
     * In order to run the ExecutionEngine, the ExecutionEngine must
     * be run by an base::ActivityInterface implementation. As long as
     * there is no activity or the activity is not started, this
     * TaskContext will not accept any asynchronous invocations,
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
         * Returns the name of this TaskContext.
         */
        virtual const std::string& getName() { return tcservice->getName(); }

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
         * Get a pointer to the activity running this
         * component.
         *
         * Unlike the non-template version, it converts it to the activity type
         * provided.
         *
         * Example usage in a startHook():
         * <code>
         * RTT::extras::FileDescriptorActivity* fd_activity =
         *    getActivity<RTT::extras::FileDescriptorActivity>();
         * if (fd_activity)
         * {
         *    // do specific setup for FDActivity
         * }
         * </code>
         *
         * @return Our activity if it is of type T, NULL otherwise.
         */
        template<typename T>
        T* getActivity() { return dynamic_cast<T*>(getActivity()); }

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

        virtual bool start();
        virtual bool stop();

        /**
         * These functions are used to setup and manage peer-to-peer networks
         * of TaskContext objects.
         *
         * @name Peer-to-Peer functions
         * @{
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
         * Disconnect this TaskContext from it's peers and ports.
         * All its Data Flow Ports are disconnected as well.
         * This method invokes removePeer() as well on the peers listed in this->getPeerList().
         */
        virtual void disconnect();

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
        /** @} */

        /**
         * These functions are used to create and manage services.
         * Use provides() or requires() to access the Service
         * or ServiceRequester objects that contain all service related functions.
         * @name Services
         * @{
         */

        /**
         * Returns this Service.
         * @return a shared pointer from this.
         */
        interface::Service::shared_ptr provides() { return tcservice; }

        /**
         * Returns a sub-service provider which resorts under
         * this service provider.
         * @param service_name The name of the sub-service.
         */
        interface::Service::shared_ptr provides(const std::string& service_name) { return tcservice->provides(service_name); }

        /**
         * Returns the object that manages which methods this Task
         * requires to be implemented by another task.
         */
        interface::ServiceRequester* requires() { return tcrequests; }

        /**
         * Returns the object that manages which methods this Task
         * requires to be implemented by another service.
         */
        interface::ServiceRequester* requires(const std::string& service_name) {
            return tcrequests->requires(service_name);
        }

        /**
         * Connects all requires/provides services of this component to these of a peer.
         */
        virtual bool connectServices( TaskContext* peer);

        /**
         * Use this method to be able to make OperationCaller calls to services provided by this component.
         * In case the service does not exist in this component, it tries to load the service using the plugin::PluginLoader class.
         * If all fails, a null pointer is returned.
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
            if (!provides()->hasService(name) && plugin::PluginLoader::Instance()->loadService(name, this) == false)
                return boost::shared_ptr<ServiceType>();
            LocalServices::iterator it = localservs.find(name);
            if (  it != localservs.end() ) {
                return boost::dynamic_pointer_cast<ServiceType>(it->second);
            }
            boost::shared_ptr<ServiceType> st = boost::make_shared<ServiceType>(this);
            st->connectTo( provides(name) );
            localservs[name] = st;
            return st;
        }
        /** @} */

        /**
         * Adding and getting operations from the TaskContext interface.
         * These functions all forward to the service provider representing
         * this TaskContext. Use operations() to access the complete OperationRepository
         * interface of this TaskContext.
         * @name Operations
         * @{ */
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

        template<class Func, class Service>
        Operation< typename internal::GetSignature<Func>::Signature >&
        addOperation( const std::string name, Func func, Service* serv = 0, ExecutionThread et = ClientThread )
        {
            return tcservice->addOperation(name,func, serv, et);
        }

        /**
         * Get a previously added operation for
         * use in a C++ OperationCaller object. Store the result of this
         * function in a OperationCaller<\a Signature> object.
         *
         * @param name The name of the operation to retrieve.
         *
         * @return true if it could be found, false otherwise.
         */
        interface::OperationRepositoryPart* getOperation( std::string name )
        {
            return tcservice->getOperation(name);
        }

        /**
         * Returns the operations of this TaskContext as an OperationRepository.
         */
        interface::OperationRepository* operations() { return this->provides().get(); }

        /** @} */

        /**
         * Adding and getting attributes from the TaskContext interface.
         * These functions all forward to the service provider representing
         * this TaskContext. Use attributes() to access the complete
         * ConfigurationInterface interface of this TaskContext.
         * @name Attributes
         * @{ */
        /**
         * Adds a variable of any type as read/write attribute to the attribute interface.
         * An Alias is created which causes contents of the \a attr
         * variable always to be in sync
         * with the contents of the attribute object in the interface.
         * @param name The name of this attribute
         * @param attr The variable that will be aliased.
         */
        template<class T>
        bool addAttribute( const std::string& name, T& attr) {
            return tcservice->addAttribute(name, attr);
        }

        /**
         * Adds a variable of any type as read-only attribute to the attribute interface.
         * An Alias is created which causes contents of the
         * attribute always to be in sync
         * with the contents of \a attr, but it can only be read through the interface.
         * @param name The name of this attribute
         * @param attr The variable that will be aliased.
         */
        template<class T>
        bool addConstant( const std::string& name, const T& attr) {
            return tcservice->addConstant(name, attr);
        }

        /**
         * Add an base::AttributeBase which remains owned by the
         * user. This is a low-level function that can be used
         * if you already created an Attribute object that does
         * not belong yet to a service.
         *
         * @param a remains owned by the user, and becomes
         * served by the repository.
         */
        bool addAttribute( base::AttributeBase& a )
        {
            return tcservice->addAttribute(a);
        }

        /**
         * Retrieve a Attribute by name. Returns zero if
         * no Attribute by that name exists.
         * @example
           Attribute<double> d_attr = getAttribute("Xval");
           @endexample
         * @see addAttribute to add an Attribute.
         */
        base::AttributeBase* getAttribute( const std::string& name ) const
        {
            return tcservice->getAttribute(name);
        }

        /**
         * Returns the attributes of this TaskContext as an ConfigurationInterface.
         */
        interface::ConfigurationInterface* attributes() { return this->provides().get(); }

        /** @} */

        /**
         * Adding and getting properties from the TaskContext interface.
         * These functions all forward to the service provider representing
         * this TaskContext. Use properties() to access the complete PropertyBag
         * interface of this TaskContext.
         * @name Properties
         * @{ */
        /**
         * Adds a variable of any type as a property to the attribute interface.
         * A Property is created which causes contents of the
         * property always to be in sync
         * with the contents of \a attr.
         * @param name The name of this property
         * @param attr The variable that will be aliased.
         * @return the Property object by reference, which you can further query or document.
         */
        template<class T>
        Property<T>& addProperty( const std::string& name, T& attr) {
            return tcservice->addProperty(name, attr);
        }

        /**
         * Add an base::PropertyBase as a property.
         * @return false if a property with the same name already exists.
         */
        bool addProperty( base::PropertyBase& pb ) {
            return tcservice->addProperty(pb);
        }

        /**
         * Get a Property with name \a name.
         *
         * @param  name The name of the property to search for.
         * @return The Property with this name, zero
         *         if it does not exist
         */
        base::PropertyBase* getProperty(const std::string& name) const
        {
            return tcservice->getProperty(name);
        }

        /**
         * Returns the properties of this TaskContext as a PropertyBag.
         */
        PropertyBag* properties() { return this->provides()->properties(); }

        /** @} */

        /**
         * These functions serve to manage ports and data flow
         * connections. Use ports() to access the complete
         * DataFlowInterface functionality of this TaskContext.
         * @name Ports
         * @{
         */
        /**
         * Name and add a Port to the interface of this task and
         * add a Service with the same name of the port.
         * @param name The name to give to the port.
         * @param port The port to add.
         */
        base::PortInterface& addPort(const std::string& name, base::PortInterface& port) {
            port.setName(name);
            return ports()->addPort(port);
        }

        /**
         * Add a Port to the interface of this task and
         * add a Service with the same name of the port.
         * @param port The port to add.
         */
        base::PortInterface& addPort(base::PortInterface& port) {
            return ports()->addPort(port);
        }

        /**
         * Name and add an Event triggering Port to the interface of this task and
         * add a Service with the same name of the port.
         * @param name The name to give to the port.
         * @param port The port to add.
         * @param callback (Optional) provide a function which will be called asynchronously
         * when new data arrives on this port. You can add more functions by using the port
         * directly using base::PortInterface::getNewDataOnPort().
         * @note This function will temporarily stop your TaskContext and
         * re-start it in case it was running.
         */
        base::InputPortInterface& addEventPort(const std::string& name, base::InputPortInterface& port, base::InputPortInterface::NewDataOnPortEvent::SlotFunction callback = base::InputPortInterface::NewDataOnPortEvent::SlotFunction() ) {
            port.setName(name);
            return ports()->addEventPort(port,callback);
        }

        /**
         * Add an Event triggering Port to the interface of this task and
         * add a Service with the same name of the port.
         * @param port The port to add.
         * @param callback (Optional) provide a function which will be called asynchronously
         * when new data arrives on this port. You can add more functions by using the port
         * directly using base::PortInterface::getNewDataOnPort().
         * @note This function will temporarily stop your TaskContext and
         * re-start it in case it was running.
         */
        base::InputPortInterface& addEventPort(base::InputPortInterface& port, base::InputPortInterface::NewDataOnPortEvent::SlotFunction callback = base::InputPortInterface::NewDataOnPortEvent::SlotFunction() ) {
            return ports()->addEventPort(port,callback);
        }

        /**
         * Get a port of this Component.
         * @param name The port name
         * @return a pointer to a port or null if it does not exist.
         */
        base::PortInterface* getPort(const std::string& name) const {
            return ports()->getPort(name);
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

        /**
         * Add a data flow connection from this task's ports to a peer's ports.
         */
        virtual bool connectPorts( TaskContext* peer );
        /** @} */

    protected:

        /**
         * Hook called in the Running state.
         *
         * This default implementation calls updateHook(updated_ports)
         */
        virtual void updateHook();

        /**
         * This method gets called when new data is available on some input ports. The ports
         * are listed as argument to the method
         *
         * The default implementation does nothing;
         */
        virtual void updateHook(std::vector<base::PortInterface*> const& updated_ports);

        /**
         * This method allows to test in updateHook() if a specific port has
         * triggered this particular update.
         *
         * This works only in updateHook(), and allows only to test ports that
         * have been added to the data flow interface using
         * DataFlowInterface::addEventPort.
         */
        bool isPortUpdated(base::PortInterface const& port) const;

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

        /**
         * Internal function to setup the 'this' service and activity.
         */
        void setup();

        friend class interface::DataFlowInterface;
        typedef std::vector< base::PortInterface* > PortList;
        PortList updated_ports;
        internal::MWSRQueue<base::PortInterface*>* portqueue;
        typedef std::map<base::PortInterface*, boost::shared_ptr<base::InputPortInterface::NewDataOnPortEvent> > UserCallbacks;
        UserCallbacks user_callbacks;

        /**
         * This callback is called each time data arrived on an
         * event port.
         */
        void dataOnPort(base::PortInterface* port);
        /**
         * Called to inform us of the number of possible
         * ports that will trigger a dataOnPort event.
         * @return false if this->isRunning().
         */
        bool dataOnPortSize(unsigned int max);
        /**
         * Function to call in the thread of this component if data on the given port arrives.
         */
        void dataOnPortCallback(base::InputPortInterface* port, base::InputPortInterface::SlotFunction callback);
        /**
         * Inform that a given port will no longer raise dataOnPort() events.
         */
        void dataOnPortRemoved(base::PortInterface* port);

        typedef std::map<std::string, boost::shared_ptr<interface::ServiceRequester> > LocalServices;
        LocalServices localservs;

        interface::Service::shared_ptr tcservice;
        interface::ServiceRequester*           tcrequests;
    private:
        // non copyable
        TaskContext( TaskContext& );
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
