/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  FileDescriptorActivity.hpp

                        FileDescriptorActivity.hpp -  description
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


#ifndef FILEDESCRIPTOR_ACTIVITY_HPP
#define FILEDESCRIPTOR_ACTIVITY_HPP

#include "FileDescriptorActivityInterface.hpp"
#include "../Activity.hpp"
#include <set>

namespace RTT { namespace extras {

    /** An activity which is triggered by the availability of data on a set of
     * file descriptors. step() (and hence the base::RunnableInterface's step()
     * method) is called when data is available or when an error is encountered
     * on the file descriptor.
     *
     * To use it, one must add the file descriptors to watch in the task's
     * configureHook()
     *
     * <code>
     *   FileDescriptorActivity* fd_activity =
     *      dynamic_cast<FileDescriptorActivity*>(getActivity().get());
     *   if (fd_activity)
     *   {
     *      fd_activity->watch(device_fd);
     *      // optional, set a timeout in milliseconds
     *      fd_activity->setTimeout(1000);
     *      // or in microseconds
     *      fd_activity->setTimeout_us(1000);
     *   }
     * </code>
     *
     * Then, updateHook() and -- when in ERROR state -- errorHook() will be
     * called when one of these three events happen:
     * <ul>
     *      <li>new data is available on one of the watched FDs
     *      <li>an error happens on one of the watched FDs
     *      <li>the timeout is reached
     * </ul>
     *
     * The different cases can be tested in updateHook() as follows:
     *
     * <code>
     * FileDescriptorActivity* fd_activity =
     *    dynamic_cast<FileDescriptorActivity*>(getActivity().get());
     * if (fd_activity)
     * {
     *   if (fd_activity->hasError())
     *   {
     *   }
     *   else if (fd_activity->hasTimeout())
     *   {
     *   }
     *   else
     *   {
     *     // If there is more than one FD, discriminate. Otherwise,
     *     // we don't need to use isUpdated
     *     if (fd_activity->isUpdated(device_fd))
     *     {
     *     }
     *     else if (fd_activity->isUpdated(another_fd))
     *     {
     *     }
     *   }
     * }
     * </code>
     */
    class RTT_API FileDescriptorActivity : public extras::FileDescriptorActivityInterface,
                                           public Activity
    {
        std::set<int> m_watched_fds;
        bool m_running;
        int  m_interrupt_pipe[2];
        int  m_timeout_us;		//! timeout in microseconds
        Seconds m_period;		//! intended period
        /** Lock that protects the access to m_fd_set and m_watched_fds */
        mutable RTT::os::Mutex m_lock;
        fd_set m_fd_set;
        fd_set m_fd_work;
        bool m_has_error;
        bool m_has_timeout;

        static const char CMD_ANY_COMMAND = 0;
        RTT::os::Mutex m_command_mutex;
        bool m_break_loop;
        bool m_trigger;
        bool m_user_timeout;
        bool m_update_sets;

        /** Internal method that makes sure loop() takes into account
         * modifications in the set of watched FDs
         */
        void triggerUpdateSets();

        /** Internal method that writes on an internal pipe to wake up
         * the main loop
         */
        void writeInterruptPipe();

        /** Internal method that clears the interrupt pipe used to wake
         * up the main loop
         */
        void clearInterruptPipe();

    public:
        /**
         * Create a FileDescriptorActivity with a given priority and base::RunnableInterface
         * instance. The default scheduler for NonPeriodicActivity
         * objects is ORO_SCHED_RT.
         *
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         * @param name The name of the underlying thread.
         */
        FileDescriptorActivity(int priority, base::RunnableInterface* _r = 0, const std::string& name ="FileDescriptorActivity" );

        /**
         * Create a FileDescriptorActivity with a given scheduler type, priority and
         * base::RunnableInterface instance.
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         * @param name The name of the underlying thread.
         */
        FileDescriptorActivity(int scheduler, int priority, base::RunnableInterface* _r = 0, const std::string& name ="FileDescriptorActivity" );

        /**
         * Create a FileDescriptorActivity with a given scheduler type, priority, _intended_ period, and
         * RunnableInterface instance.
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority The priority of the underlying thread.
         * @param period The _intended_ periodicity of the activity
         * @param _r The optional runner, if none, this->loop() is called.
         * @param name The name of the underlying thread.
         */
        FileDescriptorActivity(int scheduler, int priority, Seconds period, base::RunnableInterface* _r = 0, const std::string& name ="FileDescriptorActivity" );

        /**
         * Create a FileDescriptorActivity with a given scheduler type, priority, _intended_ period,
         * CPU affinity, and RunnableInterface instance.
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority The priority of the underlying thread.
         * @param period The _intended_ periodicity of the activity
         * @param cpu_affinity The prefered cpu to run on (a mask)
         * @param _r The optional runner, if none, this->loop() is called.
         * @param name The name of the underlying thread.
         */
        FileDescriptorActivity(int scheduler, int priority, Seconds period, unsigned cpu_affinity,
							   base::RunnableInterface* _r = 0, const std::string& name ="FileDescriptorActivity" );

        virtual ~FileDescriptorActivity();

        bool isRunning() const;

		/// Get the _intended_ period (not the actual running period)
        virtual Seconds getPeriod() const;

		/// Set the _intended_ period (not the actual running period)
        virtual bool setPeriod(Seconds period);

        /** Sets the file descriptor the activity should be listening to.
         *
         * This method is thread-safe, i.e. it can be called from any thread
         *
         * @param fd the file descriptor
         */
        void watch(int fd);

        /** Removes a file descriptor from the set of watched FDs
         *
         * This method is thread-safe, i.e. it can be called from any thread
         *
         * @param fd the file descriptor
         */
        void unwatch(int fd);

        /** Remove all FDs that are currently being watched */
        void clearAllWatches();

        /** True if this specific FD is being watched by the activity
         *
         * @param fd the file descriptor
         */
        bool isWatched(int fd) const;

        /** True if this specific FD has new data.
         *
         * This should only be used from within the base::RunnableInterface this
         * activity is driving, i.e. in TaskContext::updateHook() or
         * TaskContext::errorHook().
         *
         * @param fd the file descriptor
         */
        bool isUpdated(int fd) const;

        /** True if the base::RunnableInterface has been triggered because of a
         * timeout, instead of because of new data is available.
         *
         * This should only be used from within the base::RunnableInterface this
         * activity is driving, i.e. in TaskContext::updateHook() or
         * TaskContext::errorHook().
         */
        bool hasTimeout() const;

        /** True if one of the file descriptors has a problem (for instance it
         * has been closed)
         *
         * This should only be used from within the base::RunnableInterface this
         * activity is driving, i.e. in TaskContext::updateHook() or
         * TaskContext::errorHook().
         */
        bool hasError() const;

        /** Sets the timeout, in milliseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
		 * @pre 0 <= timeout (otherwise an error is logged and \a timeout
		 * is ignored)
         *
         * @param timeout The timeout (milliseconds)
         */
        void setTimeout(int timeout);

        /** Sets the timeout, in microseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
		 * @pre 0 <= timeout (otherwise an error is logged and \a timeout_us
		 * is ignored)
         *
         * @param timeout The timeout (microseconds)
         */
        void setTimeout_us(int timeout_us);

        /** Get the timeout, in milliseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
         *
         * @return The timeout (milliseconds)
         */
        int getTimeout() const;

        /** Get the timeout, in microseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
         *
         * @return The timeout (microseconds)
         */
        int getTimeout_us() const;

        /** Start the underlying thread and make it call \c loop
         */
        virtual bool start();

        /** The main loop, listening to various wake-up events
         *
         * The loop can be broken by calling \c breakLoop. \c timeout and \c trigger
         * will wake it up and make it call work with resp. a TimeOut and Trigger
         * reason. Available I/O on watched file descriptors will wake it up and
         * make it call \c work with a IOReady reason
         */
        virtual void loop();

        /** Wake-up \c loop and make it return */
        virtual bool breakLoop();
        virtual bool stop();

        /** @deprecated does nothing, FileDescriptorActivity uses the \c work interface
         */
        virtual void step();

        /** Called by loop() when it is woken up
         *
         * The reason parameter allows to know why the loop was woken up:
         * - TimeOut if the activity time out has been reached or if \c timeout
         *   has been called. In the former case, \c hasTimeout will return true.
         * - IOReady is some I/O has been received on the watched file descriptors
         * - Trigger if trigger() has been called
         *
         * Calls runner->work with the same reason. By default \c
         * ExecutionEngine will call all messages, port callbacks, functions
         * and hooks in IOReady and TimeOut, but only messages and port
         * callbacks in Trigger
         */
        virtual void work(base::RunnableInterface::WorkReason reason);

        /**
         * Wake up the main thread (in \c loop) and call \c work with Trigger as reason
         *
         * @return true if the activity is active, that is if it is started and
         *   will process the trigger. false otherwise.
         */
        virtual bool trigger();

        /**
         * Wake up the main thread (in \c loop) and call \c work with TimeOut as reason
         *
         * @return true if the activity is active, that is if it is started and
         *   will process the trigger. false otherwise.
         */
        virtual bool timeout();
    };
}}

#endif
