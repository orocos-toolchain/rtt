/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  CorbaDispatcher.cpp

                        CorbaDispatcher.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#include "CorbaDispatcher.hpp"
#include <boost/lexical_cast.hpp>

using namespace RTT;
using namespace RTT::corba;

CorbaDispatcher::DispatchMap CorbaDispatcher::DispatchI;
RTT_CORBA_API os::Mutex* CorbaDispatcher::mlock = 0;

int CorbaDispatcher::defaultScheduler = ORO_SCHED_RT;
int CorbaDispatcher::defaultPriority  = os::LowestPriority;

CorbaDispatcher::DispatchEntry& CorbaDispatcher::Get(std::string const& name, int scheduler, int priority)
{
    DispatchMap::iterator result = DispatchI.find(name);
    if ( result != DispatchI.end() )
        return result->second;

    CorbaDispatcher* dispatcher = new CorbaDispatcher( name, scheduler, priority );
    dispatcher->start();
    return (DispatchI[name] = DispatchEntry(dispatcher));
}

CorbaDispatcher* CorbaDispatcher::Instance(DataFlowInterface* iface, int scheduler, int priority) {
    return Instance(defaultDispatcherName(iface), scheduler, priority);
}

std::string CorbaDispatcher::defaultDispatcherName(DataFlowInterface* iface)
{
    std::string name;
    if ( iface == 0 || iface->getOwner() == 0)
        name = "Global";
    else
        name = iface->getOwner()->getName();
    return name + ".CorbaDispatch." + boost::lexical_cast<std::string>(reinterpret_cast<uint64_t>(iface));
}

/**
 * Create a new dispatcher and registers it under a certain name
 *
 * @param name the dispatcher registration name
 * @return
 */
CorbaDispatcher* CorbaDispatcher::Instance(std::string const& name, int scheduler, int priority) {
    if (!mlock) mlock = new os::Mutex();
    os::MutexLock lock(*mlock);

    return Get(name, scheduler, priority).dispatcher;
}

CorbaDispatcher* CorbaDispatcher::Acquire(RTT::DataFlowInterface* interface, int scheduler, int priority) {
    return Acquire(defaultDispatcherName(interface), scheduler, priority);
}

CorbaDispatcher* CorbaDispatcher::Acquire(std::string const& name, int scheduler, int priority) {
    if (!mlock) mlock = new os::Mutex();
    os::MutexLock lock(*mlock);

    DispatchEntry& entry = Get(name, scheduler, priority);
    entry.refcount.inc();
    return entry.dispatcher;
}

void CorbaDispatcher::Release(CorbaDispatcher* dispatcher) {
    return Release(dispatcher->getName());
}

void CorbaDispatcher::Release(std::string const& name) {
    if (!mlock) return;
    os::MutexLock lock(*mlock);

    DispatchMap::iterator result = DispatchI.find(name);
    if ( result != DispatchI.end() ) {
        if (result->second.refcount.dec_and_test())
        {
            delete result->second.dispatcher;
            DispatchI.erase(result);
        }
    }
}

void CorbaDispatcher::hasElement(base::ChannelElementBase::shared_ptr c0, base::ChannelElementBase::shared_ptr c1, bool& result)
{
    result = result || (c0 == c1);
}

void CorbaDispatcher::dispatchChannel( base::ChannelElementBase::shared_ptr chan ) {
    bool has_element = false;
    RClist.apply(boost::bind(&CorbaDispatcher::hasElement, _1, chan, boost::ref(has_element)));
    if (!has_element)
        RClist.append( chan );
    this->trigger();
}

void CorbaDispatcher::cancelChannel( base::ChannelElementBase::shared_ptr chan ) {
    RClist.erase( chan );
}

bool CorbaDispatcher::initialize() {
    log(Info) <<"Started " << this->getName() << "." <<endlog();
    do_exit = false;
    return true;
}

void CorbaDispatcher::loop() {
    while ( !RClist.empty() && !do_exit) {
        base::ChannelElementBase::shared_ptr chan = RClist.front();
        CRemoteChannelElement_i* rbase = dynamic_cast<CRemoteChannelElement_i*>(chan.get());
        if (rbase)
            rbase->transferSamples();
        RClist.erase( chan );
    }
}

bool CorbaDispatcher::breakLoop() {
    do_exit = true;
    return true;
}
