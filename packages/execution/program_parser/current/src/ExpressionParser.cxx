#include "execution/ExpressionParser.hpp"

#include "execution/ParseContext.hpp"
#include "execution/ArgumentsParser.hpp"
#include "execution/Operators.hpp"
#include "execution/ParsedValue.hpp"
#include "execution/DataSourceCondition.hpp"
#include "execution/GlobalFactory.hpp"
#include "execution/DataSourceFactory.hpp"

#include "corelib/ConditionDuration.hpp"

#include <geometry/frames.h>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

namespace ORO_Execution
{
  using boost::bind;
  using ORO_CoreLib::ConditionDuration;

  using ORO_Geometry::Vector;
  using ORO_Geometry::Rotation;

  DataCallParser::DataCallParser( ExpressionParser& p, ParseContext& c )
    : expressionparser( p ), context( c )
  {
    BOOST_SPIRIT_DEBUG_RULE( datacall );
    BOOST_SPIRIT_DEBUG_RULE( arguments );

    // this parser uses a neat boost.spirit trick to avoid keeping
    // loads of stacks for all parsing data ( this parser needs to be
    // reentrant because it can be called while parsing an argument of
    // a datacall, which has itself been called while parsing an
    // argument of a datacall... ).  Boost.Spirit allows you to change
    // the parser that a rule points to at runtime, so we only create
    // the parser just before it's going to be used, when we know what
    // arguments we want..  See the ArgumentsParser doc for more
    // details..
    datacall = (
         lexeme_d[
              commonparser.lexeme_identifier[
                bind( &DataCallParser::seenobjectname, this, _1, _2 ) ]
           >> "."
           >> commonparser.lexeme_identifier[
                bind( &DataCallParser::seenmethodname, this, _1, _2 ) ]
           ][ bind( &DataCallParser::seendataname, this ) ]
      >> !arguments
      )[ bind( &DataCallParser::seendatacall, this ) ];
  };

  void DataCallParser::seendataname()
  {
    const GlobalDataSourceFactory& gdsf =
      context.scriptableextension->dataFactory();
    // get hold of the DataSourceFactory for the object being
    // called..
    const DataSourceFactory* fact = gdsf.factory( mobject );
    if ( !fact )
      throw parse_exception( "Object \"" + mobject + "\"not registered." );
    if ( ! fact->hasData( mmethod ) )
      throw parse_exception( "No match for method \"" + mmethod +
                             "\" on object \"" + mobject + "\"." );
    // create an argument parser for the call..
    ArgumentsParser* argspar =
      new ArgumentsParser( expressionparser, context,
                           mobject, mmethod );
    // we no longer need these two..
    mobject.clear();
    mmethod.clear();

    // keep hold of the argspar, we're still going to need it after
    // it's done its work..  ( in seendatacall(), that is.. )
    argparsers.push( argspar );

    // set the arguments parser to the parser provided by the
    // ArgumentsParser we just created..
    arguments = argspar->parser();
  };

  void DataCallParser::seendatacall()
  {
    ArgumentsParser* argspar = argparsers.top();
    argparsers.pop();
    std::string obj = argspar->objectname();
    std::string meth = argspar->methodname();

    const GlobalDataSourceFactory& gdsf =
      context.scriptableextension->dataFactory();
    const DataSourceFactory* fact = gdsf.factory( obj );
    // we already checked for the existence of this object and method
    // in seendataname()..
    assert( fact );
    assert( fact->hasData( meth ) );
    PropertyBagOwner argsspec( fact->getArgumentSpec( meth ) );
    bool needargs = !argsspec.bag.getProperties().empty();
    if ( ! argspar->parsed() && needargs )
      throw parse_exception(
        "Call to \"" + obj + "." + meth + "\" requires arguments" );

    try
    {
      ret = fact->create( meth, argspar->result() );
    }
    catch( const wrong_number_of_args_exception& e )
    {
      throw parse_exception(
        "Wrong number of arguments in call to \"" + obj + "." + meth + "\": " +
        boost::lexical_cast<std::string>( e.wanted ) + " needed." );
    }
    catch( const wrong_types_of_args_exception& e )
    {
      throw parse_exception(
        "Wrong type of arg provided for argument " +
        boost::lexical_cast<std::string>( e.whicharg ) + " in call to \"" +
        obj + "." + meth + "\"." );
    }
    catch( ... )
    {
      assert( false );
    };
    assert( ret.get() );
  };

  DataCallParser::~DataCallParser()
  {
    // if argparsers is not empty, then something went wrong during
    // the parsing ( someone threw an exception ), and we're
    // responsible for cleaning up the argparsers we created..
    while ( ! argparsers.empty() )
    {
      delete argparsers.top();
      argparsers.pop();
    };
  };

  ExpressionParser::ExpressionParser( ParseContext& pc )
    : context( pc ), datacallparser( *this, pc )
  {
    BOOST_SPIRIT_DEBUG_RULE( expression );
    BOOST_SPIRIT_DEBUG_RULE( unarynotexp );
    BOOST_SPIRIT_DEBUG_RULE( unaryminusexp );
    BOOST_SPIRIT_DEBUG_RULE( unaryplusexp );
    BOOST_SPIRIT_DEBUG_RULE( multexp );
    BOOST_SPIRIT_DEBUG_RULE( divexp );
    BOOST_SPIRIT_DEBUG_RULE( modexp );
    BOOST_SPIRIT_DEBUG_RULE( plusexp );
    BOOST_SPIRIT_DEBUG_RULE( minusexp );
    BOOST_SPIRIT_DEBUG_RULE( smallereqexp );
    BOOST_SPIRIT_DEBUG_RULE( smallerexp );
    BOOST_SPIRIT_DEBUG_RULE( greatereqexp );
    BOOST_SPIRIT_DEBUG_RULE( greaterexp );
    BOOST_SPIRIT_DEBUG_RULE( equalexp );
    BOOST_SPIRIT_DEBUG_RULE( notequalexp );
    BOOST_SPIRIT_DEBUG_RULE( orexp );
    BOOST_SPIRIT_DEBUG_RULE( andexp );
    BOOST_SPIRIT_DEBUG_RULE( ifthenelseexp );
    BOOST_SPIRIT_DEBUG_RULE( frameexp );
    BOOST_SPIRIT_DEBUG_RULE( vectorexp );
    BOOST_SPIRIT_DEBUG_RULE( rotexp );
    BOOST_SPIRIT_DEBUG_RULE( groupexp );
    BOOST_SPIRIT_DEBUG_RULE( atomicexpression );
    BOOST_SPIRIT_DEBUG_RULE( constructorexp );
    BOOST_SPIRIT_DEBUG_RULE( framector );
    BOOST_SPIRIT_DEBUG_RULE( vectorctor );
    BOOST_SPIRIT_DEBUG_RULE( rotationctor );
    BOOST_SPIRIT_DEBUG_RULE( time_expression );
    BOOST_SPIRIT_DEBUG_RULE( time_spec );

    expression = ifthenelseexp;

    // We parse expressions without regard of the types.  First we
    // parse the expressions, then we worry about whether what the
    // user says is actually valid.  You can try to add up two
    // booleans, and the parser will parse it, but it will notice
    // you're writing bogus when it tries to pass it to an operator
    // structure from Operators.hpp

    // TODO: implement the ifthenelse operator ?
    ifthenelseexp = andexp;
    andexp =
      orexp >> *( ( str_p( "&&" ) | "and" ) >> orexp[
                    bind( &ExpressionParser::seen_binary, this, "&&" ) ] );
    orexp =
      notequalexp >> *( ( str_p( "||" ) | "or" ) >> notequalexp[
                        bind( &ExpressionParser::seen_binary, this, "||" ) ] );
    notequalexp =
      equalexp >> *( "!=" >> equalexp[
                       bind( &ExpressionParser::seen_binary, this, "!=" ) ] );
    equalexp =
         greatereqexp
      >> *( "==" >> greatereqexp[
              bind( &ExpressionParser::seen_binary, this, "==" ) ] );
    greatereqexp =
         greaterexp
      >> *( ">=" >> greaterexp[
              bind( &ExpressionParser::seen_binary, this, ">=" ) ] );
    greaterexp =
         smallereqexp
      >> *( '>' >> smallereqexp[
              bind( &ExpressionParser::seen_binary, this, ">" ) ] );
    smallereqexp =
         smallerexp
      >> *( "<=" >> smallerexp[
              bind( &ExpressionParser::seen_binary, this, "<=" ) ] );
    smallerexp =
      minusexp >> *( '<' >> minusexp[
                       bind( &ExpressionParser::seen_binary, this, "<" ) ] );
    minusexp =
      plusexp >> *( '-' >> plusexp[
                      bind( &ExpressionParser::seen_binary, this, "-" ) ] );
    plusexp =
      modexp >> *( '+' >> modexp[
                     bind( &ExpressionParser::seen_binary, this, "+" ) ] );
    modexp =
      divexp >> *( '%' >> divexp[
                     bind( &ExpressionParser::seen_binary, this, "%" ) ] );
    divexp =
      multexp >> *( '/' >> multexp[
                      bind( &ExpressionParser::seen_binary, this, "/" ) ] );
    multexp =
         unaryplusexp
      >> *( '*' >> unaryplusexp[
              bind( &ExpressionParser::seen_binary, this, "*" ) ] );
    unaryplusexp =
        '+' >> unaryminusexp[
          bind( &ExpressionParser::seen_unary, this, "+" ) ]
      | unaryminusexp;
    unaryminusexp =
        '-' >> unarynotexp[
          bind( &ExpressionParser::seen_unary, this, "-" ) ]
      | unarynotexp;
    unarynotexp =
        ( str_p( "not" ) | '!' ) >> atomicexpression[
            bind( &ExpressionParser::seen_unary, this, "!" ) ]
      | atomicexpression;

    atomicexpression =
        // either a property of a component
        datacallparser.parser()[
          bind( &ExpressionParser::seendatacall, this ) ]
        // or a constant or user-defined value..
      | context.valueparser.parser()[
          bind( &ExpressionParser::seenvalue, this ) ]
        // or a 'constructor' call
      | constructorexp
        // or a parenthesis group.
      | groupexp
      | time_expression;

    constructorexp =
        framector
      | vectorctor
      | rotationctor;

    framector = (
         str_p( "frame" )
      >> '('
      >> expression
      >> ','
      >> expression
      >> ')' )[ bind( &ExpressionParser::seen_binary, this, "framevr" ) ];

    vectorctor = (
         str_p( "vector" )
      >> '('
      >> expression
      >> ','
      >> expression
      >> ','
      >> expression
      >> ')' )[ bind( &ExpressionParser::seen_ternary, this, "vectorxyz" ) ];

    rotationctor = (
         str_p( "rotation" )
      >> '('
      >> expression
      >> ','
      >> expression
      >> ','
      >> expression
      >> ')' )[ bind( &ExpressionParser::seen_ternary, this,
                      "rotationeuler" ) ];

    // needs no semantic action, its result is already on top of
    // the stack, where it should be..
    groupexp = '(' >> expression >> ')';

    // a time_condition.  This will result in a ConditionDuration
    // being output as a DataSource<bool>.  The format looks like:
    // "time > time_spec" where time is the literal word "time", ">"
    // can optionally be replaced by ">=" with the normal difference
    // in meaning, and time_spec is an amount of time, specified in
    // seconds ( "123s" ), milliseconds ( "123ms" ), microseconds(
    // "123us" ) or nanoseconds ( "123ns" ).
    time_expression = "time" >> ( str_p( ">=" ) | ">" ) >> time_spec;

    time_spec = lexeme_d[
      uint_p[ bind( &ExpressionParser::seentimespec, this, _1 ) ] >>
      ( str_p( "s" ) | "ms" | "us" | "ns" )[
        bind( &ExpressionParser::seentimeunit, this, _1, _2 ) ] ];
  };

  void ExpressionParser::seentimeunit( iter_t begin, iter_t )
  {
    // the string starting at begin, ending at end is either ms, us,
    // ns or s, so we only need to check the first letter...
    switch( *begin )
    {
    case 's': nsecs *= 1000000000;
    case 'm': nsecs *= 1000000;
    case 'u': nsecs *= 1000;
    };
    DataSourceBase* dsb = new DataSourceCondition(
      new ConditionDuration( nsecs ) );
    dsb->ref();
    parsestack.push( dsb );
  }

  void ExpressionParser::seentimespec( int n )
  {
    nsecs = n;
  }

  void ExpressionParser::seenvalue()
  {
    DataSourceBase* ds = context.valueparser.lastParsed()->toDataSource();
    ds->ref();
    parsestack.push( ds );
  };

  void ExpressionParser::seendatacall()
  {
    DataSourceBase* n = datacallparser.getParseResult();
    n->ref();
    parsestack.push( n );
  };

  ExpressionParser::~ExpressionParser()
  {
    // if parsestack is not empty, then something went wrong, someone
    // threw an exception, so we clean up..
    while ( !parsestack.empty() )
    {
      parsestack.top()->deref();
      parsestack.pop();
    }
  };

  rule_t& ExpressionParser::parser()
  {
    return expression;
  };

  DataSourceBase* ExpressionParser::getResult()
  {
    assert( !parsestack.empty() );
    return parsestack.top();
  };

  void ExpressionParser::seen_unary( const std::string& op )
  {
    DataSourceBase::shared_ptr arg( parsestack.top() );
    // we still have a reference to this DataSource that we took when
    // we pushed it up the parsestack..
    arg->deref();
    parsestack.pop();
    DataSourceBase* ret =
      OperatorRegistry::instance().applyUnary( op, arg.get() );
    if ( ! ret )
      throw parse_exception( "Cannot apply unary operator \"" + op +
                             "\" to value." );
    ret->ref();
    parsestack.push( ret );
  };

  void ExpressionParser::seen_binary( const std::string& op )
  {
    DataSourceBase::shared_ptr arg1( parsestack.top() );
    parsestack.pop();
    DataSourceBase::shared_ptr arg2( parsestack.top() );
    parsestack.pop();

    // we still have a reference to these, that we took when
    // we pushed it up the parsestack..
    arg1->deref();
    arg2->deref();

    // Arg2 is the first (!) argument, as it was pushed on the stack
    // first.
    DataSourceBase* ret =
      OperatorRegistry::instance().applyBinary( op, arg2.get(), arg1.get() );
    if ( ! ret )
      throw parse_exception( "Cannot apply binary operator \"" + op +
                             "\" to value." );
    ret->ref();
    parsestack.push( ret );
  };

  void ExpressionParser::seen_ternary( const std::string& op )
  {
    DataSourceBase::shared_ptr arg1( parsestack.top() );
    parsestack.pop();
    DataSourceBase::shared_ptr arg2( parsestack.top() );
    parsestack.pop();
    DataSourceBase::shared_ptr arg3( parsestack.top() );
    parsestack.pop();

    // we still have a reference to these, that we took when
    // we pushed it up the parsestack..
    arg1->deref();
    arg2->deref();
    arg3->deref();

    // Arg3 is the first (!) argument, as it was pushed on the stack
    // first.
    DataSourceBase* ret =
      OperatorRegistry::instance().applyTernary( op, arg3.get(),
                                                 arg2.get(), arg1.get() );
    if ( ! ret )
      throw parse_exception( "Cannot apply ternary operator \"" + op +
                             "\" to value." );
    ret->ref();
    parsestack.push( ret );
  };

  void ExpressionParser::dropResult()
  {
    parsestack.top()->deref();
    parsestack.pop();
  }
}
