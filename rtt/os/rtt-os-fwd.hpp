/***************************************************************************
  tag: Peter Soetens Fri Aug 14 20:13:44 2009 +0200 rtt-os-fwd.hpp

                        rtt-os-fwd.hpp -  description
                           -------------------
    begin                : Fri Aug 14 2009
    copyright            : (C) 2009 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_RTT_os_FWD_HPP
#define ORO_RTT_os_FWD_HPP

namespace RTT {
    namespace os {
        class AtomicInt;
        class Condition;
        class MainThread;
        class Mutex;
        class MutexInterface;
        class MutexLock;
        class MutexRecursive;
        class MutexTimedLock;
        class MutexTryLock;
        class SharedMutex;
        class SharedMutexLock;
        class Semaphore;
        class StartStopManager;
        class Thread;
        class ThreadInterface;
        class TimeService;
        class Timer;
        struct CleanupFunction;
        struct InitFunction;
    }
    namespace detail {
        using namespace os;
    }
}
#endif
