 
#ifndef RTT_OS_MAIN_THREAD_HPP
#define RTT_OS_MAIN_THREAD_HPP

#include "ThreadInterface.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{ namespace OS {
    
    /**
     * A class which represents the main() thread.
     */
    class MainThread
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
        virtual bool run( OS::RunnableInterface* r);

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
         * Returns zero.
         */
        virtual nsecs getPeriodNS() const;

        /**
         * Returns always true.
         */
        virtual bool isRunning() const;

        /**
         * Returns "main".
         */
        virtual const char* getName() const;

        virtual RTOS_TASK * getTask();
      
        virtual bool setScheduler(int sched_type);

        virtual int getScheduler() const;

        virtual bool setPriority(int priority);

        virtual int getPriority() const;

        /**
         * Returns zero, the number of the main() thread.
         */
        unsigned int threadNumber() const;
    protected:
        
    };
}}

#endif
