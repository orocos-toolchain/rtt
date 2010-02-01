#ifndef ORO_SERVICE_PROVIDER_HPP
#define ORO_SERVICE_PROVIDER_HPP

#include "../rtt-config.h"
#include "OperationRepository.hpp"
#include "../internal/OperationRepositoryPart.hpp"
#include "../internal/LocalMethod.hpp"
#include "../internal/MethodC.hpp"
#include "../internal/UnMember.hpp"

#include "AttributeRepository.hpp"
#include "../Operation.hpp"
#ifdef ORO_REMOTING
#include "../internal/RemoteMethod.hpp"
#endif
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/function_types/components.hpp>

namespace RTT
{ namespace interface {

    /**
     * This class allows storage and retrieval of operations,
     * attributes and properties provided by a component.
     *
     * @ingroup Services
     */
    class RTT_API ServiceProvider
        : public OperationRepository,
          public AttributeRepository
    {
    public:
        typedef OperationRepository Factory;

        ServiceProvider(const std::string& name, TaskContext* owner = 0);

        virtual ~ServiceProvider();

        const std::string& getName() const { return mname; }

        const std::string& getDescription() const { return mdescription; }

        void setDescription(const std::string& d) { mdescription = d;}

        void doc(const std::string& d) { mdescription = d; }

        void setName(const std::string& n) { mname = n;}

        void setOwner(TaskContext* new_owner);

        void setParent(ServiceProvider* new_parent);
        /**
         * The parent is the direct parent of this service.
         */
        ServiceProvider* getParent() const { return parent; }

        /**
         * Return a standard container which contains all the sub-service names
         * of this Service
         */
        virtual std::vector<std::string> getServiceNames() const;

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
        virtual bool addService( interface::ServiceProvider *obj );

        /**
         * Remove and destroy a previously added sub-service.
         * @param the name of the service to remove.
         */
        virtual void removeService( std::string const& service_name );

        /**
         * Unmount a sub-service from this service, without destroying it.
         * @param the name of the service to unmount.
         * @return The unmounted service object, or null if name was not known.
         */
        virtual ServiceProvider* unmountService( string const& name);

        ServiceProvider* provides() { return this; }

        ServiceProvider* provides(const std::string& service_name) {
            ServiceProvider* sp = services[service_name];
            if (sp)
                return sp;
            sp = new ServiceProvider(service_name, mowner);
            services[service_name] = sp;
            return sp;
        }

        bool hasService(const std::string& service_name) {
            return services.find(service_name) != services.end();
        }


        /**
         * Clear all added operations from the repository, saving memory space.
         */
        void clear();

        /**
         * Returns the names of all operations added to this interface.
         * @see getNames() to get a list of all operations available to scripting.
         */
        std::vector<std::string> getOperations() const;

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
            Logger::In in("ServiceProvider::getOperation");
            if ( simpleoperations.count(name) ) {
                if ( boost::dynamic_pointer_cast< base::MethodBase<Signature> >(simpleoperations[name]->getImplementation()) )
                    return simpleoperations[name]->getImplementation();
                else
                    log(Error) << "Operation '"<< name <<"' found, but has wrong Signature."<<endlog();
                return boost::shared_ptr<base::DisposableInterface>();
            }

#ifdef ORO_REMOTING
            if ( this->hasMember(name ) ) {
                return boost::shared_ptr<base::DisposableInterface>(new internal::RemoteOperation<Signature>(this, name));
            }
#endif
            log(Warning) << "No such operation: "<< name <<endlog();
            return boost::shared_ptr<base::DisposableInterface>();
        }

        /**
         * Removes a previously added operation.
         */
        void removeOperation( const std::string& name );

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
            if ( this->addLocalOperation( op ) == false )
                return op;
            this->add( op.getName(), new internal::OperationRepositoryPartFused<Signature>( &op ) );
            return op;
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
            typedef typename GetSignature<Func>::Signature Signature;
            Operation<Signature>* op = new Operation<Signature>(name);
            op->calls(func, serv, et);
            if ( this->addLocalOperation( *op ) == false ) {
                assert(false);
                return *op; // should never be reached.
            }
            ownedoperations.push_back(op);
            this->add( op->getName(), new internal::OperationRepositoryPartFused<Signature>( op ) );

            return *op;
        }

        /**
         * Returns a function signature from a C++ member function
         * suitable for DS operations
         */
        template<class FunctionT>
        struct GetSignatureDS {
            typedef typename internal::ArgMember< typename boost::remove_pointer<FunctionT>::type >::type Signature;
        };

        /**
         * For internal use only. The pointer of the object of which a member function
         * must be invoked is stored in a internal::DataSource such that the pointer can change
         * during program execution. Required in scripting for state machines.
         */
        template<class Func,class ObjT>
        Operation< typename GetSignatureDS<Func>::Signature>& addOperationDS( const std::string& name, Func func, internal::DataSource< boost::shared_ptr<ObjT> >* sp,
                ExecutionThread et = ClientThread)
        {
            typedef typename GetSignatureDS<Func>::Signature SignatureDS;    // function signature with normal object pointer
            Operation<SignatureDS>* op = new Operation<SignatureDS>(name);
            op->calls(func);
            if ( this->addLocalOperation( *op ) == false ) {
                assert(false);
                return *op; // should never be reached.
            }
            ownedoperations.push_back(op);
            //typedef typename internal::incomplete<ObjT>::x x;
            //typedef typename internal::incomplete<SignatureDS>::y y;
            this->add( op->getName(), new internal::OperationRepositoryPartFusedDS<SignatureDS,ObjT>( sp, op) );

            return *op;
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
        base::DataSourceBase* getOperation( std::string name,
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
        internal::MethodC create(std::string name, ExecutionEngine* caller) {
            return internal::MethodC( this, name, caller );
        }

        /**
         * Reset the implementation of a operation.
         */
        bool resetOperation(std::string name, base::OperationBase* impl)
        {
            if (!hasOperation(name))
                return false;
            simpleoperations[name] = impl;
            return true;
        }
    protected:
        typedef std::map< std::string, interface::ServiceProvider* > Services;
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
        ServiceProvider* parent;
    };
}}


#endif
