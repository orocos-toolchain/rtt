#ifndef FILEDESCRIPTOR_ACTIVITY_HPP
#define FILEDESCRIPTOR_ACTIVITY_HPP

#include "NonPeriodicActivity.hpp"

namespace RTT {
    /** An activity which is triggered by the availability of data on a given
     * file descriptor. step() (and hence the RunnableInterface's step() method)
     * is called when data is available or when an error is encountered on the
     * file descriptor.
     *
     * The file descriptor can be given by two means:
     * <ul>
     *   <li> setFileDescriptor is called
     *   <li> one of the underlying task contexts (if any) subclasses the
     *   FileDescriptorActivity::Provider interface. In that case, the
     *   getFileDescriptor() method of that task context is called to
     *   get the descriptor
     * </ul>
     */
    class FileDescriptorActivity : public NonPeriodicActivity
    {
        bool m_running;
        int  m_fd;
        int  m_timeout;
        bool m_close_on_stop;
        int  m_interrupt_pipe[2];
        RunnableInterface* runner;

        static const int CMD_BREAK_LOOP = 0;
        static const int CMD_TRIGGER    = 1;

    public:
        /** If this interface is derived from one of the TaskContext objects
         * which are executed by this activity, then the activity can
         * auto-configure itself (i.e. it can get the file descriptor from
         * that task context). Otherwise, setFileDescriptor must be called
         * explicitely
         */
        struct Provider {
            virtual int getFileDescriptor() const = 0;
            virtual int getTimeout() const { return 0; }
        };

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

        /** Returns the file descriptor the activity is listening to. Returns -1
         * if no file descriptor is set
         */
        int  getFileDescriptor() const;
        /** Sets the file descriptor the activity should be listening to.
         * @arg close_on_stop { if true, the file descriptor will be closed by the
         * activity when stop() is called. Otherwise, the file descriptor is
         * left as-is.}
         */
        void setFileDescriptor(int fd, bool close_on_stop = false);

        /** Returns the timeout waiting on the file descriptor in milliseconds.
         * Zero means infinity (no timeout)
         */
        int  getTimeout() const;
        /** Sets the timeout waiting on the file descriptor, in milliseconds.
         * Zero means infinity (no timeout)
         */
        void setTimeout(int timeout);

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
