#ifndef FILEDESCRIPTOR_ACTIVITY_HPP
#define FILEDESCRIPTOR_ACTIVITY_HPP

#include "NonPeriodicActivity.hpp"
#include <set>

namespace RTT {
    /** An activity which is triggered by the availability of data on a given
     * file descriptor. step() (and hence the RunnableInterface's step() method)
     * is called when data is available or when an error is encountered on the
     * file descriptor.
     */
    class FileDescriptorActivity : public NonPeriodicActivity
    {
        std::set<int> m_watched_fds;
        bool m_running;
        int  m_interrupt_pipe[2];
        int  m_timeout;
        fd_set m_fd_set;
        fd_set m_fd_work;
        bool m_has_error;
        bool m_has_timeout;
        RunnableInterface* runner;

        static const int CMD_BREAK_LOOP = 0;
        static const int CMD_TRIGGER    = 1;

    public:
        /**
         * Create a FileDescriptorActivity with a given priority and RunnableInterface
         * instance. The default scheduler for NonPeriodicActivity
         * objects is ORO_SCHED_RT.
         *
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        FileDescriptorActivity(int priority, RunnableInterface* _r = 0 );

        /**
         * Create a FileDescriptorActivity with a given scheduler type, priority and
         * RunnableInterface instance.
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        FileDescriptorActivity(int scheduler, int priority, RunnableInterface* _r = 0 );

        /**
         * Create a FileDescriptorActivity with a given priority, name and
         * RunnableInterface instance.
         * @param priority The priority of the underlying thread.
         * @param name The name of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        FileDescriptorActivity(int priority, const std::string& name, RunnableInterface* _r = 0 );

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

        /** True if this specific FD has been updated. This should only be
         * called from the RunnableInterface this activity is driving.
         */
        bool isUpdated(int fd) const;

        /** True if the RunnableInterface has been triggered because of a
         * timeout, instead of because of new data is available.
         *
         * This should only be used from within the RunnableInterface this
         * activity is driving, i.e. in TaskContext::updateHook() or
         * TaskContext::errorHook().
         */
        bool hasTimeout() const;

        /** True if one of the file descriptors has a problem (for instance it
         * has been closed)
         *
         * This should only be used from within the RunnableInterface this
         * activity is driving.
         */
        bool hasError() const;

        /** Sets the timeout, in milliseconds, for waiting on the IO. Set to 0
         * for infinity.
         */
        void setTimeout(int timeout);

        /** Get the timeout, in milliseconds, for waiting on the IO. Set to 0
         * for infinity.
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
}

#endif
