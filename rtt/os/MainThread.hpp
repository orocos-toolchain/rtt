/***************************************************************************
  tag: FMTC  do nov 2 13:06:03 CET 2006  MainThread.hpp

                        MainThread.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

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



#ifndef RTT_OS_MAIN_THREAD_HPP
#define RTT_OS_MAIN_THREAD_HPP

#include "ThreadInterface.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{ namespace os {

    /**
     * A class which represents the main() thread.
     */
    class RTT_API MainThread
        : public ThreadInterface
    {
        MainThread();
        /**
         * We use a shared_ptr such that under exception
         * conditions, the program is still cleaned up.
         */
        static boost::shared_ptr<ThreadInterface> mt;
        RTOS_TASK main_task;
    public:
        virtual ~MainThread();

        /**
         * Return an object which represents the main thread.
         * Only after this call, OS calls may be done.
         */
        static ThreadInterface* Instance();

        /**
         * This is called to cleanup the main thread.
         * After this call, no OS calls may be done.
         */
        static void Release();

        /**
         * Always fails.
         */
        virtual bool start();

        /**
         * Always fails.
         */
        virtual bool stop();

        /**
         * Returns zero.
         */
        virtual Seconds getPeriod() const;

        /**
         * Returns always false.
         */
        virtual bool setPeriod(Seconds period);

        /**
         * Returns zero.
         */
        virtual nsecs getPeriodNS() const;

        /**
         * Returns always true.
         */
        virtual bool isRunning() const;

        /**
         * Returns always true.
         */
        virtual bool isActive() const;

        /**
         * Returns "main".
         */
        virtual const char* getName() const;

        virtual void setName(const char* name);

        virtual RTOS_TASK * getTask();

        virtual const RTOS_TASK * getTask() const;

        virtual bool setScheduler(int sched_type);

        virtual int getScheduler() const;

        virtual bool setPriority(int priority);

        virtual int getPriority() const;

        virtual unsigned int getPid() const;

        virtual void setMaxOverrun(int m);

        virtual int getMaxOverrun() const;

        virtual void setWaitPeriodPolicy(int p);

        virtual void yield();

        /**
         * Returns zero, the number of the main() thread.
         */
        unsigned int threadNumber() const;
    protected:

    };
}}

#endif
