
#ifndef PROGRAM_TASK_HPP
#define PROGRAM_TASK_HPP

#include "TaskContext.hpp"
#include "ProgramInterface.hpp"
#include "DataSource.hpp"

namespace ORO_Execution
{

    /**
     * @brief This class represents a program as a task in
     * the Orocos TaskContext system.
     */
    class ProgramTask
        : public TaskContext
    {
        VariableDataSource<ProgramInterfaceWPtr>::shared_ptr program;
    public:
        /**
         * By constructing this object, a program is added to a taskcontext
         * as a TaskContext, with its commands and methods.
         */
        ProgramTask( ProgramInterfacePtr, ExecutionEngine* ee = 0 );

        ~ProgramTask();

        /**
         * Returns the Program of this task.
         */
        ProgramInterfacePtr getProgram() const { return program->get().lock(); }

    };
}

#endif


