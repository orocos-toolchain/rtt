#include "parser-debug.hpp"
#include "parse_exception.hpp"
#include "ScriptParser.hpp"
#include "CommonParser.hpp"
#include "StateGraphParser.hpp"
#include "ProgramGraphParser.hpp"
#include "../TaskContext.hpp"
#include "../internal/Exceptions.hpp"

#include <iostream>
#include <functional>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/call_traits.hpp>
#include <iostream>
#include <memory>
#include "../internal/mystd.hpp"

namespace RTT
{
    using namespace boost;
    using namespace detail;
    using boost::bind;

    using namespace std;

    namespace {
        assertion<std::string> expect_eof("Invalid input in file.");
        guard<std::string> no_function_guard;

        error_status<> handle_no_function(scanner_t const& scan, parser_error<std::string, iter_t>&e )
        {
            return error_status<>( error_status<>::fail );
        }

    }

    ScriptParser::ScriptParser(iter_t& positer, TaskContext* tc,
            ExecutionEngine* tcaller) :
        context(tc), caller(tcaller), mpositer(positer), ln_offset(0),
                commonparser(new CommonParser), stateparser(
                        new StateGraphParser(mpositer, context, caller,
                                commonparser)), programparser(
                        new ProgramGraphParser(mpositer, tc, tcaller,
                                *commonparser))
    {
        BOOST_SPIRIT_DEBUG_RULE( production );
        BOOST_SPIRIT_DEBUG_RULE( statemachine );
        BOOST_SPIRIT_DEBUG_RULE( program );
        BOOST_SPIRIT_DEBUG_RULE( function );
        BOOST_SPIRIT_DEBUG_RULE( statement );

        production
                = *(
                        statement[bind(&ScriptParser::seenstatement, this)]
                        | statemachine[bind(&ScriptParser::seenstatemachine, this)]
                        | program[bind(&ScriptParser::seenprogram, this)]
                        | no_function_guard(function[bind(&ScriptParser::seenfunction, this)])[&handle_no_function]
                        )
                        >> expect_eof(end_p);

        statemachine = stateparser->parser();
        program = programparser->programParser();
        function = programparser->functionParser();
        statement = programparser->statementParser();

        // prepare parser to parse statements right away:
        programparser->initBodyParser("script", tc->provides(),
                mpositer.get_position().line);
    }

    void ScriptParser::seenstatement()
    {
        cout << "Seen statement." << endl;

        ProgramInterfacePtr ret = programparser->bodyParserResult();
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
        cout << "Executed statement." << endl;
        programparser->initBodyParser("script", context->provides(),
                mpositer.get_position().line);
    }

    void ScriptParser::seenprogram()
    {
        cout << "Seen program." << endl;
    }

    void ScriptParser::seenfunction()
    {
        cout << "Seen function." << endl;
    }

    void ScriptParser::seenstatemachine()
    {
    }

    void ScriptParser::parse(iter_t& begin, iter_t end)
    {
        //skip_parser_t skip_parser = SKIP_PARSER;
        //iter_pol_t iter_policy( skip_parser );
        //#define SKIP_PARSER
        iter_pol_t iter_policy((comment_p("#") | comment_p("//") | comment_p(
                "/*", "*/") | (space_p - eol_p) | commonparser->skipper));
        scanner_pol_t policies(iter_policy);
        scanner_t scanner(begin, end, policies);

        ln_offset = mpositer.get_position().line - 1;

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
        delete programparser;
        delete stateparser;
        delete commonparser;
    }

    void ScriptParser::clear()
    {

    }

}
