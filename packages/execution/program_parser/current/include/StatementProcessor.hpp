#ifndef ORO_EXECUTION_STATEMENT_PROCESSOR_HPP
#define ORO_EXECUTION_STATEMENT_PROCESSOR_HPP

#include <string>
#include "CommandC.hpp"

namespace ORO_Execution
{
    class TaskContext;

    /**
     * This class parses and executes a single scripting
     * statement.
     */
    class StatementProcessor
    {
        class D;
        D* d;
    public:
        StatementProcessor(TaskContext* tc);

        ~StatementProcessor();
        /**
         * Execute a script statement.
         * @retval -1 a parse error occured.
         * @retval 0 statement executed
         * @return a positive number if it is a command, which
         * is the ticket number of the command.
         */
        int execute(const std::string& code);

        /**
         * Check if commands are finished and free resources if so.
         */
        void checkFinished();

        /**
         * Return the command with the ticket number \a cnr.
         */
        CommandC getCommand( int cnr );
    };


}


#endif
#ifndef ORO_EXECUTION_STATEMENT_PROCESSOR_HPP
#define ORO_EXECUTION_STATEMENT_PROCESSOR_HPP

#include <string>
#include "CommandC.hpp"

namespace ORO_Execution
{
    class TaskContext;

    /**
     * This class parses and executes a single scripting
     * statement.
     */
    class StatementProcessor
    {
        class D;
        D* d;
    public:
        StatementProcessor(TaskContext* tc);

        ~StatementProcessor();
        /**
         * Execute a script statement.
         * @retval -1 a parse error occured.
         * @retval 0 statement executed
         * @return a positive number if it is a command, which
         * is the ticket number of the command.
         */
        int execute(const std::string& code);

        /**
         * Check if commands are finished and free resources if so.
         */
        void checkFinished();

        /**
         * Return the command with the ticket number \a cnr.
         */
        CommandC getCommand( int cnr );
    };


}


#endif
#ifndef ORO_EXECUTION_STATEMENT_PROCESSOR_HPP
#define ORO_EXECUTION_STATEMENT_PROCESSOR_HPP

#include <string>
#include "CommandC.hpp"

namespace ORO_Execution
{
    class TaskContext;

    /**
     * This class parses and executes a single scripting
     * statement.
     */
    class StatementProcessor
    {
        class D;
        D* d;
    public:
        StatementProcessor(TaskContext* tc);

        ~StatementProcessor();
        /**
         * Execute a script statement.
         * @retval -1 a parse error occured.
         * @retval 0 statement executed
         * @return a positive number if it is a command, which
         * is the ticket number of the command.
         */
        int execute(const std::string& code);

        /**
         * Check if commands are finished and free resources if so.
         */
        void checkFinished();

        /**
         * Return the command with the ticket number \a cnr.
         */
        CommandC getCommand( int cnr );
    };


}


#endif
#ifndef ORO_EXECUTION_STATEMENT_PROCESSOR_HPP
#define ORO_EXECUTION_STATEMENT_PROCESSOR_HPP

#include <string>
#include "CommandC.hpp"

namespace ORO_Execution
{
    class TaskContext;

    /**
     * This class parses and executes a single scripting
     * statement.
     */
    class StatementProcessor
    {
        class D;
        D* d;
    public:
        StatementProcessor(TaskContext* tc);

        ~StatementProcessor();
        /**
         * Execute a script statement.
         * @retval -1 a parse error occured.
         * @retval 0 statement executed
         * @return a positive number if it is a command, which
         * is the ticket number of the command.
         */
        int execute(const std::string& code);

        /**
         * Check if commands are finished and free resources if so.
         */
        void checkFinished();

        /**
         * Return the command with the ticket number \a cnr.
         */
        CommandC getCommand( int cnr );
    };


}


#endif
