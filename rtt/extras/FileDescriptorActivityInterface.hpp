/***************************************************************************

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

#ifndef FILEDESCRIPTOR_ACTIVITY_INTERFACE_HPP
#define FILEDESCRIPTOR_ACTIVITY_INTERFACE_HPP

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
    class RTT_API FileDescriptorActivityInterface
    {
    public:
        virtual ~FileDescriptorActivityInterface() {};

        /** Sets the file descriptor the activity should be listening to.
         *
         * This method is thread-safe, i.e. it can be called from any thread
         *
         * @param fd the file descriptor
         */
        virtual void watch(int fd) = 0;

        /** Removes a file descriptor from the set of watched FDs
         *
         * This method is thread-safe, i.e. it can be called from any thread
         *
         * @param fd the file descriptor
         */
        virtual void unwatch(int fd) = 0;

        /** Remove all FDs that are currently being watched */
        virtual void clearAllWatches() = 0;

        /** True if this specific FD is being watched by the activity
         *
         * @param fd the file descriptor
         */
        virtual bool isWatched(int fd) const = 0;

        /** True if this specific FD has new data.
         *
         * This should only be used from within the base::RunnableInterface this
         * activity is driving, i.e. in TaskContext::updateHook() or
         * TaskContext::errorHook().
         *
         * @param fd the file descriptor
         */
        virtual bool isUpdated(int fd) const = 0;

        /** True if the base::RunnableInterface has been triggered because of a
         * timeout, instead of because of new data is available.
         *
         * This should only be used from within the base::RunnableInterface this
         * activity is driving, i.e. in TaskContext::updateHook() or
         * TaskContext::errorHook().
         */
        virtual bool hasTimeout() const = 0;

        /** True if one of the file descriptors has a problem (for instance it
         * has been closed)
         *
         * This should only be used from within the base::RunnableInterface this
         * activity is driving, i.e. in TaskContext::updateHook() or
         * TaskContext::errorHook().
         */
        virtual bool hasError() const = 0;

        /** Sets the timeout, in milliseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
		 * @pre 0 <= timeout (otherwise an error is logged and \a timeout
		 * is ignored)
         *
         * @param timeout The timeout (milliseconds)
         */
        virtual void setTimeout(int timeout) = 0;

        /** Sets the timeout, in microseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
		 * @pre 0 <= timeout (otherwise an error is logged and \a timeout_us
		 * is ignored)
         *
         * @param timeout The timeout (microseconds)
         */
        virtual void setTimeout_us(int timeout_us) = 0;

        /** Get the timeout, in milliseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
         *
         * @return The timeout (milliseconds)
         */
        virtual int getTimeout() const = 0;

        /** Get the timeout, in microseconds, for waiting on the IO. Set to 0
         * for blocking behaviour (no timeout).
         *
         * @return The timeout (microseconds)
         */
        virtual int getTimeout_us() const = 0;
    };
}}

#endif
