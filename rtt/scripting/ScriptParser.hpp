#ifndef RTT_SCRIPTING_PARSER_HPP
#define RTT_SCRIPTING_PARSER_HPP

#include "parser-types.hpp"

#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "../internal/DataSource.hpp"
#include "../Service.hpp"

namespace RTT
{
    namespace scripting
    {

        /**
         * Parsers and executes any RTT script you throw at it.
         */
        class ScriptParser
        {
            // Our task we are parsing in :
            TaskContext* context;
            // The task that will execute it:
            ExecutionEngine* caller;
            // The storage for top-level variables
            Service::shared_ptr storage;
            our_pos_iter_t& mpositer;
            our_pos_iter_t saveStartPos;
            // running offset
            int ln_offset;

            rule_t production;
            rule_t statement;
            rule_t program;
            rule_t function;
            rule_t statemachine;

            CommonParser* commonparser;
            StateGraphParser* stateparser;
            ProgramGraphParser* programparser;

            void clear();

            void seenstatement();
            void seenprogram();
            void seenfunction();
            void seenstatemachine();

        public:
            /**
             * Initialize with an initial position, the TaskContext in which context to
             * parse the scripts and the TaskContext that will call the parse()
             * functions.
             */
            ScriptParser(iter_t& positer, TaskContext* tc, ExecutionEngine* caller);
            ~ScriptParser();

            /**
             * Parses and executes the script from begin to end.
             * The script must be complete and well formed.
             * @throw parse_exception or one of its variants in case of
             * wrong input
             */
            void parse(iter_t& begin, iter_t end);

            /**
             * Parses and executes a (possibly) incomplete script.
             * @return a valid datasource which contains the result
             * of the last statement parsed. It does not need to be
             * evaluate()'ed anymore to read its contents. In case
             * the input is incomplete, a null data source is returned.
             * @throw parse_exception or one of its variants in case of
             * wrong input
             */
            base::DataSourceBase::shared_ptr parseExcerpt(iter_t& begin,
                    iter_t end);

        };
    }
}

#endif
