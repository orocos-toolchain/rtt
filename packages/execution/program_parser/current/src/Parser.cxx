#include "execution/Parser.hpp"
#include "execution/parser-common.hpp"
#include "execution/ProgramParser.hpp"
#include "execution/ConditionParser.hpp"
#include "corelib/ConditionInterface.hpp"

#include <iostream>

using namespace boost;

namespace ORO_Execution
{

  Program* Parser::parseProgram( std::istream& s, Processor* proc,
                                 const GlobalFactory* ext )
  {
    our_buffer_t program;

    s.unsetf( std::ios_base::skipws );

    std::istream_iterator<char> streambegin( s );
    std::istream_iterator<char> streamend;
    std::copy( streambegin, streamend, std::back_inserter( program ) );

    our_pos_iter_t parsebegin( program.begin(), program.end(), "teststring" );
    our_pos_iter_t parseend;

    // The internal parser.
    ProgramParser gram( parsebegin, proc, ext );
    Program* ret = gram.parse( parsebegin, parseend );
    if ( ret )
      std::cerr << "Program Parsed Successfully !" << std::endl;
    return ret;
  };

  ConditionInterface* Parser::parseCondition( std::string& s,
                                              const GlobalFactory* e )
  {
    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend;

    // TODO: get a processor from somewhere, even though the condition
    // parser doesn't really need it..
    ParseContext pc( 0, e );
    ConditionParser parser( pc );
    try
    {
      parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
    }
    catch( const parse_exception& e )
    {
      std::cerr << "Parse error at line "
                << parsebegin.get_position().line
                << ": " << e.what() << std::endl;
      return 0;
    };
    ConditionInterface* ret = parser.getParseResult();
    parser.reset();
    return ret;
  };
}
