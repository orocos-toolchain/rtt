/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  ServiceRequester.cpp

                        ServiceRequester.cpp -  description
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


#include "ServiceRequester.hpp"
#include "Service.hpp"
#include "internal/mystd.hpp"
#include "Logger.hpp"
#include "TaskContext.hpp"
#include <boost/bind.hpp>

#include <utility>

namespace RTT
{
    using namespace boost;
    using namespace detail;
    using namespace std;

    ServiceRequester::ServiceRequester(const std::string& name, TaskContext* tc) :
        mrname(name), mrowner(tc)
    {
    }

    ServiceRequester::~ServiceRequester()
    {
    }

    bool ServiceRequester::addOperationCaller(OperationCallerBaseInvoker& isb)
    {
        if (mmethods.find(isb.getName()) != mmethods.end())
        {
            log(Error) << "OperationCaller with name '" + isb.getName() + "' already present." << endlog();
            return false;
        }
        mmethods.insert(make_pair(isb.getName(), &isb));
        return true;
    }

    std::vector<std::string> ServiceRequester::getOperationCallerNames() const
    {
        return keys(mmethods);
    }

    std::vector<std::string> ServiceRequester::getRequesterNames() const
    {
        return keys(mrequests);
    }

    OperationCallerBaseInvoker* ServiceRequester::getOperationCaller(const std::string& name)
    {
        OperationCallers::iterator it = mmethods.find(name);
        if (it != mmethods.end())
            return it->second;
        return 0;
    }

    void ServiceRequester::setOwner(TaskContext *new_owner)
    {
        for(OperationCallers::iterator it = mmethods.begin(); it != mmethods.end(); ++it)
            it->second->setCaller( new_owner ? new_owner->engine() : 0);

        this->mrowner = new_owner;

        for(Requests::iterator it = mrequests.begin(); it != mrequests.end(); ++it)
            it->second->setOwner( new_owner );
    }

    ServiceRequester::shared_ptr ServiceRequester::requires() {
        try {
            return shared_from_this();
        } catch( boost::bad_weak_ptr& /*bw*/ ) {
            log(Error) <<"When using boost < 1.40.0 : You are not allowed to call requires() on a ServiceRequester that does not yet belong to a TaskContext or another ServiceRequester." << endlog();
            log(Error) <<"Try to avoid using requires() in this case: omit it or use the service requester directly." <<endlog();
            log(Error) <<"OR: upgrade to boost 1.40.0, then this error will go away." <<endlog();
            throw std::runtime_error("Illegal use of requires()");
        }
    }

    ServiceRequester::shared_ptr ServiceRequester::requires(const std::string& service_name) {
        if (service_name == "this")
            return requires();
        Requests::iterator it = mrequests.find(service_name);
        if (it != mrequests.end() )
            return it->second;
        shared_ptr sp = boost::make_shared<ServiceRequester>(service_name, mrowner);
        if ( addServiceRequester(sp) )
            return sp;
        return shared_ptr();
    }

    bool ServiceRequester::addServiceRequester(ServiceRequester::shared_ptr obj) {
        if ( mrequests.find( obj->getRequestName() ) != mrequests.end() ) {
            log(Error) << "Could not add ServiceRequester " << obj->getRequestName() <<": name already in use." <<endlog();
            return false;
        }

        // only set the owner if one is present.
        if ( mrowner ) {
            obj->setOwner( mrowner );
        }
        mrequests[obj->getRequestName()] = obj;
        return true;
    }

    bool ServiceRequester::connectTo( Service::shared_ptr sp) {
        if (!sp) return false;
        for (OperationCallers::iterator it = mmethods.begin(); it != mmethods.end(); ++it) {
            if ( !it->second->ready() ) {
                if (sp->hasOperation( it->first )) {
                    it->second->setImplementation( sp->getLocalOperation( it->first ), mrowner ? mrowner->engine() : 0 );
                    if ( it->second->ready() ) {
                        log(Debug) << "Successfully set up OperationCaller " << it->first <<endlog();
                        if (!mrowner)
                            log(Debug) << "OperationCaller "<< it->first << " has no caller set: using GlobalEngine."<<endlog();
                    }
                }
                if (sp->hasMember( it->first )) {
                    it->second->setImplementationPart( sp->getOperation( it->first ), mrowner ? mrowner->engine() : 0 );
                    if ( it->second->ready() ) {
                        log(Debug) << "Successfully set up OperationCaller " << it->first <<endlog();
                        if (!mrowner)
                            log(Debug) << "OperationCaller "<< it->first << " has no caller set: using GlobalEngine."<<endlog();
                    }
                }
            }
        }
        if (ready()) {
            if (!mprovider)
                mprovider = sp;
            log(Info) << "Found complete interface of requested service '" << mrname <<"'"<< endlog();
            return true;
        }

        return false;
    }

    void ServiceRequester::disconnect()
    {
        for_each(mmethods.begin(), mmethods.end(),
                 boost::bind(&OperationCallerBaseInvoker::disconnect, boost::bind(&OperationCallers::value_type::second, _1) )
                 );
    }

    bool ServiceRequester::ready() const
    {
        for (OperationCallers::const_iterator it = mmethods.begin(); it != mmethods.end(); ++it)
            if ( !it->second->ready() ) {
                log(Debug) << "ServiceRequester: "<< it->first << " not set up." <<endlog();
                return false;
            }
        return true;
    }

    void ServiceRequester::clear()
    {
        mmethods.clear();
        mrequests.clear();
    }
}
