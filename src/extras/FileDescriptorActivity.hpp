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
    class RTT_API FileDescriptorActivity : public Activity
    {
        std::set<int> m_watched_fds;
        bool m_running;
        int  m_interrupt_pipe[2];
        int  m_timeout;
        fd_set m_fd_set;
        fd_set m_fd_work;
        bool m_has_error;
        bool m_has_timeout;
        base::RunnableInterface* runner;

        static const int CMD_BREAK_LOOP = 0;
        static const int CMD_TRIGGER    = 1;

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

        virtual ~FileDescriptorActivity();

        bool isRunning() const;

        /** Sets the file descriptor the activity should be listening to.
         * @arg close_on_stop { if true, the file descriptor will be closed by the
         * activity when stop() is called. Otherwise, the file descriptor is
         * left as-is.}
         *
         * @param fd the file descriptor
         * @param close_on_stop if true, the FD will be closed automatically when the activity is stopped
         */
        void watch(int fd);

        /** Removes a file descriptor from the set of watched FDs
         *
         * The FD is never closed, even though close_on_stop was set to true in
         * watchFileDescriptor
         */
        void unwatch(int fd);

        /** True if this specific FD is being watched by the activity
         */
        bool isWatched(int fd) const;

        /** True if this specific FD has new data.
         *
         * This should only be used from within the base::RunnableInterface this
         * activity is driving, i.e. in TaskContext::updateHook() or
         * TaskContext::errorHook().
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
         */
        void setTimeout(int timeout);

        /** Get the timeout, in milliseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
         */
        int getTimeout() const;

        virtual bool start();
        virtual void loop();
        virtual bool breakLoop();
        virtual bool stop();
    
        /** Called by loop() when data is available on the file descriptor. By
         * default, it calls step() on the associated runner interface (if any)
         */
        virtual void step();

        /** Force calling step() even if no data is available on the file
         * descriptor, and returns true if the signalling was successful
         */
        virtual bool trigger();
    };
}}

#endif
