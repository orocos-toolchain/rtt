
#ifndef SIMULATIONTHREAD_HPP
#define SIMULATIONTHREAD_HPP

#include "TaskExecution.hpp"


namespace ORO_CoreLib
{

    class TaskSimulation;
    class HeartBeatGenerator;
    /**
     * This thread is the simulated realtime thread in the
     * Orocos system. It behaves (to the TaskSimulation) like a perfect scheduler.
     * 
     * It Uses the Singleton pattern, since there will be only one.
     *
     * @see PeriodicThread, ZeroTimeThread
     */
    class SimulationThread
        : public TaskExecution
    {
    public:
        static SimulationThread* Instance();

        /**
         * Releases the SimulationThread
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release();

        /**
         * Destructor
         */
        virtual ~SimulationThread();

        /**
         * Add a TaskSimulation which is handled each n nanoseconds
         * 
         * @param t The task to handle each n nanoseconds
         * @param n handle every n nanoseconds
         */
        bool taskAdd( TaskSimulation* t, const nsecs n );

        /**
         * Remove a TaskSimulation from handleing
         *
         * @post <t> is no longer handled by this thread
         */
        void taskRemove( TaskSimulation* t );

    protected:
        void step();

        /**
         * Constructor
         */
        SimulationThread();

    private:

        /**
         * Our only instance of the SimulationThread
         */
        static SimulationThread* _instance;

        /**
         * The System clock.
         */
        HeartBeatGenerator* beat;

    };
} // namespace ORO_CoreLib

#endif
