#include "execution/ValueChangeParser.hpp"

#include "execution/ParseContext.hpp"
#include "execution/ParsedValue.hpp"
#include "execution/Types.hpp"

#include <boost/bind.hpp>
#include <geometry/frames.h>

namespace ORO_Execution
{
  using boost::bind;
  using ORO_Geometry::Frame;
  using ORO_Geometry::Vector;
  using ORO_Geometry::Rotation;

  ValueChangeParser::ValueChangeParser( ParseContext& pc )
    : context( pc ), expressionparser( pc )
  {
    BOOST_SPIRIT_DEBUG_RULE( constantdefinition );
    BOOST_SPIRIT_DEBUG_RULE( aliasdefinition );
    BOOST_SPIRIT_DEBUG_RULE( variabledefinition );
    BOOST_SPIRIT_DEBUG_RULE( variableassignment );

    // we can't use commonparser.identifier to parse a type name,
    // because that one is meant to be used for identifier used by the
    // user, and excludes keywords such as "int", "string" etc.
    chset<> identchar( "a-zA-Z-_0-9" );
    RULE( type_name, lexeme_d[ alpha_p >> *identchar ] );

    constantdefinition = (
         "const"
         // the type
      >> type_name [
           bind( &ValueChangeParser::seentype, this, _1, _2 ) ]
         // next the name for the constant
      >> commonparser.identifier [
           bind( &ValueChangeParser::storedefinitionname, this, _1, _2 ) ]
      >> str_p( "=" )
         // and a value to assign to it..
      >> expressionparser.parser() )[
           bind( &ValueChangeParser::seenconstantdefinition, this ) ];

    aliasdefinition = (
         "alias"
         // the type
      >> type_name [
           bind( &ValueChangeParser::seentype, this, _1, _2 ) ]
         // next the name for the alias
      >> commonparser.identifier[
           bind( &ValueChangeParser::storedefinitionname, this, _1, _2 ) ]
      >> '='
         // and a value to assign to it
      >> expressionparser.parser() )[
           bind( &ValueChangeParser::seenaliasdefinition, this ) ];

    variabledefinition = (
         "var"
      >> type_name [
           bind( &ValueChangeParser::seentype, this, _1, _2 ) ]
      >> commonparser.identifier[
           bind( &ValueChangeParser::storedefinitionname, this, _1, _2 ) ]
      >> '='
      >> expressionparser.parser() )[
           bind( &ValueChangeParser::seenvariabledefinition, this ) ];

    variableassignment = (
         "set"
      >> commonparser.identifier[
           bind( &ValueChangeParser::storename, this, _1, _2 ) ]
      >> ( ch_p( '=' ) | "to" )
      >> expressionparser.parser() )[
           bind( &ValueChangeParser::seenvariableassignment, this ) ];

  };

  void ValueChangeParser::seenconstantdefinition()
  {
    DataSourceBase::shared_ptr expr = expressionparser.getResult();
    expressionparser.dropResult();
    ParsedValueBase* var = type->buildConstant();
    context.valueparser.setValue( valuename, var );
    try
    {
      assigncommand = var->assignCommand( expr.get(), true );
    }
    catch( const bad_assignment& e )
    {
      throw parse_exception(
        "Attempt to initialize a constant with a value of a different type." );
    }
    assert( assigncommand );
    type = 0;
  };

  void ValueChangeParser::storedefinitionname( iter_t begin, iter_t end )
  {
    std::string name( begin, end );
    if ( context.valueparser.isDefined( name ) )
      throw parse_exception( "Identifier \"" + name +
                             "\" is already defined." );
    valuename = name;
  };

  void ValueChangeParser::seentype( iter_t begin, iter_t end )
  {
    std::string name( begin, end );
    type = TypeInfoRepository::instance().type( name );
    if ( type == 0 )
      throw parse_exception( "\"" + name + "\" is an unknown type..." );
  };

  void ValueChangeParser::seenaliasdefinition()
  {
    DataSourceBase::shared_ptr expr = expressionparser.getResult();
    expressionparser.dropResult();
    ParsedValueBase* alias;
    alias = type->buildAlias( expr.get() );
    if ( ! alias )
      throw parse_exception(
        "Attempt to define an alias to an expression of a different type." );
    context.valueparser.setValue( valuename, alias );
    assigncommand = 0;
  };

  void ValueChangeParser::seenvariabledefinition()
  {
    ParsedValueBase* var = type->buildVariable();
    context.valueparser.setValue( valuename, var );
    DataSourceBase::shared_ptr expr = expressionparser.getResult();
    expressionparser.dropResult();
    try
    {
      assigncommand = var->assignCommand( expr.get(), true );
    }
    catch( const bad_assignment& e )
    {
      throw parse_exception( "Attempt to initialize a variable with a value "
                             "of a different type." );
    }
    assert( assigncommand );
    type = 0;
  }

  void ValueChangeParser::storename( iter_t begin, iter_t end )
  {
    valuename = std::string( begin, end );
  }

  void ValueChangeParser::seenvariableassignment()
  {
    ParsedValueBase* var = context.valueparser.getValue( valuename );
    if ( !var )
      throw parse_exception( "Variable \"" + valuename + "\" not defined." );
    DataSourceBase::shared_ptr expr = expressionparser.getResult();
    expressionparser.dropResult();
    try {
      assigncommand = var->assignCommand( expr.get(), false );
    }
    catch( const bad_assignment& e )
    {
      throw parse_exception(
        "Attempt to assign a value to a variable of a different type." );
    }
    if ( ! assigncommand )
      throw parse_exception(
        "Cannot set constant or alias \"" + valuename + "\"." );
  }

  void ValueChangeParser::reset()
  {
    assigncommand = 0;
    valuename = "";
    type = 0;
  }

  rule_t& ValueChangeParser::constantDefinitionParser()
  {
    return constantdefinition;
  }

  rule_t& ValueChangeParser::aliasDefinitionParser()
  {
    return aliasdefinition;
  }

  rule_t& ValueChangeParser::variableDefinitionParser()
  {
    return variabledefinition;
  }

  rule_t& ValueChangeParser::variableAssignmentParser()
  {
    return variableassignment;
  };
}
