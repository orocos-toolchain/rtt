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
        mmethods.insert(make_pair<std::string, OperationCallerBaseInvoker*> (isb.getName(), &isb));
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

    OperationCallerBaseInvoker& ServiceRequester::getOperationCaller(const std::string& name)
    {
        return *mmethods.find(name)->second;
    }

    bool ServiceRequester::connectTo( Service::shared_ptr sp) {
        for (OperationCallers::iterator it = mmethods.begin(); it != mmethods.end(); ++it) {
            if ( !it->second->ready() ) {
                if (sp->hasOperation( it->first )) {
                    it->second->setImplementation( sp->getLocalOperation( it->first ), mrowner ? mrowner->engine() : 0 );
                    if ( it->second->ready() ) {
                        if (mrowner)
                            log(Debug) << "Successfully set up OperationCaller " << it->first <<endlog();
                        else
                            log(Warning) << "OperationCaller "<< it->first << " has no caller set."<<endlog();
                    }
                }
                if (sp->hasMember( it->first )) {
                    it->second->setImplementationPart( sp->getOperation( it->first ), mrowner ? mrowner->engine() : 0 );
                    if ( it->second->ready() ) {
                        if (mrowner)
                            log(Debug) << "Successfully set up OperationCaller " << it->first <<endlog();
                        else
                            log(Warning) << "OperationCaller "<< it->first << " has no caller set."<<endlog();
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
        ExecutionEngine* ee(0);
        for_each(mmethods.begin(), mmethods.end(),
                 boost::bind(&OperationCallerBaseInvoker::setImplementation, boost::bind(&OperationCallers::value_type::second, _1), boost::shared_ptr<base::DisposableInterface>(), ee )
                 );
    }

    bool ServiceRequester::ready() const
    {
        for (OperationCallers::const_iterator it = mmethods.begin(); it != mmethods.end(); ++it)
            if ( !it->second->ready() ) {
                log(Debug) << "ServiceRequeste: "<< it->first << " not set up." <<endlog();
                return false;
            }
        return true;
    }
}
