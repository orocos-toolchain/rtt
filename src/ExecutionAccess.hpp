#ifndef ORO_EXECUTION_ACCESS_HPP
#define ORO_EXECUTION_ACCESS_HPP

#include <TaskContext.hpp>

namespace RTT
{
    /**
     * @brief The ExecutionAccess provides access to programs
     * and state machines loaded in the execution engine.
     * It can be found as the \a engine object of a TaskContext.
     */
    class ExecutionAccess
    {
    protected:
        TaskContext* tc;

    public:
        ExecutionAccess( TaskContext* parent );

        virtual ~ExecutionAccess();

        /**
         * @name Script Program Commands
         * @{
         */
        /**
         * Start a Program.
         * @param name The name of the Program.
         */
        bool startProgram(const std::string& name);

        /**
         * Stops the execution of a program.
         * @param name The name of the Program.
         */
        bool stopProgram(const std::string& name);

        /**
         * Pauses the execution of a running program.
         * @param name The name of the Program.
         */
        bool pauseProgram(const std::string& name);

        /**
         * Steps a single instruction of a paused program.
         * @param name The name of the Program.
         */
        bool stepProgram(const std::string& name);
        /**
         *@}
         */

        /**
         * @name Script Program Methods
         * @{
         */
        /** 
         * Check if a program is loaded
         * 
         * @param name The name of the program.
         * 
         * @return True if so.
         */
        virtual bool hasProgram(const std::string& name) const;

        /** 
         * Inspect if a loaded program is in the running state
         * 
         * @param name The name of the Program.
         * 
         * @return true if so.
         */
        bool isProgramRunning(const std::string& name) const;

        /** 
         * Inspect if a loaded program is in the paused state
         * 
         * @param name The name of the Program.
         * 
         * @return true if so.
         */
        bool isProgramPaused(const std::string& name) const;

        /** 
         * Inspect if a loaded program is in the error state
         * 
         * @param name The name of the Program.
         * 
         * @return true if so.
         */
        bool inProgramError(const std::string& name) const;
        /**
         *@}
         */

        /**
         * @name Script State Machine Commands
         * @{
         */
        /**
         * Activate a previously loaded StateMachine.
         * @param name The name of the StateMachine.
         */
        bool activateStateMachine(const std::string& name);

        /**
         * Deactivate a stopped StateMachine.
         * @param name The name of the StateMachine.
         */
        bool deactivateStateMachine(const std::string& name);

        /**
         * Start a previously activated StateMachine.
         * @param name The name of the StateMachine.
         */
        bool startStateMachine(const std::string& name);

        /**
         * Pause a previously activated StateMachine.
         * @param name The name of the StateMachine.
         */
        bool pauseStateMachine(const std::string& name);

        /**
         * Stops the execution of a StateMachine and enters the Final_State.
         * @param name The name of the StateMachine.
         */
        bool stopStateMachine(const std::string& name);

        /**
         * Resets the execution of a StateMachine and enters the Initial_State.
         * @param name The name of the StateMachine.
         */
        bool resetStateMachine(const std::string& name);

        /**
         * Request a state change in a state machine.
         * @param name The name of the StateMachine.
         * @param state The state to enter.
         * @return true if the state change request was accepted.
         */
        bool requestStateMachineState(const std::string& name, const std::string& state);
        /**
         *@}
         */

        /**
         * @name Script State Machine Methods
         * @{
         */
        /** 
         * Check if a state machine is loaded.
         * 
         * @param name The name of the state machine.
         * 
         * @return True if so.
         */
        virtual bool hasStateMachine(const std::string& name) const;

        /**
         * Returns true if the state machine has been activated.
         * @param name The name of the StateMachine.
         */
        bool isStateMachineActive(const std::string& name) const;

        /**
         * Returns true if the state machine was successfully started.
         * @param name The name of the StateMachine.
         */
        bool isStateMachineRunning(const std::string& name) const;

        /**
         * Returns true if the state machine is in error.
         * @param name The name of the StateMachine.
         */
        bool inStateMachineError(const std::string& name) const;

        /**
         * Returns true if the state machine is paused.
         * @param name The name of the StateMachine.
         */
        bool isStateMachinePaused(const std::string& name) const;

        /**
         * Get the current state of a state machine.
         * @param name The name of the StateMachine.
         */
        const std::string& getStateMachineState(const std::string& name) const;

        /**
         * Check if a state machine is in a given state
         * @param name The name of the StateMachine.
         * @param state The state to enter.
         * @return true if so.
         */
        bool inStateMachineState(const std::string& name, const std::string& state) const;
        /**
         *@}
         */

        bool true_gen() const;
    };

}

#endif
