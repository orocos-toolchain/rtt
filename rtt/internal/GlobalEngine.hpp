/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  GlobalEngine.hpp

                        GlobalEngine.hpp -  description
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


#ifndef ORO_GLOBALENGINE_HPP_
#define ORO_GLOBALENGINE_HPP_

#include "rtt/ExecutionEngine.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{

    namespace internal
    {

        /**
         * A process-wide execution engine that processes every
         * asynchronous operation that is not being processed by
         * a TaskContext.
         *
         * In practice, this only occurs when a client does a 
         * send() on an OperationCaller which calls an Operation
         * with the ClientThread policy. 
         */
        class GlobalEngine: public RTT::ExecutionEngine
        {
            boost::shared_ptr<base::ActivityInterface> mact;
            GlobalEngine(int scheduler, int priority, unsigned cpu_affinity);
            virtual ~GlobalEngine();
        public:
            /** @overload
             * This is kept (instead of using default parameters) for ABI
             * compatibility
             */
            RTT_API static ExecutionEngine* Instance() { return Instance(ORO_SCHED_OTHER); }
            RTT_API static ExecutionEngine* Instance(int scheduler, int priority = os::LowestPriority, unsigned cpu_affinity = 0);
            RTT_API static void Release();
        };

    }

}

#endif /* ORO_GLOBALENGINE_HPP_ */
