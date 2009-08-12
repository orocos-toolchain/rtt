#ifndef ORO_SEQUENTIAL_ACTIVITY_HPP
#define ORO_SEQUENTIAL_ACTIVITY_HPP

#include "../base/ActivityInterface.hpp"
#include "../base/RunnableInterface.hpp"
#include "os/Mutex.hpp"

namespace RTT
{

    /**
     * @brief The default, thread-less activity for any newly created TaskContext.
     *
     * The presence of this activity object indicates that the TaskContext does
     * not have its own active thread, but executes all asynchronous operations
     * (such as commands and events) in the thread of the caller, so in fact
     * synchronously. The SequentialActivity uses a Mutex lock to guard against
     * concurrent executions and makes your TaskContext equally thread-safe as
     * the other activity implementations.
     *
     * This activity emulates the same behaviour as a NonPeriodicActivity, but
     * without using a thread.
     *
     * \section ExecReact Reactions to execute():
     * Always returns false.
     *
     * \section TrigReact Reactions to trigger():
     * This causes step() to be executed.
     *
     * @ingroup CoreLibActivities
     */
    class RTT_API SequentialActivity
        :public ActivityInterface
    {
    public:
        /**
         * Create an activity which is the Sequential. The period will be 0.0.
         * @param run Run this instance.
         */
        SequentialActivity( RunnableInterface* run = 0 );

        /**
         * Cleanup and notify the RunnableInterface that we are gone.
         */
        ~SequentialActivity();

        Seconds getPeriod() const;

        OS::ThreadInterface* thread();

        bool initialize();
        void step();
        void loop();
        bool breakLoop();
        void finalize();

        bool start();

        bool stop();

        bool isRunning() const;

        bool isPeriodic() const;

        bool isActive() const;

        bool execute();

        bool trigger();
    private:
        bool running;
        bool active;
        OS::Mutex execution_lock;
};

}


#endif
