#ifndef TASKSIMULATION_HPP
#define TASKSIMULATION_HPP

#include "SimulationThread.hpp"
#include "RealTimeTask.hpp"

namespace ORO_CoreLib
{
    /**
     * @brief A TaskSimulation is a RealTimeTask which is used
     * for simulation.
     *
     * They all run in one thread, which updates the 
     * the system clock such that the task thinks it runs
     * in realtime, while it runs 'as fast as possible'.
     * 
     * They will run in the SimulationThread.
     */
    class TaskSimulation
        : public RealTimeTask
    {
    public:
        /**
         * Construct a TaskSimulation with a periodicity <period>
         * Seconds
         */
        TaskSimulation(Seconds period, RunnableInterface* r=0 );
        /**
         * Construct a TaskSimulation with a periodicity of
         * <s> seconds and <ns> nanoseconds
         */
        TaskSimulation(secs s, nsecs ns, RunnableInterface* r=0 );

        /**
         * Destroys this instance. The Task is stopped.
         */
        virtual  ~TaskSimulation();
                                 
    protected:
        virtual bool taskAdd();

        virtual void taskRemove(); 

        virtual TaskThreadInterface* thread() const { return simt; }

        /**
         * The SimulationThread used for scheduling this task
         */
        SimulationThread* simt;

    };

}

#endif
