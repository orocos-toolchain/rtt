#include "ServiceProvider.hpp"
#include "../TaskContext.hpp"
#include <algorithm>

namespace RTT {
    using namespace detail;

    ServiceProvider::ServiceProvider(const std::string& name, TaskContext* owner)
    : mname(name), mowner(owner)
    {
    }

    ServiceProvider::~ServiceProvider()
    {
        clear();
    }

    bool ServiceProvider::addService( ServiceProvider* obj ) {
        if ( services.find( obj->getName() ) != services.end() ) {
            log(Error) << "Could not add Service " << obj->getName() <<": name already in use." <<endlog();
            return false;
        }
        obj->setOwner( mowner );
        services[obj->getName()] = obj;
        return true;
    }

    void ServiceProvider::removeService( string const& name) {
        // carefully written to avoid destructor to call back on us.
        if ( services.count(name) ) {
            ServiceProvider* sp = services.find(name)->second;
            services.erase(name);
            delete sp;
        }
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

    void ServiceProvider::clear()
    {
        while ( !simpleoperations.empty() )
        {
            simpleoperations.erase(simpleoperations.begin() );
        }
        OperationFactory::clear();
    }

    std::vector<std::string> ServiceProvider::getOperations() const
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
        OperationFactory::remove(name);
    }
    void ServiceProvider::setOwner(TaskContext* new_owner) {
        for( SimpleOperations::iterator it= simpleoperations.begin(); it != simpleoperations.end(); ++it)
            it->second->setOwner( new_owner ? new_owner->engine() : 0);
        mowner = new_owner;

        for( Services::iterator it= services.begin(); it != services.end(); ++it)
            it->second->setOwner( new_owner );
}

}
