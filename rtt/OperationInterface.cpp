#include "OperationInterface.hpp"
#include "FactoryExceptions.hpp"
#include <algorithm>
#include "../internal/mystd.hpp"
#include "../internal/Exceptions.hpp"
#include "../Handle.hpp"

using namespace RTT;
using namespace RTT::detail;

OperationInterfacePart::~OperationInterfacePart()
{
}

boost::shared_ptr<base::DisposableInterface> OperationInterfacePart::getLocalOperation() const
{
    return boost::shared_ptr<base::DisposableInterface>();
}

void OperationInterface::clear()
{
    for (map_t::iterator i = data.begin(); i != data.end(); ++i)
        delete i->second;
    data.clear();
}

std::vector<std::string> OperationInterface::getNames() const
{
    std::vector<std::string> ret;
    std::transform(data.begin(), data.end(), std::back_inserter(ret), select1st<map_t::value_type> ());
    return ret;
}

bool OperationInterface::hasMember(const std::string& name) const
{
    return data.find(name) != data.end();
}

int OperationInterface::getArity(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        return -1;
    return i->second->arity();
}

int OperationInterface::getCollectArity(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        return -1;
    return i->second->collectArity();
}

bool OperationInterface::isSynchronous(const std::string& name) const
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

base::DataSourceBase::shared_ptr OperationInterface::produce(const std::string& name, const Arguments& args, ExecutionEngine* caller) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
    return i->second->produce(args, caller);
}

base::DataSourceBase::shared_ptr OperationInterface::produceSend(const std::string& name, const Arguments& args, ExecutionEngine* caller) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
    return i->second->produceSend(args, caller);
}

base::DataSourceBase::shared_ptr OperationInterface::produceHandle(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
    return i->second->produceHandle();
}

base::DataSourceBase::shared_ptr OperationInterface::produceCollect(const std::string& name, const Arguments& args, DataSource<bool>::shared_ptr blocking) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
    return i->second->produceCollect(args, blocking);
}

Handle OperationInterface::produceSignal(const std::string& name, base::ActionInterface* act, const Arguments& args) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
    return i->second->produceSignal(act, args);
}

OperationInterface::Descriptions OperationInterface::getArgumentList(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(name_not_found_exception(), Descriptions());
    return i->second->getArgumentList();
}

std::string OperationInterface::getResultType(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(name_not_found_exception(), std::string());
    return i->second->resultType();
}

std::string OperationInterface::getDescription(const std::string& name) const
{
    map_t::const_iterator i = data.find(name);
    if (i == data.end() || i->second == 0)
        ORO_THROW_OR_RETURN(name_not_found_exception(), std::string());
    return i->second->description();
}

void OperationInterface::add(const std::string& name, OperationInterfacePart* part)
{
    map_t::iterator i = data.find(name);
    if (i != data.end())
        delete i->second;
    data[name] = part;
}

void OperationInterface::remove(const std::string& name)
{
    map_t::iterator i = data.find(name);
    if (i != data.end())
    {
        delete i->second;
        data.erase(i);
    }
}

OperationInterfacePart* OperationInterface::getPart(const std::string& name)
{
    map_t::iterator i = data.find(name);
    if (i != data.end())
    {
        return i->second;
    }
    return 0;
}

