#ifndef ORO_ACTIVITY_HPP
#define ORO_ACTIVITY_HPP

#include "Time.hpp"
#include "RunnableInterface.hpp"
#include "ActivityInterface.hpp"
#include "os/Thread.hpp"

namespace RTT
{

    /**
     * @brief An Activity is an object that represents a thread.
     *
     * This object implements the ActivityInterface and maps that to an
     * OS thread, using the RTT::OS::Thread class. One Activity object
     * maps to one OS thread. It can execute periodically the step() function
     * or the loop() function in case of non-periodic execution.
     *
     * When provided one, it will execute a RunnableInterface object, or the equivalent methods in
     * it's own interface when none is given.
     *
     * @ingroup CoreLibActivities
     */
    class RTT_API Activity: public ActivityInterface, public OS::Thread
    {
    public:
        using OS::Thread::run;
        using ActivityInterface::run;

        /**
         * @brief Create a not real-time Activity.
         * This creates a not real-time, non-periodic thread.
         *
         * @param _r The optional runner, if none, this->loop() is called.
         * @param name The name of the underlying thread.
         */
        Activity(RunnableInterface* r = 0, const std::string& name ="Activity" );

        /**
         * @brief Create a real-time Activity with a given priority and period.
         * The thread is run in the ORO_SCHED_RT scheduler.
         *
         * @param priority
         *        The priority of this activity.
         * @param period
         *        The periodicity of the Activity. A zero is interpreted as non periodic.
         * @param r
         *        The optional RunnableInterface to run exclusively within this Activity
         * @param name The name of the underlying thread.
         */
        Activity(int priority, Seconds period = 0.0,
                 RunnableInterface* r = 0, const std::string& name ="Activity");

        /**
         * @brief Create an Activity with a given scheduler type, priority and period.
         *
         * @param scheduler
         *        The scheduler in which the activity's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority
         *        The priority of this activity.
         * @param period
         *        The periodicity of the Activity
         * @param r
         *        The optional RunnableInterface to run exclusively within this Activity
         * @param name The name of the underlying thread.
         */
        Activity(int scheduler, int priority, Seconds period,
                 RunnableInterface* r = 0, const std::string& name ="Activity");

        /**
         * Stops and terminates a Activity
         */
        virtual ~Activity();

        virtual bool start();

        virtual bool execute();

        virtual bool trigger();

        virtual bool stop();

        virtual bool isRunning() const;

        virtual bool isActive() const;

        virtual bool isPeriodic() const;

        virtual Seconds getPeriod() const;

        virtual bool setPeriod(Seconds period);

        virtual OS::ThreadInterface* thread();

        /**
         * @see RunnableInterface::initialize()
         */
        virtual bool initialize();

        /**
         * @see RunnableInterface::step()
         */
        virtual void step();

        /**
         * @see RunnableInterface::loop()
         */
        virtual void loop();

        /**
         * @see RunnableInterface::breakLoop()
         */
        virtual bool breakLoop();

        /**
         * @see RunnableInterface::finalize()
         */
        virtual void finalize();

        /**
         * Run another (or self in case of null)
         * task. This overload is needed to redirect
         * the call of run( 0 ) to the correct function.
         */
        bool run( int ) { RunnableInterface* d(0); return ActivityInterface::run(d); }

    };

}

#endif
