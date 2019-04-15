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
            struct DispatchEntry {
                os::AtomicInt refcount;
                CorbaDispatcher* dispatcher;

                DispatchEntry()
                    : dispatcher(0) {}
                explicit DispatchEntry(CorbaDispatcher* dispatcher)
                    : dispatcher(dispatcher) {}
            };
            typedef std::map<std::string, DispatchEntry> DispatchMap;
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

            /** Internal access and auto-creation of dispatch entries
             *
             * It is a helper method, and does NOT acquire the locking mutex.
             * Callers MUST acquire it before calling
             */
            static DispatchEntry& Get(std::string const& name, int scheduler = defaultScheduler, int priority = defaultPriority);

        public:
            static std::string defaultDispatcherName(DataFlowInterface* iface);

            static CorbaDispatcher* Instance(DataFlowInterface* iface, int scheduler = defaultScheduler, int priority = defaultPriority);

            /**
             * Create a new dispatcher and registers it under a certain name
             *
             * @param name the dispatcher registration name
             * @return
             */
            static CorbaDispatcher* Instance(std::string const& name, int scheduler = defaultScheduler, int priority = defaultPriority);

            static CorbaDispatcher* Acquire(DataFlowInterface* interface, int scheduler = defaultScheduler, int priority = defaultPriority);

            static CorbaDispatcher* Acquire(std::string const& name, int scheduler = defaultScheduler, int priority = defaultPriority);

            static void Release(std::string const& name);
            static void Release(CorbaDispatcher* dispatcher);

	    static void hasElement(base::ChannelElementBase::shared_ptr c0, base::ChannelElementBase::shared_ptr c1, bool& result);

            void dispatchChannel( base::ChannelElementBase::shared_ptr chan );

            void cancelChannel( base::ChannelElementBase::shared_ptr chan );

            bool initialize();

            void loop();
            bool breakLoop();
        };
    }
}
#endif
