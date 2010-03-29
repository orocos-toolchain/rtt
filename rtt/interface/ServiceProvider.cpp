#include "ServiceProvider.hpp"
#include "../TaskContext.hpp"
#include <algorithm>
#include "../internal/mystd.hpp"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <algorithm>

namespace RTT {
    using namespace detail;
    using namespace std;
    using namespace boost;

    ServiceProvider::shared_ptr ServiceProvider::Create(const std::string& name, TaskContext* owner) {
        return shared_ptr(new ServiceProvider(name,owner));
    }

    ServiceProvider::ServiceProvider(const std::string& name, TaskContext* owner)
    : mname(name), mowner(owner), parent()
    {
    }

    ServiceProvider::~ServiceProvider()
    {
        clear();
    }

    vector<string> ServiceProvider::getProviderNames() const {
        return keys(services);
    }

    bool ServiceProvider::addService( ServiceProvider::shared_ptr obj ) {
        if ( services.find( obj->getName() ) != services.end() ) {
            log(Error) << "Could not add Service " << obj->getName() <<": name already in use." <<endlog();
            return false;
        }
        if ( obj->getParent() == 0) {
            obj->setOwner( mowner );
            obj->setParent( shared_from_this() );
        }
        services[obj->getName()] = obj;
        return true;
    }

    void ServiceProvider::removeService( string const& name) {
        // carefully written to avoid destructor to call back on us when called from removeService.
        if ( services.count(name) ) {
            shared_ptr sp = services.find(name)->second;
            services.erase(name);
            sp.reset(); // this possibly deletes.
        }
    }

    ServiceProvider::shared_ptr ServiceProvider::provides(const std::string& service_name) {
        if (service_name == "this")
            return shared_from_this();
        shared_ptr sp = services[service_name];
        if (sp)
            return sp;
        sp = boost::make_shared<ServiceProvider>(service_name, mowner);
        services[service_name] = sp;
        return sp;
    }

    ServiceProvider::shared_ptr ServiceProvider::getService(const std::string& service_name) {
        Services::iterator it = services.find(service_name);
        if (it != services.end() )
            return it->second;
        return shared_ptr();
    }

    bool ServiceProvider::hasService(const std::string& service_name) {
        if (service_name == "this")
            return true;
        return services.find(service_name) != services.end();
    }

    bool ServiceProvider::addLocalOperation( OperationBase& op )
    {
        Logger::In in("ServiceProvider::addLocalOperation");
        if ( op.getName().empty() || !op.getImplementation() ) {
            log(Error) << "Failed to add Operation: '"<< op.getName() <<"' has no name or no implementation." <<endlog();
            return false;
        }
        if ( simpleoperations.count( op.getName() ) ) {
            log(Warning) << "While adding Operation: '"<< op.getName() <<"': replacing previously added operation." <<endlog();
            this->removeOperation(op.getName());
        }
        simpleoperations[op.getName()] = &op;
        if (mowner)
            op.setOwner(mowner->engine());
        return true;
    }

    boost::shared_ptr<base::DisposableInterface> ServiceProvider::getLocalOperation( std::string name ) {
        if ( hasOperation(name) ) {
            return simpleoperations.find(name)->second->getImplementation();
        }
        return boost::shared_ptr<base::DisposableInterface>();
    }

    void ServiceProvider::clear()
    {
        while ( !simpleoperations.empty() )
        {
            simpleoperations.erase(simpleoperations.begin() );
        }

        for_each(ownedoperations.begin(),ownedoperations.end(), lambda::delete_ptr() );
        ownedoperations.clear();

        OperationRepository::clear();
        while ( !services.empty() ) {
            if ( services.begin()->second->getParent() == shared_from_this() )
                this->removeService( services.begin()->first );
        }
    }

    std::vector<std::string> ServiceProvider::getOperationNames() const
    {
        return keys(simpleoperations);
    }

    bool ServiceProvider::hasOperation(const std::string& name) const
    {
        return simpleoperations.count(name) == 1;
    }

    void ServiceProvider::removeOperation(const std::string& name)
    {
        if (!hasOperation(name))
            return;
        OperationList::iterator it = find(ownedoperations.begin(), ownedoperations.end(), simpleoperations.find(name)->second );
        if (it != ownedoperations.end()) {
            delete *it;
            ownedoperations.erase(it);
        }
        simpleoperations.erase( name );
        OperationRepository::remove(name);
    }
    void ServiceProvider::setOwner(TaskContext* new_owner) {
        for( SimpleOperations::iterator it= simpleoperations.begin(); it != simpleoperations.end(); ++it)
            it->second->setOwner( new_owner ? new_owner->engine() : 0);
        mowner = new_owner;

        for( Services::iterator it= services.begin(); it != services.end(); ++it)
            it->second->setOwner( new_owner );
    }

    void ServiceProvider::setParent( ServiceProvider::shared_ptr p) {
        parent = p;
    }

    internal::MethodC ServiceProvider::create(std::string name, ExecutionEngine* caller) {
        return internal::MethodC( getPart(name), name, caller );
    }

}
