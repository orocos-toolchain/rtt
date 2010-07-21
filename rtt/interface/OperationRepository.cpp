#include "OperationRepository.hpp"
#include "FactoryExceptions.hpp"
#include <algorithm>
#include "../internal/mystd.hpp"
#include "../internal/Exceptions.hpp"
#include "../Handle.hpp"

using namespace RTT;
using namespace RTT::detail;

OperationRepositoryPart::~OperationRepositoryPart()
{
}

boost::shared_ptr<base::DisposableInterface> OperationRepositoryPart::getLocalOperation() const
{
    return boost::shared_ptr<base::DisposableInterface>();
}

void OperationRepository::clear()
{
    for (map_t::iterator i = data.begin(); i != data.end(); ++i)
        delete i->second;
    data.clear();
}

std::vector<std::string> OperationRepository::getNames() const
{
    std::vector<std::string> ret;
    std::transform(data.begin(), data.end(), std::back_inserter(ret), select1st<map_t::value_type> ());
    return ret;
}

bool OperationRepository::hasMember(const std::string& name) const
{
    return data.find(name) != data.end();
}

int OperationRepository::getArity(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        return -1;
    return i->second->arity();
}

int OperationRepository::getCollectArity(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        return -1;
    return i->second->collectArity();
}

bool OperationRepository::isSynchronous(const std::string& name) const
{
    if (!hasMember(name))
        return false;
    try {
        produceHandle(name);
    } catch(...) {
        return true;
    }
    return false;
}

base::DataSourceBase::shared_ptr OperationRepository::produce(const std::string& name, const Arguments& args, ExecutionEngine* caller) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(interface::name_not_found_exception(), 0);
    return i->second->produce(args, caller);
}

base::DataSourceBase::shared_ptr OperationRepository::produceSend(const std::string& name, const Arguments& args, ExecutionEngine* caller) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(interface::name_not_found_exception(), 0);
    return i->second->produceSend(args, caller);
}

base::DataSourceBase::shared_ptr OperationRepository::produceHandle(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(interface::name_not_found_exception(), 0);
    return i->second->produceHandle();
}

base::DataSourceBase::shared_ptr OperationRepository::produceCollect(const std::string& name, const Arguments& args, DataSource<bool>::shared_ptr blocking) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(interface::name_not_found_exception(), 0);
    return i->second->produceCollect(args, blocking);
}

Handle OperationRepository::produceSignal(const std::string& name, base::ActionInterface* act, const Arguments& args) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(interface::name_not_found_exception(), 0);
    return i->second->produceSignal(act, args);
}

OperationRepository::Descriptions OperationRepository::getArgumentList(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(interface::name_not_found_exception(), Descriptions());
    return i->second->getArgumentList();
}

std::string OperationRepository::getResultType(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(interface::name_not_found_exception(), std::string());
    return i->second->resultType();
}

std::string OperationRepository::getDescription(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(interface::name_not_found_exception(), std::string());
    return i->second->description();
}

void OperationRepository::add(const std::string& name, OperationRepositoryPart* part)
{
    map_t::iterator i = data.find(name);
    if (i != data.end())
        delete i->second;
    data[name] = part;
}

void OperationRepository::remove(const std::string& name)
{
    map_t::iterator i = data.find(name);
    if (i != data.end())
    {
        delete i->second;
        data.erase(i);
    }
}

OperationRepositoryPart* OperationRepository::getPart(const std::string& name)
{
    map_t::iterator i = data.find(name);
    if (i != data.end())
    {
        return i->second;
    }
    return 0;
}

