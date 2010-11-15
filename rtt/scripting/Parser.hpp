/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Parser.hpp

                        Parser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef EXECUTION_PARSER_HPP
#define EXECUTION_PARSER_HPP

#include <iosfwd>
#include <utility>
#include <map>
#include <string>
#include <istream>
#include <vector>
#include "parse_exception.hpp"
#include "../internal/DataSource.hpp"
#include "ProgramInterface.hpp"
#include "ParsedStateMachine.hpp"
#include "rtt-scripting-config.h"
#include "rtt-scripting-fwd.hpp"

namespace RTT
{ namespace scripting {

    /**
     * @brief This class is the public interface to the
     * Orocos Program Parser Framework. It parsers Orocos program scripts,
     * Orocos State Contexts and allows to parse a text Command, Condition or Expression
     * to the respective objects.
     * @see ScriptingAccess for a more user friendly loading of scripts
     * into TaskContexts.
     */
    class RTT_SCRIPTING_API Parser
    {
        ExecutionEngine* mcaller;
    public:
        /**
         * Create a parser and allow to explicitly specify which
         * TaskContext is calling it. This allows to generate the correct
         * asynchonous method invocation code in case caller and parser
         * are not identical.
         */
        Parser(TaskContext* caller = 0);

        /**
         * Runs all statements in \a code.
         * @param code A list of scripting statements and definitions
         * @param mowner The task in which the code must be interpreted
         * @param filename An informational description of the source of \a code.
         * @throw parse_exception and its derivatives.
         */
        void runScript(std::string const& code, TaskContext* mowner, ScriptingService* service, std::string const& filename );

        /**
         * List of parsed functions.
         */
        typedef std::vector< ProgramInterfacePtr > ParsedFunctions;

        /**
         * @brief Reads out the string, parses it, and returns a new @ref
         * FunctionGraph.
         * @throw parse_exception Throws exceptions of type parse_exception.
         */
        ParsedFunctions parseFunction( const std::string& s, TaskContext*, const std::string& filename = "stream" );

        /**
         * List of parsed programs.
         */
        typedef std::vector< ProgramInterfacePtr > ParsedPrograms;

        /**
         * @brief Reads out the string, parses it, and returns a new @ref
         * ProgramGraph.
         * @throw parse_exception Throws exceptions of type parse_exception.
         */
        ParsedPrograms parseProgram( const std::string& s, TaskContext*, const std::string& filename = "stream" );

        /**
         * List of parsed State Machines.
         */
        typedef std::vector< ParsedStateMachinePtr > ParsedStateMachines;

        /**
         * @brief Reads out the string, parses it, and returns a new @ref
         * ParsedStateMachine.
         * @throw file_parse_exception Throws exceptions of type file_parse_exception.
         */
        ParsedStateMachines parseStateMachine(const std::string& s, TaskContext*, const std::string& filename = "stream" );

        /**
         * @brief Parses the string as a condition, and returns a new
         * ConditionInterface.  Will throw parse_exception on
         * failure. Use ext to get the data from the components.
         * @throw parse_exception
         */
        ConditionInterface* parseCondition(const std::string& s, TaskContext* );

        /**
        * @brief Parses the expression in s.
        * @return A base::DataSourceBase which contains the expression.
        * @throw parse_exception Throws exceptions of type parse_exception.
        */
        base::DataSourceBase::shared_ptr
        parseExpression( const std::string&s, TaskContext* );

        /**
        * @brief Parses a change of a value in s.
        * @return A base::DataSourceBase which contains the command to change the value.
        * @throw parse_exception Throws exceptions of type parse_exception.
        */
        base::DataSourceBase::shared_ptr
        parseValueChange( const std::string&s, TaskContext* );

        /**
        * @brief Parses a whole value manipulation/creation statement.
        * Requires the set/var/const etc prefixes.
        * @return A base::DataSourceBase which contains the command to change the value.
        * @throw parse_exception Throws exceptions of type parse_exception.
        */
        base::DataSourceBase::shared_ptr
        parseValueStatement( const std::string&s, TaskContext* );
    };
}}
#endif
