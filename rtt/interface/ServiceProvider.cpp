#include "ServiceProvider.hpp"

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

    bool ServiceProvider::addLocalOperation( OperationBase& op )
    {
        Logger::In in("ServiceProvider");
        if ( simpleoperations.count( op.getName() ) ) {
            log(Error) << "Failed to addOperation: '"<< op.getName() <<"' already added." <<endlog();
            return false;
        }
        if ( op.getName().empty() || !op.ready() ) {
            log(Error) << "Failed to addOperation: '"<< op.getName() <<"' was not ready() or has no name." <<endlog();
            return false;
        }
        simpleoperations[op.getName()] = &op; //.getImplementation();
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
        simpleoperations.erase( name );
        OperationFactory::remove(name);
    }

}
