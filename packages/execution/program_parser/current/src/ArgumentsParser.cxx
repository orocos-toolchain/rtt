#include "execution/ArgumentsParser.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "execution/ExpressionParser.hpp"
#include "execution/ParseContext.hpp"

namespace ORO_Execution
{
  using boost::bind;

  ArgumentsParser::ArgumentsParser(
    ExpressionParser& p, ParseContext& c, const std::string& o,
    const std::string& m )
    : mparsed( false ), context( c ), expressionparser( p ),
      mobject( o ), mmethod( m )
  {
    BOOST_SPIRIT_DEBUG_RULE( argument );
    BOOST_SPIRIT_DEBUG_RULE( arguments );

    // a series of arguments; a command-separated list of arguments
    // between parentheses..
    arguments = (
      "("
      >> !( argument >> *( ch_p( ',' ) >> argument ) )
      >> ")" )[
        bind( &ArgumentsParser::seenarguments, this ) ];

    // a single argument is just a normal expression..
    argument =
      expressionparser.parser() [
        bind( &ArgumentsParser::seen_arg, this ) ];
  }

  void ArgumentsParser::seen_arg()
  {
    margs.push_back( expressionparser.getResult() );
    margs.back()->ref();
    expressionparser.dropResult();
  }

  void ArgumentsParser::seenarguments()
  {
    mparsed = true;
  };

  ArgumentsParser::~ArgumentsParser()
  {
    // we release our reference to the parsed DataSources..
    std::for_each( margs.begin(), margs.end(),
                   std::mem_fun( &DataSourceBase::deref ) );
    margs.clear();
  };
}
