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

#ifndef FILEDESCRIPTORSIMULATION_ACTIVITY_HPP
#define FILEDESCRIPTORSIMULATION_ACTIVITY_HPP

#include "../base/ActivityInterface.hpp"
#include "../base/RunnableInterface.hpp"
#include "../extras/FileDescriptorActivityInterface.hpp"

namespace RTT { namespace extras {

    /** Simulate a FileDescriptorActivity (FDA)
        Primarily this is designed for unit tests that are testing components
        that require a FDA. This implementation allows deterministic cycling of
        such a component.

        The intended use case is a component that is woken up by its FDA and
        then the component determines why it was woken up by calling
        \a isUpdated(), \a hasTimeout(), and \a hasError(). The unit test code
        should call \a work() with an appropriate reason, which will end up
        being returned to the component through the above calls. So a unit test
        call of \a work(IOReady) will result in \a isUpdated() returning true to
        the component, and similarly for \a work(TimeOut) and \a hasTimeout().
        Currently \a hasError() always returns false - there is no way to
        simulate this with the current implementation.
     
        \note Component OwnThread operations are executed by the MainThread,
        which is correct for a unit test case that is directly executing.
     */
    class RTT_API FileDescriptorSimulationActivity : public extras::FileDescriptorActivityInterface,
                                                     public base::ActivityInterface
    {
    public:
        // priority and name are ignored
        FileDescriptorSimulationActivity(int priority,
                                         base::RunnableInterface* _r = 0,
                                         const std::string& name ="FileDescriptorSimulationActivity" );

        // scheduler, priority, and name, are ignored
        FileDescriptorSimulationActivity(int scheduler,
                                         int priority,
                                         base::RunnableInterface* _r = 0,
                                         const std::string& name ="FileDescriptorSimulationActivity" );

        // scheduler, priority, and name, are ignored
        FileDescriptorSimulationActivity(int scheduler,
                                         int priority,
                                         Seconds period,
                                         base::RunnableInterface* _r = 0,
                                         const std::string& name ="FileDescriptorSimulationActivity" );

        // scheduler, priority, cpu_affinity, and name, are ignored
        FileDescriptorSimulationActivity(int scheduler,
                                         int priority,
                                         Seconds period,
                                         unsigned cpu_affinity,
                                         base::RunnableInterface* _r = 0,
                                         const std::string& name ="FileDescriptorSimulationActivity" );

        /**
         * Cleanup and notify the base::RunnableInterface that we are gone.
         */
        virtual ~FileDescriptorSimulationActivity();

        /**********************************************************************
         * FDAInterface functions
         **********************************************************************/

        /// Does nothing
        void watch(int fd);

        /// Does nothing
        void unwatch(int fd);

        /// Does nothing
        void clearAllWatches();

        /// Returns false always
        bool isWatched(int fd) const;

        /// Return (IOReady == lastReason)
        /// \sa work()
        bool isUpdated(int fd) const;

        /// Return (Timeout == lastReason)
        /// \sa work()
        bool hasTimeout() const;

        bool hasError() const;

        /// Does nothing
        void setTimeout(int timeout);

        /// Does nothing
        void setTimeout_us(int timeout_us);

        /// Return 0
        int getTimeout() const;

        /// Returns 0
        int getTimeout_us() const;

        /**********************************************************************
         * ActivityInterface functions
         **********************************************************************/

        /// Returns true
        virtual bool start();
        /// Returns true
        virtual bool stop();
        /// Returns true
        virtual bool isRunning() const;
        /// Returns true
        virtual bool isActive() const;
        /// Returns \a period
        virtual Seconds getPeriod() const;
        /// Returns true iff (0 != period)
        virtual bool isPeriodic() const;
        /// If s>0 then returns true and \a period == \a s, otherwise returns false
        virtual bool setPeriod(Seconds s) ;
        /// Returns 0
        virtual unsigned getCpuAffinity() const;
        /// Returns true
        virtual bool setCpuAffinity(unsigned cpu) ;
        /// Returns true
        virtual bool execute();
        /// Returns true
        virtual bool trigger();
        /// Returns true
        virtual bool timeout() ;
        /// Returns os::MainThread::Instance()
        virtual os::ThreadInterface* thread();

        /// If have a runner then pass the reason to the runner and store in lastReason
        virtual void work(base::RunnableInterface::WorkReason reason);
    protected:
        /// Fake period - some classes require period!=0.
        /// Is initialized to 0
        Seconds                                 period;
        //// Must model running - simply is true after start() and before stop()
        bool                                    running;
        /// Value passed to last work() call
        /// Used to determine the return from isUpdated() and hasTimeout()
        base::RunnableInterface::WorkReason     lastReason;
    };
}}

#endif
