/***************************************************************************
  tag: Peter Soetens Fri Oct 22 20:28:53 2010 +0200 ScriptParser.cpp

                        ScriptParser.cpp -  description
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


#include "rtt/scripting/parser-debug.hpp"
#include "rtt/scripting/parse_exception.hpp"
#include "rtt/scripting/ScriptParser.hpp"
#include "rtt/scripting/CommonParser.hpp"
#include "rtt/scripting/StateGraphParser.hpp"
#include "rtt/scripting/ProgramGraphParser.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/internal/Exceptions.hpp"
#include "rtt/scripting/ScriptingService.hpp"

#include <iostream>
#include <functional>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/call_traits.hpp>
#include <iostream>
#include <memory>
#include "rtt/internal/mystd.hpp"
#include "rtt/internal/GlobalEngine.hpp"
#include "rtt/scripting/ParsedStateMachine.hpp"

namespace RTT
{
    using namespace boost;
    using namespace detail;
    using boost::bind;

    using namespace std;

    namespace {
        boost::spirit::classic::assertion<std::string> expect_eof("Invalid input in file.");
        guard<std::string> no_function_guard;

    }

    error_status<> ScriptParser::handle_no_function(scanner_t const& scan, parser_error<std::string, iter_t>&e )
    {
        // only fail if the parser could not make sense of it, otherwise, rethrow:
        if ( programparser->parserUsed() == false )
            return error_status<>( error_status<>::fail );
        else
            return error_status<>( error_status<>::rethrow );
    }

    ScriptParser::ScriptParser(iter_t& positer, TaskContext* tc,
            ExecutionEngine* tcaller) :
            context(tc), caller(tcaller), storage( Service::Create("stack") ),
            mpositer(positer), ln_offset(0),
            commonparser(new CommonParser),
            // Both state and programs are executed (called) in the target context (see seenprogram, seenstatemachine):
            stateparser(new StateGraphParser(mpositer, context, context->engine(), commonparser)),
            programparser(new ProgramGraphParser(mpositer, context, context->engine(), *commonparser)),
            // statements are directly executed:
            statementparser(new ProgramGraphParser(mpositer, context, caller, *commonparser))
    {
        BOOST_SPIRIT_DEBUG_RULE( production );
        BOOST_SPIRIT_DEBUG_RULE( statemachine );
        BOOST_SPIRIT_DEBUG_RULE( program );
        BOOST_SPIRIT_DEBUG_RULE( function );
        BOOST_SPIRIT_DEBUG_RULE( statement );

        production
                = *(
                        statemachine[bind(&ScriptParser::seenstatemachine, this)]
                        | program[bind(&ScriptParser::seenprogram, this)]
                        | no_function_guard(function[bind(&ScriptParser::seenfunction, this)])[boost::bind(&ScriptParser::handle_no_function,this,_1,_2)]
                        | statement[bind(&ScriptParser::seenstatement, this)]
                        | commonparser->notassertingeos
                        )
                        >> expect_eof(end_p);

        statemachine = stateparser->parser();
        program = programparser->programParser();
        function = programparser->functionParser();
        statement = statementparser->statementParser();

        // prepare parser to parse statements right away:
        programparser->initBodyParser("script", storage, 0);
        statementparser->initBodyParser("script", storage, 0);
                //mpositer.get_position().line);
        stateparser->storeOffset();
    }

    void ScriptParser::seenstatement()
    {
        ProgramInterfacePtr ret = statementparser->bodyParserResult();
        int steps = 0;
        // we execute the result directly.
        ret->loaded( GlobalEngine::Instance() );
        ret->start();
        while (ret->execute() && ret->isRunning())
        {
            if (ret->inError())
            {
                log(Error)
                        << "Script encountered an error during execution of line "
                        << ret->getLineNumber() << endlog();
            }
            ++steps;
            if (steps > 10000)
            {
                log(Error)
                        << "Parser refuses to execute more than 10000 yield statements. Fix your program."
                        << endlog();
                break;
            }
        }
        ret->unloaded();
        statementparser->initBodyParser("script", storage, 0);
                //mpositer.get_position().line);
        stateparser->storeOffset();
    }

    void ScriptParser::seenprogram()
    {
        // Load the programs in the Scripting Service of this component:
        assert( context->provides()->hasService("scripting"));
        ScriptingService::shared_ptr ss = dynamic_pointer_cast<ScriptingService>( context->provides("scripting") );
        assert(ss);
        ProgramInterfacePtr ret = programparser->programParserResult();
        try {
            FunctionGraphPtr func = dynamic_pointer_cast<FunctionGraph>(ret);
            if (func)
                func->setText(program_text);
            log(Info) << "Loading Program '"<< ret->getName() <<"'" <<endlog();
            if ( ss->loadProgram( ret ) == false)
                throw program_load_exception( "Could not load Program '"+ ret->getName() +"' :\n failed to load in ScriptingService.\n");
        } catch (program_load_exception& e ) {
            log(Error) << "Could not load Program '"<< ret->getName() <<"' :" << endlog();
            log(Error) << e.what() << endlog();
            throw;
        }
        programparser->initBodyParser("script", storage, 0);
                //mpositer.get_position().line);
        stateparser->storeOffset();
    }

    void ScriptParser::seenfunction()
    {
        programparser->initBodyParser("script", storage, 0);
                //mpositer.get_position().line);
        stateparser->storeOffset();
    }

    void ScriptParser::seenstatemachine()
    {
        // Load the statemachines in the Scripting Service of this component:
        assert( context->provides()->hasService("scripting"));
        ScriptingService::shared_ptr ss = dynamic_pointer_cast<ScriptingService>( context->provides("scripting") );
        assert(ss);
        ParsedStateMachinePtr ret = stateparser->getParserResult();
        if (ret) {
            try {
                log(Info) << "Loading StateMachine '"<< ret->getName() <<"'" <<endlog();
                ss->loadStateMachine( ret ); // throws load_exception
            } catch (program_load_exception& e ) {
                log(Error) << "Could not load StateMachine'"<< ret->getName() <<"' :" << endlog();
                log(Error) << e.what() << endlog();
                throw;
            }
        }
        programparser->initBodyParser("script", storage, 0);
                //mpositer.get_position().line);
        stateparser->storeOffset();
    }

    void ScriptParser::parse(iter_t& begin, iter_t end)
    {
        //skip_parser_t skip_parser = SKIP_PARSER;
        //iter_pol_t iter_policy( skip_parser );
        //#define SKIP_PARSER
        skip_parser_t skippers = (comment_p("#") | comment_p("//") 
                          | comment_p("/*", "*/") 
                          | (space_p - eol_p) 
			 | (commonparser->skipper));

        iter_pol_t iter_policy(skippers);
        scanner_pol_t policies(iter_policy);
        scanner_t scanner(begin, end, policies);

        ln_offset = mpositer.get_position().line - 1;
        program_text = std::string( begin, end ); // begin is by reference.

        stateparser->storeOffset();
        try
        {
            if (!production.parse(scanner))
            {
                // on error, we clear all remaining data, cause we can't
                // guarantee consistency...
                clear();
                throw file_parse_exception(new parse_exception_syntactic_error(
                        "Syntax error"), mpositer.get_position().file,
                        mpositer.get_position().line,
                        mpositer.get_position().column);
            }
            return;
        } catch (const parser_error<std::string, iter_t>& e)
        {
            // on error, we clear all remaining data, cause we can't
            // guarantee consistency...
            clear();
            throw file_parse_exception(new parse_exception_syntactic_error(
                    e.descriptor), mpositer.get_position().file,
                    mpositer.get_position().line,
                    mpositer.get_position().column);
        } catch (const parse_exception& e)
        {
            // on error, we clear all remaining data, cause we can't
            // guarantee consistency...
            clear();
            throw file_parse_exception(e.copy(), mpositer.get_position().file,
                    mpositer.get_position().line,
                    mpositer.get_position().column);
        } catch (...)
        {
            assert( false );
        }
    }

    ScriptParser::~ScriptParser()
    {
        clear();
        delete statementparser;
        delete programparser;
        delete stateparser;
        delete commonparser;
    }

    void ScriptParser::clear()
    {

    }

}
