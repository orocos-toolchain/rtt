/***************************************************************************
  tag: Peter Soetens Fri Oct 22 20:28:53 2010 +0200 ScriptParser.hpp

                        ScriptParser.hpp -  description
                           -------------------
    begin                : Fri Oct 22 2010
    copyright            : (C) 2010 Peter Soetens
    email                : peter@thesourceworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


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
            std::string program_text;

            rule_t production;
            rule_t statement;
            rule_t program;
            rule_t function;
            rule_t statemachine;

            CommonParser* commonparser;
            StateGraphParser* stateparser;
            ProgramGraphParser* programparser;
            ProgramGraphParser* statementparser;

            void clear();

            void seenstatement();
            void seenprogram();
            void seenfunction();
            void seenstatemachine();

            error_status<> handle_no_function(scanner_t const& scan, parser_error<std::string, iter_t>&e );

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
