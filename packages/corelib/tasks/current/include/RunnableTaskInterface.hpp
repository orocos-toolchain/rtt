#ifndef RUNNABLETASKINTERFACE_HPP
#define RUNNABLETASKINTERFACE_HPP

#include "Time.hpp"

namespace ORO_CoreLib
{
    /**
     * @brief Interface to start/stop a task.
     *
     * A RunnableTaskInterface provides the control methods
     * for tasks. They can be started, stopped
     * and be queried for their state and period.
     *
     * It is complementary to the RunnableInterface, which
     * defines the methods for the functionality.
     */
    class RunnableTaskInterface
    {
    public:

        /**
         * Start the task.
         * 
         * @return true if the task is started, false otherwise
         */
        virtual bool start() = 0;

        /**
         * Stop the task
         *
         * @return true if the task is stopped, false otherwise
         */
        virtual bool stop() = 0;

        /**
         * Query for the tasks state.
         *
         * @return true if it is running, false otherwise
         */
        virtual bool isRunning() = 0;

        /**
         * Get the periodicity of this task in Seconds
         *
         * @return The execution period of this task
         */
        virtual Seconds periodGet() = 0;
    };

}

#endif
