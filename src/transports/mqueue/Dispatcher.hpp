/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  Dispatcher.hpp

                        Dispatcher.hpp -  description
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



#include "../../os/MutexLock.hpp"
#include "../../Activity.hpp"
#include "../../base/ChannelElementBase.hpp"
#include "../../Logger.hpp"
#include <map>
#include <sys/select.h>
#include <mqueue.h>



namespace RTT {
    namespace mqueue {
        /**
         * This object waits on a set of open message queue
         * file descriptors and signals the channel that has
         * received new data.
         * Reasonably, there should be one dispatcher for each
         * peer component sending us data.
         */
        class Dispatcher : public Activity
        {
            static Dispatcher* DispatchI;

            typedef std::map<mqd_t,base::ChannelElementBase*> MQMap;
            MQMap mqmap;

            fd_set socks;        /* Socket file descriptors we want to wake up for, using select() */

            int highsock;        /* Highest #'d file descriptor, needed for select() */

            bool do_exit;

            os::Mutex maplock;

            Dispatcher( const std::string& name)
            : Activity(ORO_SCHED_RT, os::HighestPriority, 0.0, 0, name),
              highsock(0), do_exit(false)
              {}

            void build_select_list() {

                /* First put together fd_set for select(), which will
                   consist of the sock veriable in case a new connection
                   is coming in, plus all the sockets we have already
                   accepted. */


                /* FD_ZERO() clears out the fd_set called socks, so that
                    it doesn't contain any file descriptors. */

                FD_ZERO(&socks);
                highsock = 0;

                /* Loops through all the possible connections and adds
                    those sockets to the fd_set */
                os::MutexLock lock(maplock);
                for (MQMap::const_iterator it = mqmap.begin(); it != mqmap.end(); ++it) {
                    FD_SET( it->first, &socks);
                    if ( int(it->first) > highsock)
                        highsock = int(it->first);
                }
            }

            void read_socks() {
                /* OK, now socks will be set with whatever socket(s)
                   are ready for reading.*/

                /* Run through our sockets and check to see if anything
                    happened with them, if so 'service' them. */
                os::MutexLock lock(maplock);
                for (MQMap::iterator it = mqmap.begin(); it != mqmap.end(); ++it) {
                    if ( FD_ISSET( it->first, &socks) ) {
                        //log(Debug) << "New data on " << it->first <<endlog();
                        it->second->signal();
                    }
                }
            }

        public:
            static Dispatcher* Instance() {
                if (DispatchI == 0) {
                    DispatchI = new Dispatcher("MQueueDispatch");
                    DispatchI->start();
                }
                return DispatchI;
            }

            void addQueue( mqd_t mqdes, base::ChannelElementBase* chan ) {
                if (mqdes < 0) {
                    log(Error) <<"Invalid mqd_t given to MQueue Dispatcher." <<endlog();
                    return;
                }
                log(Debug) <<"Dispatcher is monitoring mqdes "<< mqdes <<endlog();
                os::MutexLock lock(maplock);
                mqmap[mqdes] = chan;
            }

            void removeQueue(mqd_t mqdes) {
                log(Debug) <<"Dispatcher drops mqdes "<< mqdes <<endlog();
                os::MutexLock lock(maplock);
                if (mqmap.count(mqdes))
                    mqmap.erase( mqmap.find(mqdes) );
            }

            bool initialize() {
                do_exit = false;
                return true;
            }

            void loop() {
                struct timeval timeout;  /* Timeout for select */
                int readsocks;       /* Number of sockets ready for reading */
                while (1) { /* select loop */
                    build_select_list();
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 50000;

                    /* The first argument to select is the highest file
                        descriptor value plus 1.*/

                    readsocks = select(highsock+1, &socks, (fd_set *) 0,
                      (fd_set *) 0, &timeout);

                    /* select() returns the number of sockets that had
                        things going on with them -- i.e. they're readable. */

                    /* Once select() returns, the original fd_set has been
                        modified so it now reflects the state of why select()
                        woke up. i.e. If file descriptor 4 was originally in
                        the fd_set, and then it became readable, the fd_set
                        contains file descriptor 4 in it. */

                    if (readsocks < 0) {
                        log(Error) <<"Dispatcher failed to select on message queues. Stopped thread."<<endlog();
                        return;
                    }
                    if (readsocks == 0) {
                        // nop
                    } else // readsocks > 0
                        read_socks();

                    if ( do_exit )
                        return;
                } /* while(1) */
            }

            bool breakLoop() {
                do_exit = true;
                return true;
            }
        };
    }
}
