/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  Service.hpp

                        Service.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_SERVICE_PROVIDER_HPP
#define ORO_SERVICE_PROVIDER_HPP

#include "rtt-config.h"
#include "OperationInterface.hpp"
#include "DataFlowInterface.hpp"
#include "internal/OperationInterfacePartFused.hpp"
#include "internal/LocalOperationCaller.hpp"
#include "internal/OperationCallerC.hpp"
#include "internal/UnMember.hpp"
#include "internal/GetSignature.hpp"

#include "ConfigurationInterface.hpp"
#include "Operation.hpp"
#ifdef ORO_REMOTING
#include "internal/RemoteOperationCaller.hpp"
#endif
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/function_types/components.hpp>
#include <boost/enable_shared_from_this.hpp>
#if BOOST_VERSION >= 104000 && BOOST_VERSION <= 105030
#include <boost/smart_ptr/enable_shared_from_this2.hpp>
#endif

namespace RTT
{
    /**
     * @defgroup Services Service Interface
     * A Service consists of a configuration, operation and data flow
     * interface which can be used, or connected to, other components.
     * @ingroup CompModel
     */
    /**
     * This class allows storage and retrieval of operations,
     * attributes and properties provided by a component.
     *
     * Services can be nested in sub-services, although this is rare.
     *
     * @ingroup Services
     */
    class RTT_API Service
        : public OperationInterface,
          public ConfigurationInterface,
          public DataFlowInterface,
#if BOOST_VERSION >= 104000 && BOOST_VERSION <= 105030
          public boost::enable_shared_from_this2<Service>
#else
          public boost::enable_shared_from_this<Service>
#endif
    {
    public:
        typedef OperationInterface Factory;
        typedef boost::shared_ptr<Service> shared_ptr;
        typedef std::vector<std::string> ProviderNames;

        /**
         * Creates a Service with a name and an owner.  Each
         * service must be owned by a TaskContext and the owner can be
         * set afterwards with setOwner.
         * @param name The name of this service.
         * @param owner The TaskContext that will execute the operations of this service.
         * @warning When using boost < 1.40, the owner is not stored in the Service, until
         * the Service object is effectively added to the TaskContext.
         * @see getOwner()
         */
        static Service::shared_ptr Create(const std::string& name, TaskContext* owner = 0);

        /**
         * Creates a Service with a name and an owner.  Each
         * service must be owned by a TaskContext and the owner can be
         * set afterwards with setOwner.
         * @param name The name of this service.
         * @param owner The TaskContext that will execute the operations of this service.
         * @warning When using boost < 1.40, the owner is not stored in the Service, until
         * the Service object is effectively added to the TaskContext.
         * @see getOwner()
         */
        Service(const std::string& name, TaskContext* owner = 0);

        virtual ~Service();

        /**
         * Returns the name of this service instance.
         */
        const std::string& getName() const { return mname; }

        /**
         * Returns a descriptive text for this service.
         */
        const std::string& doc() const { return mdescription; }

        /**
         * Sets the descriptive text for this service.
         */
        void doc(const std::string& description) { mdescription = description; }

        /**
         * Changes the name of this service.
         */
        void setName(const std::string& name) { mname = name;}

        /**
         * Sets the owning TaskContext that will execute the
         * operations in this service.
         */
        void setOwner(TaskContext* new_owner);

        /**
         * Sets the parent service in case this service is
         * a sub-service.
         */
        void setParent(shared_ptr new_parent);

        /**
         * The parent is the direct parent of this service.
         */
        shared_ptr getParent() const { return parent; }

        /**
         * Return a standard container which contains all the sub-service names
         * of this Service
         */
        virtual ProviderNames getProviderNames() const;

        /**
         * The owner is the top-level TaskContext owning this service
         * (indirectly). A Service can only belong to one TaskContext.
         * @note This function will only return the owner after the Service
         * has been added to the TaskContext.
         */
        TaskContext* getOwner() const { return mowner; }

        /**
         * Return the execution engine of this service owner.
         * \see getOwner
         */        
        ExecutionEngine* getOwnerExecutionEngine() const;

        /**
         * Add a new Service to this TaskContext.
         *
         * @param obj This object becomes owned by this TaskContext.
         *
         * @return true if it could be added, false if such
         * service already exists.
         */
        virtual bool addService( shared_ptr obj );

        /**
         * Remove a previously added sub-service.
         * @param the name of the service to remove.
         */
        virtual void removeService( std::string const& service_name );

        /**
         * Returns this Service.
         * @return a shared pointer from this.
         */
        Service::shared_ptr provides();

        /**
         * Returns a sub-Service which resorts under
         * this Service.
         * @param service_name The name of the sub-service.
         */
        Service::shared_ptr provides(const std::string& service_name);
        /**
         * Returns a shared pointer to strictly a sub-service.
         * This method will not return the this pointer when
         * service_name equals "this".
         */
        shared_ptr getService(const std::string& service_name);

        /**
         * Check if this service has the sub-service \a service_name.
         */
        bool hasService(const std::string& service_name);

        /**
         * Clear all added operations, properties, attributes, ports and services from the repository,
         * saving memory space.
         */
        void clear();

        /**
         * Returns the names of all operations added to this interface.
         * @see getNames() to get a list of all operations available to scripting.
         */
        std::vector<std::string> getOperationNames() const;

        /**
         * Query for the existence of a Operation in this interface.
         * @see hasMember() to verify if a operation is available to scripting as well.
         */
        bool hasOperation(const std::string& name) const;

        /**
         * Add an Operation object to the operation interface. This version
         * of addOperation does not add the Operation object to the remote
         * interface and only to the local, in-process C++ interface.
         *
         * @note Do not use this function unless you know what you're doing.
         * @see addOperation() for adding normal, remotely available operations.
         *
         * @param op The Operation object to add
         *
         * @return true if it could be added, false otherwise.
         */
        bool addLocalOperation( base::OperationBase& op );

        /**
         * Get a locally added operation from this interface.
         *
         * @note Do not use this function unless you know what you're doing.
         * @see getOperation() for getting normal, remotely available operations.
         */
        boost::shared_ptr<base::DisposableInterface> getLocalOperation( std::string name );

        /**
         * Get a previously added operation for
         * use in a C++ OperationCaller object. Store the result of this
         * function in a OperationCaller<Signature> object.
         *
         * @param name The name of the operation to retrieve.
         *
         * @return A pointer to an operation interface part or a null pointer if
         * \a name was not found.
         */
        OperationInterfacePart* getOperation( std::string name );

        /**
         * Removes a previously added operation.
         */
        void removeOperation( const std::string& name );

        /**
         * Sets the thread of execution of a previously added operation.
         * Only the component itself should modify this property. Don't call
         * this function from outside the component, it has unspecified behaviour
         * when not called from within one of the member functions or a service of
         * the current component.
         *
         * This function is typically used to change the ExecutionThread of
         * the standard component hooks such as startHook(), configureHook() etc.
         * which all have the ClientThread policy by default.
         *
         * @param name The name of the operation to modify. For example, "start".
         * @param et   The ExecutionThread type in which the function of the
         * operation will be executed, being OwnThread or ClientThread.
         * @return true if \a name was a local, present operation, false otherwise.
         */
        bool setOperationThread(std::string const& name, ExecutionThread et);

        /**
         * Add an operation object to the interface. This version
         * of addOperation exports an existing Operation object to the
         * public interface of this component.
         *
         * @param op The operation object to add.
         *
         * @return The given parameter \a op
         */
        template<class Signature>
        Operation<Signature>& addOperation( Operation<Signature>& op )
        {
            if ( this->addLocalOperation( op ) == false )
                return op;
            this->add( op.getName(), new internal::OperationInterfacePartFused<Signature>( &op ) );
            return op;
        }

        /**
         * Add an operation object to the interface. This version exports an
         * existing Operation object to the synchronous interface of the component
         *
         * @param op The operation object to add.
         *
         * @return The given parameter \a op
         */
        template<class Signature>
        Operation<Signature>& addSynchronousOperation( Operation<Signature>& op )
        {
            if ( this->addLocalOperation( op ) == false )
                return op;
            this->add( op.getName(), new internal::SynchronousOperationInterfacePartFused<Signature>( &op ) );
            return op;
        }

        /**
         * Add an operation to the interface by means of a C++ function.
         * The function \a func must be a C++ member function and
         * \a serv is the object having that function.
         *
         * @param name The name of the new operation
         * @param func A pointer to a function, for example, &Bar::foo (C++ class function).
         * @param serv A pointer to the object that will execute the function.
         * @param et The ExecutionThread choice: will the owning TaskContext of this service execute
         * the function \a func in its own thread, or will the client's thread (the caller) execute \a func ?
         *
         * @return A newly created operation object, which you may further document or query.
         */
        template<class Func, class Service>
        Operation< typename internal::GetSignature<Func>::Signature >&
        addOperation( const std::string name, Func func, Service* serv, ExecutionThread et = ClientThread )
        {
            typedef typename internal::GetSignature<Func>::Signature Signature;
            Operation<Signature>* op = new Operation<Signature>(name, func, serv, et, this->getOwnerExecutionEngine() );
            ownedoperations.push_back(op);
            return addOperation( *op );
        }

        /**
         * Add an operation to the interface by means of a C function.
         * The function \a func must be a C function.
         *
         * @param name The name of the new operation
         * @param func A pointer to a C function, for example, &foo (or a \b static C++ class function).
         * @param et The ExecutionThread choice: will the owning TaskContext of this service execute
         * the function \a func in its own thread, or will the client's thread (the caller) execute \a func ?
         *
         * @return A newly created operation object, which you may further document or query.
         */
        template<class Func>
        Operation< Func >&
        addOperation( const std::string name, Func* func, ExecutionThread et = ClientThread )
        {
            typedef Func Signature;
            boost::function<Signature> bfunc = func;
            Operation<Signature>* op = new Operation<Signature>(name, bfunc, et, this->getOwnerExecutionEngine() );
            ownedoperations.push_back(op);
            return addOperation( *op );
        }

        /**
         * Add an operation to the synchronous interface by means of a function.
         * The function \a func must be a C++ function.
         *
         * @param name The name of the new operation
         * @param func A pointer to a function, for example &Bar::foo (C++ class function).
         * @param serv A pointer to the object that will execute the function in case of a C++ class function,
         * @param et The ExecutionThread choice: will the owning TaskContext of this service execute
         * the function \a func in its own thread, or will the client's thread (the caller) execute \a func ?
         *
         * @return A newly created operation object, which you may further document or query.
         */
        template<class Func, class Service>
        Operation< typename internal::GetSignature<Func>::Signature >&
        addSynchronousOperation( const std::string name, Func func, Service* serv, ExecutionThread et = ClientThread )
        {
            typedef typename internal::GetSignature<Func>::Signature Signature;
            Operation<Signature>* op = new Operation<Signature>(name, func, serv, et, this->getOwnerExecutionEngine() );
            ownedoperations.push_back(op);
            return addSynchronousOperation( *op );
        }

        /**
         * For internal use only. The pointer of the object of which a member function
         * must be invoked is stored in a internal::DataSource such that the pointer can change
         * during program execution. Required in scripting for state machines.
         */
        template<class Func,class ObjT>
        Operation< typename internal::GetSignatureDS<Func>::Signature>& addOperationDS( const std::string& name, Func func, internal::DataSource< boost::shared_ptr<ObjT> >* sp,
                ExecutionThread et = ClientThread)
        {
            typedef typename internal::GetSignatureDS<Func>::Signature SignatureDS;    // function signature with normal object pointer
            Operation<SignatureDS>* op = new Operation<SignatureDS>(name, boost::function<SignatureDS>(func), et, this->getOwnerExecutionEngine() );
            ownedoperations.push_back(op);
            return addOperationDS( sp, *op );
        }

        /**
         * For internal use only. The pointer of the object of which a member function
         * must be invoked is stored in a internal::DataSource such that the pointer can change
         * during program execution. Required in scripting for state machines.
         */
        template<class Signature,class ObjT>
        Operation<Signature>& addOperationDS( internal::DataSource< boost::shared_ptr<ObjT> >* sp, Operation<Signature>& op)
        {
            if ( this->addLocalOperation( op ) == false ) {
                assert(false);
                return op; // should never be reached.
            }
            this->add( op.getName(), new internal::OperationInterfacePartFusedDS<Signature,ObjT>( sp, &op) );
            return op;
        }

        /**
         * For internal use only. Get a previously added operation as a DataSource.
         * This function is inferior to getOperation(std::string name)
         *
         * @param name The name of the operation
         * @param args The arguments of the operation as Data Sources.
         *
         * @return A internal::DataSource which, when evaluated, invokes the operation.
         */
        base::DataSourceBase::shared_ptr getOperation( std::string name,
                                   const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const
        {
            return this->produce(name, args, caller);
        }

        /**
         * Create a OperationCallerC object, a template-less operation invocation
         * object. This function is inferior to getOperation(std::string name).
         *
         * @param name The name of the operation
         *
         * @return An object which can invoke a operation.
         */
        internal::OperationCallerC create(std::string name, ExecutionEngine* caller);

        /**
         * Reset the implementation of a operation.
         */
        bool resetOperation(std::string name, base::OperationBase* impl);
    protected:
        typedef std::map< std::string, shared_ptr > Services;
        /// the services we implement.
        Services services;

        bool testOperation(base::OperationBase& op);
        typedef std::map<std::string,base::OperationBase* > SimpleOperations;
        typedef std::vector<base::OperationBase*> OperationList;
        SimpleOperations simpleoperations;
        OperationList ownedoperations;
        std::string mname;
        std::string mdescription;
        TaskContext* mowner;
        shared_ptr parent;
    };
}


#endif
