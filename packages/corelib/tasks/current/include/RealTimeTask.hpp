#ifndef REALTIMETASK_HPP
#define REALTIMETASK_HPP

#include "RunnableInterface.hpp"
#include "EventListenerInterface.hpp"

#include "Time.hpp"
#include "TaskExecution.hpp"
#include "EventPeriodic.hpp"
#include "RunnableTaskInterface.hpp"

namespace ORO_CoreLib
{
    /**
     * A RealTimeTask is the general implementation of a Task
     * which has realtime constraints. It will execute a RunnableTaskInterface.
     * When this interface's method returns false, it will finalize() it in the
     * CompletionProcessor. If the RealTimeTask is normally stop()'ed, finalize()
     * is called in the calling thread of stop().
     */
    class RealTimeTask
        : public RunnableTaskInterface
    {
	protected:
        struct Handler;
        friend class Handler;
    public:

        /**
         * Create a RealTime Task with a given period which runs
         * a RunnableInterface.
         *
         * @param period
         *        The periodicity of the RealTimeTask
         * @param r
         *        The optional RunnableInterface to run exclusively within this Task
         */
        RealTimeTask(Seconds period, ORO_OS::RunnableInterface* r=0 );

        /**
         * Create a RealTime Task with a given period which runs
         * a RunnableInterface.
         *
         * @param sec
         *        The periodicity of the RealTimeTask, seconds partition
         * @param nsec
         *        The periodicity of the RealTimeTask, nanoseconds partition
         * @param r
         *        The optional RunnableInterface to run exclusively within this Task
         */
        RealTimeTask(secs sec, nsecs nsec, ORO_OS::RunnableInterface* r=0 );

        /**
         * Stops and terminates a RealTimeTask
         */
        virtual  ~RealTimeTask();
           
        /**
         * Run exclusively this RunnableInterface.
         *
         * @param r
         *        The RunnableInterface to run exclusively.
         * @return true if succeeded, false otherwise
         */
        virtual bool run( ORO_OS::RunnableInterface* r );

        virtual bool start();

        virtual bool stop();

        virtual bool isRunning();

        virtual Seconds periodGet();

        EventListenerInterface* handler() {return evHandler; }
    protected:
        /**
         * Adds the task to the corresponding thread
         */
        virtual bool taskAdd()=0;

        /**
         * Removes this task from the corresponding thread
         */
        virtual void taskRemove()=0; 

        virtual bool initialize() { return true; }
        
        virtual void step() {}
        
        virtual void finalize() {}

        /**
         * Calls the runners or own step function
         */
        void doStep();

        /**
         * Does an unconditional stop.
         */
        void doStop();

        /**
         * The Handler to be registered with the Event which will
         * trigger execution of this RealTimeTask.
         */
        EventListenerInterface* evHandler;

        /**
         * The handler to stop a RunnableTaskInterface when it
         * is inError.
         */
        EventCompleterInterface* taskCompleter;

        /**
         * When runner != 0 it will be executed instead of
         * this instances initialize(), step() and finalize() functions
         */
        ORO_OS::RunnableInterface* runner;

        /**
         * State info.
         */
        bool running;

        /**
         * State info.
         */
        bool inError;

        /**
         * This tasks period in nanoseconds.
         */
        nsecs per_ns;

        /**
         * Used when two threads try to stop simultanously.
         */
        ORO_OS::Mutex stop_lock;
    };

}

#endif
