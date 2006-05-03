#ifndef ORO_SLAVE_ACTIVITY_HPP
#define ORO_SLAVE_ACTIVITY_HPP

#include "ActivityInterface.hpp"
#include "RunnableInterface.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_CoreLib
{

    /**
     * @brief An ActivityInterface implementation which executes
     * 'step' upon the invocation of 'trigger()', which is called by
     * another Activity.  The SlaveActivity can only be started if the
     * master is active or if no master is present. If a master is used, 
     * this activity takes periodicity over from the master. If no
     * master is present, use one of the alternative constructors to make
     * a periodic slave or a non periodic slave.
     *
     * \section TrigReact Reactions to trigger():
     * In the non periodic case, RunnableInterface::loop() is called, in the
     * periodic case, RunnableInterface::step() is called. In case the
     * RunnableInterface did not implement loop(), step() is invoked by default.
     * If no RunnableInterface is given, said functions are called upon
     * SlaveActivity itself.
     */
    class SlaveActivity
        :public ActivityInterface
    {
    public:
        /**
         * Create an activity which is the slave of \a master.
         * @param master The activity which will execute this activity.
         * @param run Run this instance.
         */
        SlaveActivity( ActivityInterface* master, RunnableInterface* run = 0 );

        /**
         * Create an activity which is periodic.
         * @param period The periodicity at which you will \a trigger() this activity.
         * @param run Run this instance.
         */
        SlaveActivity( double period, RunnableInterface* run = 0 );

        /**
         * Create an activity for which \a trigger() will not be periodically called.
         * @param run Run this instance.
         */
        SlaveActivity( RunnableInterface* run = 0 );

        Seconds getPeriod() const;

        EventProcessor* getEventProcessor() const;

        ORO_OS::ThreadInterface* thread();

        bool initialize();
        void step();
        void loop();
        bool breakLoop();
        void finalize();

        /**
         * Start this slave. Only allowed if the master activity is already running.
         * @retval false if !master->isRunning() || this->isRunning() || initialize() returns false
         * @retval true othwerise.
         */
        bool start();

        bool stop();

        bool isRunning() const;

        bool run(RunnableInterface* _r);

        bool isPeriodic() const;

        bool isActive() const;

        /**
         * Call this function to 'trigger' the execution
         * of this Activity.
         * @return true when this->isActive(), false otherwise.
         */
        bool trigger();
    private:
        ActivityInterface* mmaster;
        double mperiod;
        RunnableInterface* runner;
        bool running;
        bool active;
};

}


#endif
