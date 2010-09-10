/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  OperationInterface.cpp

                        OperationInterface.cpp -  description
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


#include "OperationInterface.hpp"
#include "FactoryExceptions.hpp"
#include <algorithm>
#include "internal/mystd.hpp"
#include "internal/Exceptions.hpp"
#include "Handle.hpp"

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

