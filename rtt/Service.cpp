/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  Service.cpp

                        Service.cpp -  description
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


#include "Service.hpp"
#include "TaskContext.hpp"
#include <algorithm>
#include "internal/mystd.hpp"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <algorithm>

namespace RTT {
    using namespace detail;
    using namespace std;
    using namespace boost;

    Service::shared_ptr Service::Create(const std::string& name, TaskContext* owner) {
        return shared_ptr(new Service(name,owner));
    }

    Service::Service(const std::string& name, TaskContext* owner)
    : mname(name), mowner(owner), parent()
    {
        // Inform DataFlowInterface.
        mservice = this;
    }

    Service::~Service()
    {
        clear();
    }

    vector<string> Service::getProviderNames() const {
        return keys(services);
    }

    bool Service::addService( Service::shared_ptr obj ) {
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

    void Service::removeService( string const& name) {
        // carefully written to avoid destructor to call back on us when called from removeService.
        if ( services.count(name) ) {
            shared_ptr sp = services.find(name)->second;
            services.erase(name);
            sp.reset(); // this possibly deletes.
        }
    }

    Service::shared_ptr Service::provides(const std::string& service_name) {
        if (service_name == "this")
            return shared_from_this();
        shared_ptr sp = services[service_name];
        if (sp)
            return sp;
        sp = boost::make_shared<Service>(service_name, mowner);
        sp->setParent( shared_from_this() );
        services[service_name] = sp;
        return sp;
    }

    Service::shared_ptr Service::getService(const std::string& service_name) {
        Services::iterator it = services.find(service_name);
        if (it != services.end() )
            return it->second;
        return shared_ptr();
    }

    OperationInterfacePart* Service::getOperation( std::string name )
    {
        Logger::In in("Service::getOperation");
        if ( this->hasMember(name ) ) {
            return this->getPart(name);
        }
        log(Warning) << "No such operation in service '"<< getName() <<"': "<< name <<endlog();
        return 0;
    }

    bool Service::resetOperation(std::string name, base::OperationBase* impl)
    {
        if (!hasOperation(name))
            return false;
        simpleoperations[name] = impl;
        return true;
    }

    bool Service::hasService(const std::string& service_name) {
        if (service_name == "this")
            return true;
        return services.find(service_name) != services.end();
    }

    bool Service::addLocalOperation( OperationBase& op )
    {
        Logger::In in("Service::addLocalOperation");
        if ( op.getName().empty() ) {
            log(Error) << "Failed to add Operation: '"<< op.getName() <<"' has no name." <<endlog();
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

    boost::shared_ptr<base::DisposableInterface> Service::getLocalOperation( std::string name ) {
        if ( hasOperation(name) ) {
            return simpleoperations.find(name)->second->getImplementation();
        }
        return boost::shared_ptr<base::DisposableInterface>();
    }

    void Service::clear()
    {
        while ( !simpleoperations.empty() )
        {
            simpleoperations.erase(simpleoperations.begin() );
        }

        for_each(ownedoperations.begin(),ownedoperations.end(), lambda::delete_ptr() );
        ownedoperations.clear();

        OperationInterface::clear();
        while ( !services.empty() ) {
        	this->removeService( services.begin()->first );
        }
    }

    std::vector<std::string> Service::getOperationNames() const
    {
        return keys(simpleoperations);
        //return getNames();
    }

    bool Service::hasOperation(const std::string& name) const
    {
        return simpleoperations.count(name) == 1;
        //return hasMember(name);
    }

    void Service::removeOperation(const std::string& name)
    {
        if (!hasOperation(name))
            return;
        OperationList::iterator it = find(ownedoperations.begin(), ownedoperations.end(), simpleoperations.find(name)->second );
        if (it != ownedoperations.end()) {
            delete *it;
            ownedoperations.erase(it);
        }
        simpleoperations.erase( name );
        OperationInterface::remove(name);
    }
    void Service::setOwner(TaskContext* new_owner) {
        for( SimpleOperations::iterator it= simpleoperations.begin(); it != simpleoperations.end(); ++it)
            it->second->setOwner( new_owner ? new_owner->engine() : 0);

        this->mowner = new_owner;

        for( Services::iterator it= services.begin(); it != services.end(); ++it)
            it->second->setOwner( new_owner );
    }

    void Service::setParent( Service::shared_ptr p) {
        parent = p;
    }

    internal::OperationCallerC Service::create(std::string name, ExecutionEngine* caller) {
        return internal::OperationCallerC( getPart(name), name, caller );
    }

}
