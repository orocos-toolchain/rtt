#ifndef ORO_CORBA_DISPATCHER_HPP
#define ORO_CORBA_DISPATCHER_HPP

#include "../../os/MutexLock.hpp"
#include "../../Activity.hpp"
#include "../../base/ChannelElementBase.hpp"
#include "../../Logger.hpp"
#include "../../internal/List.hpp"
#include "DataFlowI.h"

namespace RTT {
    namespace corba {
        /**
         * This object sends over data flow messages
         * from local buffers to a remote channel element.
         * @todo: how many threads will use this dispatcher ? -> preferably only 2.
         * @todo: how to determine size of channel list ? can not grow in append...
         */
        class CorbaDispatcher : public Activity
        {
            static CorbaDispatcher* DispatchI;

            typedef internal::List<base::ChannelElementBase::shared_ptr> RCList;
            RCList RClist;

            bool do_exit;

            os::Mutex listlock;

            CorbaDispatcher( const std::string& name)
            : Activity(ORO_SCHED_RT, os::LowestPriority, 0.0, 0, name),
              RClist(20,2),
              do_exit(false)
              {}

        public:
            static CorbaDispatcher* Instance() {
                if (DispatchI == 0) {
                    DispatchI = new CorbaDispatcher("CorbaDispatch");
                    DispatchI->start();
                }
                return DispatchI;
            }

            void dispatchChannel( base::ChannelElementBase::shared_ptr chan ) {
                RClist.append( chan );
                this->trigger();
            }

            void cancelChannel( base::ChannelElementBase::shared_ptr chan ) {
                RClist.erase( chan );
            }

            bool initialize() {
                log(Info) <<"Started CorbaDispatcher." <<endlog();
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
