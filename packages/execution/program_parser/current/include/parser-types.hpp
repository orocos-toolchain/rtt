#ifndef PARSER_TYPES_HPP
#define PARSER_TYPES_HPP

#include <boost/spirit.hpp>

namespace ORO_Execution
{
  class ProgramGraph;
  class VertexNode;
  class EdgeCondition;
  class Processor;
}

namespace ORO_CoreLib
{
  class PropertyBag;
  class PropertyBase;
  class CommandInterface;
  class ConditionInterface;
}

namespace ORO_Execution
{
  class ParseContext;
  class ExpressionParser;
  class ParsedValueBase;
  class ArgumentsParser;
  class TypeInfo;
  class TypeInfoRepository;
  class GlobalFactory;
  class DataSourceBase;

  using ORO_CoreLib::PropertyBag;
  using ORO_CoreLib::PropertyBase;
  using ORO_CoreLib::CommandInterface;
  using ORO_CoreLib::ConditionInterface;

  using namespace boost::spirit;

  typedef std::string our_buffer_t;
  typedef our_buffer_t::iterator our_iterator_t;
  typedef position_iterator<our_iterator_t> our_pos_iter_t;
  // this is the iterator type that the parsers have to work with.  It
  // might change some time in the future..
  typedef our_pos_iter_t iter_t;

  // we need to know what type the skip parser will be in order to be
  // able to know what types the scanner and rule will be exactly.
  // So we have to put a typedef here, and in order to not put the skip
  // parser definition in two places, we put it here as a macro, and use
  // the macro at the appropriate places..

  // we support shell/perl ( "# comment\n" ), C ( non-nested "/*
  // comment */" ) and C++ ( "// comment\n" ) style comments.
  // These are skipped at the scanner level, by using the standard
  // Boost.Spirit skip_iteration_policy.

#   define SKIP_PARSER \
      ( comment_p( "#" ) | comment_p( "//" ) | \
        comment_p( "/*", "*/" ) | ( space_p - ch_p( '\n' ) ) )

  // a macro using GCC's C++ extension typeof that is used to not have
  // to specify impossibly long type names..  See the Boost.Spirit
  // documentation for more details, as that's where I got it from..
#   define RULE( name, def ) \
      typeof( (def) ) name = (def)

  // here are the typedef's for the scanner, and the rule types..
  typedef typeof( SKIP_PARSER ) skip_parser_t;
  typedef skip_parser_iteration_policy<skip_parser_t> iter_pol_t;
  typedef scanner_policies<iter_pol_t> scanner_pol_t;
  typedef scanner<iter_t, scanner_pol_t> scanner_t;
  typedef rule<scanner_t> rule_t;
  typedef rule<lexeme_scanner<scanner_t>::type > lexeme_rule_t;

}

#endif
