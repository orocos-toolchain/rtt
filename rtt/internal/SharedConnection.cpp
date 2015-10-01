/***************************************************************************
  tag: Intermodalics   Thu Jul 30 18:28:12 CEST 2015  SharedConnection.cpp

                   SharedConnection.cpp -  description
                           -------------------
    begin                : Thu July 30 2015
    copyright            : (C) 2015 Intermodalics
    email                : johannes@intermodalics.eu

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

#include "SharedConnection.hpp"
#include "../os/MutexLock.hpp"
#include "../Logger.hpp"

#include <cstdlib>
#include <ctime>

using namespace RTT;
using namespace RTT::base;
using namespace RTT::internal;

bool SharedConnID::isSameID(ConnID const& id) const
{
    SharedConnID const* real_id = dynamic_cast<SharedConnID const*>(&id);
    if (!real_id)
        return false;
    else return real_id->connection == this->connection;
}

ConnID* SharedConnID::clone() const
{
    return new SharedConnID(this->connection);
}

static std::string generateRandomName(std::size_t length = 6)
{
    std::string name;
    static const char alphanum[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";

    name.resize(length);
    std::srand(std::time(0));
    for (std::string::iterator s = name.begin(); s != name.end(); ++s) {
        *s = alphanum[std::rand() % (sizeof(alphanum) - 1)];
    }

    return name;
}

SharedConnectionBase::SharedConnectionBase(const ConnPolicy &policy)
    : policy(policy)
{
    // assign random name if none was given
    if (this->policy.name_id.empty()) {
        this->policy.name_id = generateRandomName();
    }

    // register at SharedConnectionRepository
    SharedConnectionRepository::Instance()->add(getName(), this);

    log(Debug) << "Constructed shared " << (policy.type == ConnPolicy::DATA ? "data" : (policy.type == ConnPolicy::BUFFER ? "buffer" : "")) << " connection '" << getName() << "'." << endlog();
}

SharedConnectionBase::~SharedConnectionBase()
{
    // unregister this connection at the SharedConnectionRepository
    SharedConnectionRepository::Instance()->remove(this);

    log(Debug) << "Destroyed shared connection '" << getName() << "'." << endlog();
}

SharedConnID *SharedConnectionBase::getConnID()
{
    return new SharedConnID(this);
}

const std::string &SharedConnectionBase::getName() const
{
    return policy.name_id;
}

const ConnPolicy *SharedConnectionBase::getConnPolicy() const
{
    return &policy;
}

static SharedConnectionRepository::shared_ptr the_instance;
SharedConnectionRepository::shared_ptr SharedConnectionRepository::Instance()
{
    if (!the_instance) {
        the_instance.reset(new SharedConnectionRepository());
    }
    return the_instance;
}

bool SharedConnectionRepository::add(const key_t &key, SharedConnectionBase* connection)
{
    RTT::os::MutexLock lock(mutex);
    if (map.count(key)) return false;
    map[key] = connection;
    return true;
}

void SharedConnectionRepository::remove(SharedConnectionBase* connection)
{
    RTT::os::MutexLock lock(mutex);
    for(Map::iterator it = map.begin(); it != map.end(); )
    {
        if (it->second == connection) {
            Map::iterator copy = it++;
            map.erase(copy);
        } else {
            it++;
        }
    }
}

bool SharedConnectionRepository::has(const key_t &key) const
{
    RTT::os::SharedMutexLock lock(mutex);
    return map.count(key);
}

SharedConnectionBase::shared_ptr SharedConnectionRepository::get(const key_t &key) const
{
    RTT::os::SharedMutexLock lock(mutex);
    if (!map.count(key)) return SharedConnectionBase::shared_ptr();
    return map.at(key);
}
