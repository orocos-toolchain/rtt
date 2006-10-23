#ifndef ORO_PARSEREXECUTION_ACCESS_HPP
#define ORO_PARSEREXECUTION_ACCESS_HPP

#include "ExecutionAccess.hpp"

namespace RTT
{
    class TaskContext;

    /**
     * @brief The Parser Execution Access provides access to programs
     * and state machines loaded in the execution engine.
     * It can be found as the \a engine object of a TaskContext.
     */
    class ParserExecutionAccess
        : public ExecutionAccess
    {
    public:
        ParserExecutionAccess( TaskContext* parent );

        virtual ~ParserExecutionAccess();
    };

}

#endif
