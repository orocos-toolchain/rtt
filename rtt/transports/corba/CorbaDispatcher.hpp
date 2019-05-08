/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  CorbaDispatcher.hpp

                        CorbaDispatcher.hpp -  description
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


#ifndef ORO_CORBA_DISPATCHER_HPP
#define ORO_CORBA_DISPATCHER_HPP

#include "../../os/MutexLock.hpp"
#include "../../Activity.hpp"
#include "../../base/ChannelElementBase.hpp"
#include "../../Logger.hpp"
#include "../../internal/List.hpp"
#include "DataFlowI.h"
#include "../../DataFlowInterface.hpp"
#include "../../TaskContext.hpp"

namespace RTT {
    namespace corba {
        /**
         * This object sends over data flow messages
         * from local buffers to a remote channel element.
         */
        class CorbaDispatcher : public Activity
        {
            typedef std::map<DataFlowInterface*,CorbaDispatcher*> DispatchMap;
            RTT_CORBA_API static DispatchMap DispatchI;

            typedef internal::List<base::ChannelElementBase::shared_ptr> RCList;
            RCList RClist;

            bool do_exit;

            RTT_CORBA_API static os::Mutex* mlock;

            RTT_CORBA_API static int defaultScheduler;
            RTT_CORBA_API static int defaultPriority;

            CorbaDispatcher( const std::string& name)
            : Activity(defaultScheduler, defaultPriority, 0.0, 0, name),
              RClist(20,2),
              do_exit(false)
              {}

            CorbaDispatcher( const std::string& name, int scheduler, int priority)
            : Activity(scheduler, priority, 0.0, 0, name),
              RClist(20,2),
              do_exit(false)
              {}

            ~CorbaDispatcher() {
                this->stop();
            }

        public:
            /**
             * Create a new dispatcher for a given data flow interface.
             * This method will only lock and allocate when a new dispatcher must be created,
             * otherwise, the access is lock-free and real-time.
             * One dispatcher per \a iface is created.
             * @param iface The interface to dispatch data flow messages for.
             * @return
             */
            static CorbaDispatcher* Instance(DataFlowInterface* iface, int scheduler = defaultScheduler, int priority = defaultPriority) {
                if (!mlock)
                    mlock = new os::Mutex();
                DispatchMap::iterator result = DispatchI.find(iface);
                if ( result == DispatchI.end() ) {
                    os::MutexLock lock(*mlock);
                    // re-try to find (avoid race):
                    result = DispatchI.find(iface);
                    if ( result != DispatchI.end() )
                        return result->second;
                    // *really* not found, let's create it.
                    std::string name;
                    if ( iface == 0 || iface->getOwner() == 0)
                        name = "Global";
                    else
                        name = iface->getOwner()->getName();
                    name += "Corba";
                    DispatchI[iface] = new CorbaDispatcher( name, scheduler, priority );
                    DispatchI[iface]->start();
                    return DispatchI[iface];
                }
                return result->second;
            }

            /**
             * Releases and cleans up a specific interface from dispatching.
             * @param iface
             */
            static void Release(DataFlowInterface* iface) {
                DispatchMap::iterator result = DispatchI.find(iface);
                if ( result != DispatchI.end() ) {
                    os::MutexLock lock(*mlock);
                    delete result->second;
                    DispatchI.erase(result);
                }
                if ( DispatchI.empty() )
                    delete mlock;
                mlock = 0;
            }

            /**
             * May be called during program termination to clean up all resources.
             */
            static void ReleaseAll() {
                DispatchMap::iterator result = DispatchI.begin();
                while ( result != DispatchI.end() ) {
                    delete result->second;
                    DispatchI.erase(result);
                    result = DispatchI.begin();
                }
                delete mlock;
                mlock = 0;
            }

            static void hasElement(base::ChannelElementBase::shared_ptr c0, base::ChannelElementBase::shared_ptr c1, bool& result)
            {
                result = result || (c0 == c1);
            }

            void dispatchChannel( base::ChannelElementBase::shared_ptr chan ) {
                bool has_element = false;
                RClist.apply(boost::bind(&CorbaDispatcher::hasElement, _1, chan, boost::ref(has_element)));
                if (!has_element)
                    RClist.append( chan );
                this->trigger();
            }

            void cancelChannel( base::ChannelElementBase::shared_ptr chan ) {
                RClist.erase( chan );
            }

            bool initialize() {
                log(Info) <<"Started " << this->getName() << "." <<endlog();
                do_exit = false;
                return true;
            }

            void loop() {
                while ( !RClist.empty() && !do_exit) {
                    base::ChannelElementBase::shared_ptr chan = RClist.front();
                    CRemoteChannelElement_i* rbase = dynamic_cast<CRemoteChannelElement_i*>(chan.get());
                    if (rbase)
                        rbase->transferSamples();
                    RClist.erase( chan );
                }
            }

            bool breakLoop() {
                do_exit = true;
                return true;
            }
        };
    }
}
#endif
