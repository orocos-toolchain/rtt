/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ValueChangeParser.cxx 

                        ValueChangeParser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"
#include "execution/ValueChangeParser.hpp"

#include "execution/ParseContext.hpp"
#include "execution/ParsedValue.hpp"
#include "execution/Types.hpp"

#include <boost/bind.hpp>

namespace ORO_Execution
{
  using boost::bind;

    namespace {
        assertion<std::string> expect_open("Open brace expected.");
        assertion<std::string> expect_close("Closing brace expected ( or could not find out what this line means ).");
        assertion<std::string> expect_type("Unknown type. Please specify a type.");
        assertion<std::string> expect_expr("Expected a valid expression.");
        assertion<std::string> expect_ident("Expected a valid identifier.");
        assertion<std::string> expect_init("Expected an initialisation value of the value.");
        assertion<std::string> expect_is("Expected an '=' sign.");
    }


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
      >> expect_type( type_name[bind( &ValueChangeParser::seentype, this, _1, _2 ) ])
         // next the name for the constant
      >> expect_ident(commonparser.identifier [ bind( &ValueChangeParser::storedefinitionname, this, _1, _2 ) ])
      >> expect_init( str_p( "=" )
         // and a value to assign to it..
      >> expressionparser.parser() )[
           bind( &ValueChangeParser::seenconstantdefinition, this ) ] );

    aliasdefinition = (
         "alias"
         // the type
         >> expect_type(type_name [ bind( &ValueChangeParser::seentype, this, _1, _2 ) ])
         // next the name for the alias
         >> expect_ident( commonparser.identifier[ bind( &ValueChangeParser::storedefinitionname, this, _1, _2 ) ])
         >> expect_is( ch_p('=') )
         // and a value to assign to it
         >> expect_init( expressionparser.parser() )[ bind( &ValueChangeParser::seenaliasdefinition, this ) ] );

    variabledefinition = (
         "var"
         >> expect_type( type_name [ bind( &ValueChangeParser::seentype, this, _1, _2 ) ])
         >> expect_ident( commonparser.identifier[ bind( &ValueChangeParser::storedefinitionname, this, _1, _2 ) ])
         >> expect_is(ch_p('='))
         >> expect_init( expressionparser.parser() )[bind( &ValueChangeParser::seenvariabledefinition, this ) ] );

    variableassignment = (
         "set"
         >> expect_ident( commonparser.identifier[ bind( &ValueChangeParser::storename, this, _1, _2 ) ] )
         >> !( '[' >> expressionparser.parser() >> ']' )[ bind( &ValueChangeParser::seenindexassignment, this) ] 
         >> expect_is( ch_p( '=' ) )
         >> expect_expr( expressionparser.parser()) )[
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
    if ( index_ds ) {
        try {
            assigncommand = var->assignIndexCommand( index_ds.get(), expr.get() );
        }
        catch( const bad_assignment& e) {
            throw parse_exception("Attempted to assign index of \""+ valuename +"\" with wrong type.");
        }
        if ( !assigncommand )
            throw parse_exception( "Cannot use index with constant, alias or non-indexed value \"" + valuename + "\"." );
        // allow to restart over...
        index_ds = 0;
    } else {
        try {
            assigncommand = var->assignCommand( expr.get(), false );
        }
        catch( const bad_assignment& e )
            {
                throw parse_exception(
                                      "Attempt to assign a value to a variable of a different type." );
            }
        if ( ! assigncommand )
            throw parse_exception( "Cannot set constant or alias \"" + valuename + "\"." );
    }
    assert(assigncommand);
  }

  void ValueChangeParser::seenindexassignment()
  {
    index_ds = expressionparser.getResult();
    expressionparser.dropResult();
    assert(index_ds);
  }

  void ValueChangeParser::reset()
  {
    assigncommand = 0;
    valuename = "";
    type = 0;
    index_ds = 0;
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
