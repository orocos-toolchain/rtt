#ifndef ORO_SERVICE_PROVIDER_HPP
#define ORO_SERVICE_PROVIDER_HPP

#include "../rtt-config.h"
#include "OperationRepository.hpp"
#include "../internal/OperationRepositoryPartFused.hpp"
#include "../internal/LocalMethod.hpp"
#include "../internal/MethodC.hpp"
#include "../internal/UnMember.hpp"
#include "../internal/GetSignature.hpp"

#include "ConfigurationInterface.hpp"
#include "../Operation.hpp"
#ifdef ORO_REMOTING
#include "../internal/RemoteMethod.hpp"
#endif
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/function_types/components.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace RTT
{ namespace interface {

    /**
     * This class allows storage and retrieval of operations,
     * attributes and properties provided by a component.
     *
     * Services can be nested in sub-services, although this is rare.
     *
     * @ingroup Services
     */
    class RTT_API Service
        : public OperationRepository,
          public ConfigurationInterface,
          public boost::enable_shared_from_this<Service>
    {
    public:
        typedef OperationRepository Factory;
        typedef boost::shared_ptr<Service> shared_ptr;
        typedef std::vector<std::string> ProviderNames;

        /**
         * Creates a service provider with a name and an owner.  Each
         * service must be owned by a TaskContext and the owner can be
         * set afterwards with setOwner.
         * @param name The name of this service.
         * @param owner The TaskContext that will execute the operations of this service.
         */
        static Service::shared_ptr Create(const std::string& name, TaskContext* owner = 0);

        /**
         * Creates a service provider with a name and an owner.  Each
         * service must be owned by a TaskContext and the owner can be
         * set afterwards with setOwner.
         * @param name The name of this service.
         * @param owner The TaskContext that will execute the operations of this service.
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
         * (indirectly).
         */
        TaskContext* getOwner() const { return mowner; }

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
        Service::shared_ptr provides() { return shared_from_this(); }

        /**
         * Returns a sub-service provider which resorts under
         * this service provider.
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
         * Clear all added operations from the repository, saving memory space.
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
         * use in a C++ Method object. Store the result of this
         * function in a Method<Signature> object.
         *
         * @param name The name of the operation to retrieve.
         *
         * @return A pointer to an operation repository part or a null pointer if
         * \a name was not found.
         */
        OperationRepositoryPart* getOperation( std::string name );

        /**
         * Removes a previously added operation.
         */
        void removeOperation( const std::string& name );

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
            this->add( op.getName(), new internal::OperationRepositoryPartFused<Signature>( &op ) );
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
            this->add( op.getName(), new internal::SynchronousOperationRepositoryPartFused<Signature>( &op ) );
            return op;
        }

        /**
         * Add an operation to the interface by means of a function.
         * The function \a func may be a \a free function (a 'C' function) or
         * an object member function, in which case serv may not be null
         *
         * @param name The name of the new operation
         * @param func A pointer to a function, for example, &foo ('C' function) or &Bar::foo (C++ class function).
         * @param serv A pointer to the object that will execute the function in case of a C++ class function,
         * or zero ('0') in case of a 'C' function.
         * @param et The ExecutionThread choice: will the owning TaskContext of this service execute
         * the function \a func in its own thread, or will the client's thread (the caller) execute \a func ?
         *
         * @return A newly created operation object, which you may further document or query.
         */
        template<class Func, class Service>
        Operation< typename internal::GetSignature<Func>::Signature >&
        addOperation( const std::string name, Func func, Service* serv = 0, ExecutionThread et = ClientThread )
        {
            typedef typename internal::GetSignature<Func>::Signature Signature;
            Operation<Signature>* op = new Operation<Signature>(name, func, serv, et);
            ownedoperations.push_back(op);
            return addOperation( *op );
        }

        /**
         * Add an operation to the synchronous interface by means of a function.
         * The function \a func may be a \a free function (a 'C' function) or
         * an object member function, in which case serv may not be null
         *
         * @param name The name of the new operation
         * @param func A pointer to a function, for example, &foo ('C' function) or &Bar::foo (C++ class function).
         * @param serv A pointer to the object that will execute the function in case of a C++ class function,
         * or zero ('0') in case of a 'C' function.
         * @param et The ExecutionThread choice: will the owning TaskContext of this service execute
         * the function \a func in its own thread, or will the client's thread (the caller) execute \a func ?
         *
         * @return A newly created operation object, which you may further document or query.
         */
        template<class Func, class Service>
        Operation< typename internal::GetSignature<Func>::Signature >&
        addSynchronousOperation( const std::string name, Func func, Service* serv = 0, ExecutionThread et = ClientThread )
        {
            typedef typename internal::GetSignature<Func>::Signature Signature;
            Operation<Signature>* op = new Operation<Signature>(name, func, serv, et);
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
            Operation<SignatureDS>* op = new Operation<SignatureDS>(name, boost::function<SignatureDS>(func), et);
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
            this->add( op.getName(), new internal::OperationRepositoryPartFusedDS<Signature,ObjT>( sp, &op) );
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
         * Create a MethodC object, a template-less operation invocation
         * object. This function is inferior to getOperation(std::string name).
         *
         * @param name The name of the operation
         *
         * @return An object which can invoke a operation.
         */
        internal::MethodC create(std::string name, ExecutionEngine* caller);

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
}}


#endif
