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

#include "execution/TaskContext.hpp"
#include "execution/Types.hpp"

#include <boost/bind.hpp>

namespace ORO_Execution
{
  using boost::bind;

    namespace {
        assertion<std::string> expect_open("Open brace expected.");
        assertion<std::string> expect_close("Closing brace expected (or could not find out what this line means).");
        assertion<std::string> expect_type("Unknown type. Please specify a type.");
        assertion<std::string> expect_def("Expected a type definition. Please specify a type.");
        assertion<std::string> expect_expr("Expected a valid expression.");
        assertion<std::string> expect_ident("Expected a valid identifier.");
        assertion<std::string> expect_init("Expected an initialisation value of the variable.");
        assertion<std::string> expect_is("Expected an '=' sign.");
        assertion<std::string> expect_index("Expected an index: [index].");
        assertion<std::string> expect_integer("Expected a positive integer value.");
        assertion<std::string> expect_assign("Expected an assignment after 'set'.");
    }


  ValueChangeParser::ValueChangeParser( TaskContext* pc )
      : assigncommand( 0 ), lastdefinedvalue( 0 ), peername(0),
        type( 0 ), context( pc ), expressionparser( pc ), peerparser( pc ), sizehint(-1)
  {
    BOOST_SPIRIT_DEBUG_RULE( constantdefinition );
    BOOST_SPIRIT_DEBUG_RULE( aliasdefinition );
    BOOST_SPIRIT_DEBUG_RULE( variabledefinition );
    BOOST_SPIRIT_DEBUG_RULE( variableassignment );
    BOOST_SPIRIT_DEBUG_RULE( variablechange );
    BOOST_SPIRIT_DEBUG_RULE( paramdefinition );
    BOOST_SPIRIT_DEBUG_RULE( baredefinition );

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
	 >> expect_ident( commonparser.identifier[ bind( &ValueChangeParser::storedefinitionname, this, _1, _2 ) ])
	 >> expect_is( ch_p('=') )
         // and a value to assign to it..
	 >> expect_init( expressionparser.parser() )[
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
         >> expect_def( baredefinition )
         >> !( (ch_p('=') >> expect_init( expressionparser.parser() )[bind( &ValueChangeParser::seenvariabledefinition, this ) ] )));
    
    variableassignment = 
        "set" >> variablechange;

    variablechange =    
         ( !(peerparser.parser()[ bind( &ValueChangeParser::storepeername, this) ])
         >> expect_ident( commonparser.identifier)[ bind( &ValueChangeParser::storename, this, _1, _2 ) ]
         >> !( '[' >> expect_index( expressionparser.parser() ) >> ']' )[ bind( &ValueChangeParser::seenindexassignment, this) ]
         >> ch_p( '=' )
         >> expect_expr( expressionparser.parser()) )[ bind( &ValueChangeParser::seenvariableassignment, this ) ];

    paramdefinition =
        "param"
        >> expect_def( baredefinition );

    baredefinition =
        ( type_name[ bind( &ValueChangeParser::seentype, this, _1, _2 )]
          >> expect_ident( commonparser.identifier[ bind( &ValueChangeParser::storedefinitionname, this, _1, _2 )] )
          >> !( ch_p('(') >> expect_integer( uint_p[bind( &ValueChangeParser::seensizehint, this, _1)]) >> expect_close( ch_p(')')) ) 
          )[bind( &ValueChangeParser::seenbaredefinition, this )];
  };

    TaskContext* ValueChangeParser::setStack( TaskContext* tc )
    {
        context = tc;
        peerparser.setContext( tc );
        return expressionparser.setStack( tc );
    }

    TaskContext* ValueChangeParser::setContext( TaskContext* tc )
    {
        context = tc;
        peerparser.setContext( tc );
        return expressionparser.setContext( tc );
    }

    void ValueChangeParser::seensizehint(int i)
    {
        sizehint = i;
    }

  void ValueChangeParser::seenconstantdefinition()
  {
    DataSourceBase::shared_ptr expr = expressionparser.getResult();
    expressionparser.dropResult();
    TaskAttributeBase* var;
      if (sizehint == -1 )
          var = type->buildVariable();
      else {
          var = type->buildVariable(sizehint);
      }
    context->attributeRepository.setValue( valuename, var );
    try
    {
      assigncommand = var->assignCommand( expr.get(), true );
    }
    catch( const bad_assignment& e )
    {
      throw parse_exception_semantic_error(
        "Attempt to initialize a const "+var->toDataSource()->getType()+" with a "+expr->getType()+"." );
    }
    assert( assigncommand );
    lastdefinedvalue = var;
    lastparseddefname = valuename;
    type = 0;
  };

  void ValueChangeParser::storedefinitionname( iter_t begin, iter_t end )
  {
    std::string name( begin, end );
    if ( context->attributeRepository.isDefined( name ) )
      throw parse_exception_semantic_error( "Identifier \"" + name +
                                            "\" is already defined." );
    valuename = name;
  };

  void ValueChangeParser::seentype( iter_t begin, iter_t end )
  {
    std::string name( begin, end );
    type = TypeInfoRepository::instance().type( name );
    if ( type == 0 )
      throw parse_exception_semantic_error( "\"" + name + "\" is an unknown type..." );
  };

  void ValueChangeParser::seenaliasdefinition()
  {
    DataSourceBase::shared_ptr expr = expressionparser.getResult();
    expressionparser.dropResult();
    TaskAttributeBase* alias;
    alias = type->buildAlias( expr.get() );
    if ( ! alias ) {
        // build variable to get type info workaround :
        TaskAttributeBase* orig = type->buildVariable();
        std::string tname;
        if (orig)
            tname = orig->toDataSource()->getType();
        delete orig;
        throw parse_exception_semantic_error(
        "Attempt to define an alias of type "+tname+" to an expression of type "+expr->getType()+"." );
    }
    context->attributeRepository.setValue( valuename, alias );
    lastdefinedvalue = alias;
    lastparseddefname = valuename;
    assigncommand = 0;
  };

  void ValueChangeParser::seenbaredefinition()
  {
    // type has been stored by calling 'seentype'
    // valuename has been stored by calling 'storename'
      TaskAttributeBase* var;
      if (sizehint == -1 )
          var = type->buildVariable();
      else {
          var = type->buildVariable(sizehint);
      }
      context->attributeRepository.setValue( valuename, var );
      lastdefinedvalue = var;
      lastparseddefname = valuename;
      type = 0;
  }

    void ValueChangeParser::storename( iter_t begin, iter_t end ) {
        valuename = std::string( begin, end );
    }

  void ValueChangeParser::storepeername()
  {
      peername  = peerparser.peer();
      // the peerparser.object() should contain "this"
      peerparser.reset();
  }

  void ValueChangeParser::seenvariabledefinition()
  {
    TaskAttributeBase* var =
        context->attributeRepository.getValue( valuename );
    DataSourceBase::shared_ptr expr = expressionparser.getResult();
    expressionparser.dropResult();
    try
    {
        assigncommand = var->assignCommand( expr.get(), true );
    }
    catch( const bad_assignment& e )
    {
      throw parse_exception_semantic_error
          ( "Attempt to initialize a var "+var->toDataSource()->getType()+" with a "+ expr->getType() + "." );
    }
    assert( assigncommand );
  }

  void ValueChangeParser::seenvariableassignment()
  {
      TaskAttributeBase* var;
      // is it a peers attribute ?
      if ( peername ) {
          var = peername->attributeRepository.getValue( valuename );
          if ( !var )
              throw parse_exception_semantic_error( "Attribute \"" + valuename + "\" not defined in task '"+peername->getName()+"'." );
      } else {
          var = context->attributeRepository.getValue( valuename );
          if ( !var )
              throw parse_exception_semantic_error( "Value \"" + valuename + "\" not defined in '"+context->getName()+"'." );
      }
    DataSourceBase::shared_ptr expr = expressionparser.getResult();
    expressionparser.dropResult();
    if ( index_ds ) {
        try {
            assigncommand = var->assignIndexCommand( index_ds.get(), expr.get() );
        }
        catch( const bad_assignment& e) {
            throw parse_exception_semantic_error(
                "Impossible to assign "+valuename+"[ "+index_ds->getType()+" ] to value of type "+expr->getType()+".");
        }
        if ( !assigncommand )
            throw parse_exception_semantic_error(
                "Cannot use index with constant, alias or non-indexed value \"" + valuename + "\"." );
        // allow to restart over...
        index_ds = 0;
    } else {
        try {
            assigncommand = var->assignCommand( expr.get(), false );
        }
        catch( const bad_assignment& e )
            {
                throw parse_exception_semantic_error
                    ( "Attempt to assign variable of type "+var->toDataSource()->getType()+" with a "+ expr->getType() + "." );
            }
        if ( ! assigncommand )
            throw parse_exception_semantic_error( "Cannot set constant or alias \"" + valuename + "\"." );
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
    peername = 0;
    sizehint = -1;
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
  }

  rule_t& ValueChangeParser::variableChangeParser()
  {
    return variablechange;
  }

  rule_t& ValueChangeParser::paramDefinitionParser()
  {
    return paramdefinition;
  }

  rule_t& ValueChangeParser::bareDefinitionParser()
  {
    return baredefinition;
  }
}
