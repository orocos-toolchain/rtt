#ifndef TASK_NON_PERIODIC_HPP
#define TASK_NON_PERIODIC_HPP

#include "TaskInterface.hpp"
#include "RunnableInterface.hpp"
#include "EventProcessor.hpp"
#include "os/SingleThread.hpp"

#pragma interface

namespace ORO_CoreLib
{

    /**
     * @brief A TaskInterface implementation which is run in a SingleThread.
     *
     * By default, (if no RunnableInterface is given and no overloads
     * are present ) \a loop() will block for Events to be processed and
     * can be stopped by calling stop(). To check for events from your overloads or the RunnableInterface,
     * call this->processor()->step(), for non blocking checks and this->processor()->loop() for blocking
     * on Events.
     */
    class TaskNonPeriodic
        :public TaskInterface,
         private ORO_OS::SingleThread
    {
    public:
        /**
         * Create an TaskNonPeriodic with a given priority and
         * RunnableInterface instance.
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->step() is called.
         */
        TaskNonPeriodic(int priority, RunnableInterface* _r = 0 )
            : ORO_OS::SingleThread(priority, "TaskNonPeriodic" ),proc( new BlockingEventProcessor() ), runner(_r)
        {
            if ( runner )
                runner->setTask(this);
        }

        virtual ~TaskNonPeriodic() 
        {
            this->stop();
            if ( runner )
                runner->setTask( 0 );
            delete proc;
        }
        
        /**
         * Run another (or self in case of null)
         * task.
         */
        bool run( RunnableInterface* r )
        {
            if ( isRunning() )
                return false;
            if (runner)
                runner->setTask(0);
            runner = r;
            if (runner)
                runner->setTask(this);
            return true;
        }

        virtual Seconds getPeriod() const { return 0; }

        virtual EventProcessor* processor() const { return proc; }

        virtual bool initialize() {
            bool result = proc->initialize();
            if ( runner && result )
                result = result && runner->initialize();
            return  result;
        }

        virtual void loop() { 
            if ( runner )
                runner->loop();
            else
                proc->loop(); // block in EventProcessor.
        }

        virtual void finalize() {
            if ( runner )
                runner->finalize();
            proc->finalize();
        }

        virtual bool start() {
            return SingleThread::start();
        }

        /**
         * Stop the EventProcessor's loop.
         */
        bool stop() {
            proc->breakloop();
            return true;
        }

        virtual bool isRunning() const {
            return SingleThread::isRunning();
        }

    private:
        BlockingEventProcessor* proc;
        RunnableInterface*      runner;
};

}


#endif
