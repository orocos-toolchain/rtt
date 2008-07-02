#ifndef FILEDESCRIPTOR_ACTIVITY_HPP
#define FILEDESCRIPTOR_ACTIVITY_HPP

#include "NonPeriodicActivity.hpp"

namespace RTT {
    /** An activity which is triggered by the availability of data on a given
     * file descriptor. step() (and hence the RunnableInterface's step()
     * method) is called when data is available or when an error is encountered
     * on the file descriptor.
     *
     * The file descriptor can be given by two means:
     * <ul>
     *   <li> setFileDescriptor is called
     *   <li> one of the underlying task contexts (if any) provides the
     *   FileDescriptorActivity::Provider interface. In that case, the
     *   getFileDescriptor() method of that task context is called to
     *   get the descriptor
     * </ul>
     */
    class FileDescriptorActivity : public NonPeriodicActivity
    {
        int  m_fd;
        bool m_close_on_stop;
        int  m_interrupt_pipe[2];
        RunnableInterface* runner;

    public:
        /** If this interface is derived from one of the TaskContext objects
         * which are executed by this activity, then the activity can
         * auto-configure itself (i.e. it can get the file descriptor from
         * that task context). Otherwise, setFileDescriptor must be called
         * explicitely
         */
        struct Provider {
            virtual int getFileDescriptor() const = 0;
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

        int  getFileDescriptor() const;
        void setFileDescriptor(int fd, bool close_on_stop = false);

        virtual bool start();
        virtual void loop();
        virtual bool breakLoop();
        virtual bool stop();
        virtual void step();
    };
}

#endif
