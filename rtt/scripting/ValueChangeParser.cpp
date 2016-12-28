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

#include "parser-debug.hpp"
#include "parse_exception.hpp"
#include "ValueChangeParser.hpp"

#include "../Service.hpp"
#include "../types/Types.hpp"
#include "../Attribute.hpp"
#include "../TaskContext.hpp"
#include "../types/GlobalsRepository.hpp"
//#include "DumpObject.hpp"

#include <boost/bind.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>

#include <Logger.hpp>

#include <sstream>
#include <iostream>

namespace RTT
{
    using namespace detail;

    using namespace std;

    using namespace boost;

    namespace {
        boost::spirit::classic::assertion<std::string> expect_open("Open brace expected.");
        boost::spirit::classic::assertion<std::string> expect_close("Closing brace expected (or could not find out what this line means).");
        boost::spirit::classic::assertion<std::string> expect_type("Unknown type. Please specify a type.");
        boost::spirit::classic::assertion<std::string> expect_def("Expected a type definition. Please specify a type.");
        boost::spirit::classic::assertion<std::string> expect_expr("Expected a valid expression.");
        boost::spirit::classic::assertion<std::string> expect_ident("Expected a valid identifier.");
        boost::spirit::classic::assertion<std::string> expect_init("Expected an initialisation value of the variable.");
        boost::spirit::classic::assertion<std::string> expect_cis("Expected a initialisation ('=') of const.");
        boost::spirit::classic::assertion<std::string> expect_ais("Expected a initialisation ('=') of alias.");
        boost::spirit::classic::assertion<std::string> expect_index("Expected an index: [index].");
        boost::spirit::classic::assertion<std::string> expect_integer("Expected a positive integer expression.");
        boost::spirit::classic::assertion<std::string> expect_change("Expected a variable assignment after 'set'.");
        boost::spirit::classic::assertion<std::string> expect_decl("Expected a declaration list.");
    }


    ValueChangeParser::ValueChangeParser( TaskContext* pc, CommonParser& cp, Service::shared_ptr storage, ExecutionEngine* caller )
        : type( 0 ), context( pc ), mstore( storage ? storage : pc->provides() ),
          expressionparser( pc, caller, cp ), commonparser(cp), sizehint(-1),
          typerepos( TypeInfoRepository::Instance() )
    {
        BOOST_SPIRIT_DEBUG_RULE( constantdefinition );
        BOOST_SPIRIT_DEBUG_RULE( aliasdefinition );
        BOOST_SPIRIT_DEBUG_RULE( variabledefinition );
        BOOST_SPIRIT_DEBUG_RULE( paramdefinition );
        BOOST_SPIRIT_DEBUG_RULE( baredefinition );
        BOOST_SPIRIT_DEBUG_RULE( constdecl );
        BOOST_SPIRIT_DEBUG_RULE( vardecl );
        BOOST_SPIRIT_DEBUG_RULE( baredecl );

  // a macro using GCC's C++ extension typeof that is used to not have
  // to specify impossibly long type names..  See the Boost.Spirit
  // documentation for more details, as that's where I got it from..
  // we use __typeof__ instead of typeof because it is not disabled by
  // using gcc -ansi

  //TODO: this typeof replaced by boost header might not work.
#   define RULE( name, def ) \
        boost_spirit::contiguous<boost_spirit::positive<boost_spirit::chset<char> > > name = (def)
      //BOOST_TYPE_OF(( (def) ) name = (def)
  // typeof is not a native c/c++ construct and is gcc specific
  //__typeof__( (def) ) name = (def)

        // we can't use commonparser.identifier to parse a type name,
        // because that one is meant to be used for identifier used by the
        // user, and excludes keywords such as "int", "string" etc.
        chset<> identchar( "a-zA-Z-_0-9/<>." );
        RULE( type_name, lexeme_d[ +identchar ] );

        valuechange_parsers =  constantdefinition
            | variabledefinition
            | aliasdefinition;

        constantdefinition =
            keyword_p("const")
            // the type
                >> expect_type( type_name[boost::bind( &ValueChangeParser::seentype, this, _1, _2 ) ])
                >> constdecl[boost::bind( &ValueChangeParser::seenconstantdefinition, this )]
                >> *(ch_p(',') >> constdecl[boost::bind( &ValueChangeParser::seenconstantdefinition, this )] );


        aliasdefinition =
            keyword_p("alias")
            // the type
                >> expect_type(type_name [ boost::bind( &ValueChangeParser::seentype, this, _1, _2 ) ])
            // next the name for the alias
                >> expect_ident( commonparser.identifier[ boost::bind( &ValueChangeParser::storedefinitionname, this, _1, _2 ) ])
                >> expect_ais( ch_p('=') )
            // and a value to assign to it
                >> expect_init( expressionparser.parser() )[ boost::bind( &ValueChangeParser::seenaliasdefinition, this ) ];

        variabledefinition =
            keyword_p("var")
                >> expect_type( type_name[boost::bind( &ValueChangeParser::seentype, this, _1, _2 ) ])
                >> vardecl[boost::bind( &ValueChangeParser::seenvariabledefinition, this ) ]
                >> *(ch_p(',') >> vardecl[boost::bind( &ValueChangeParser::seenvariabledefinition, this ) ] );

        paramdefinition =
            keyword_p("param")
                >> expect_type( type_name[boost::bind( &ValueChangeParser::seentype, this, _1, _2 ) ])
                >> baredecl[boost::bind( &ValueChangeParser::seenbaredefinition, this ) ]
                >> *(ch_p(',') >> baredecl[boost::bind( &ValueChangeParser::seenbaredefinition, this ) ] );

        baredefinition =
            type_name[ boost::bind( &ValueChangeParser::seentype, this, _1, _2 )]
                >> baredecl[boost::bind( &ValueChangeParser::seenbaredefinition, this )];

        baredecl =
            expect_ident( commonparser.identifier[ boost::bind( &ValueChangeParser::storedefinitionname, this, _1, _2 )] )
                >> !( ch_p('(') >> expect_integer( expressionparser.parser()[boost::bind( &ValueChangeParser::seensizehint, this)]) >> expect_close( ch_p(')')) ) ;

        vardecl =
            baredecl >> !( ch_p('=') >> expect_init( expressionparser.parser() ) );

        constdecl =
            baredecl >> expect_cis( ch_p('=') ) >> expect_init( expressionparser.parser() );

    }

    void ValueChangeParser::seensizehint()
    {
        DataSourceBase::shared_ptr expr = expressionparser.getResult();
        expressionparser.dropResult();
        assert( expr.get() );
        //assert( !expressionparser.hasResult() );
        DataSource<int>::shared_ptr i = dynamic_cast<DataSource<int>* >( expr.get() );
        std::string typen = type->getTypeName();
        if ( i.get() == 0 ) {
            this->cleanup();
            throw parse_exception_semantic_error
                ("Attempt to initialize "+typen+" "+valuename+" with a "+expr->getTypeName()+", expected an integer expression. Use ' = "+expr->getTypeName()+"( arg )' instead to use the constructor." );
        }
        if ( i->get() < 0 ) {
            std::stringstream value;
            value << i->get();
            this->cleanup();
            throw parse_exception_semantic_error
                ("Attempt to initialize "+typen+" "+valuename+" with an expression leading to a negative number "+value.str()
                 +". Initialization expressions are evaluated once at parse time !" );
        }
        if ( i->get() == 0 ) {
            Logger::log() << Logger::Warning <<
                "Attempt to initialize "<<typen<<" "<<valuename<<" with an expression leading to zero (0)"
                          <<". Initialization expressions are evaluated once at parse time !" << Logger::endl;
        }
        sizehint = i->get();
    }

    void ValueChangeParser::seenconstantdefinition()
    {
        DataSourceBase::shared_ptr expr = expressionparser.getResult();
        expressionparser.dropResult();
        //assert( !expressionparser.hasResult() );
        AttributeBase* var;
        if (sizehint == -1 )
            var = type->buildConstant(valuename, expr);
        else {
            var = type->buildConstant(valuename, expr, sizehint);
        }
        if ( var == 0 ) // bad assignment.
            {
                Logger::log() << " failed !"<<Logger::endl;
                this->cleanup();
                throw parse_exception_semantic_error
                    ("Attempt to initialize a const "+type->getTypeName()+" with a "+expr->getTypeName()+"." );
            }

        mstore->setValue( var );
        definedvalues.push_back( var );
        definednames.push_back( valuename );
        alldefinednames.push_back( valuename );
    }

    void ValueChangeParser::storedefinitionname( iter_t begin, iter_t end )
    {
        std::string name( begin, end );
        if ( mstore->getValue( name ) ) {
            this->cleanup();
            throw parse_exception_semantic_error( "Identifier \"" + name +
                                                  "\" is already defined in " + mstore->getName() );
        }

        valuename = name;
    }

    void ValueChangeParser::seentype( iter_t begin, iter_t end )
    {
        std::string name( begin, end );
        type = typerepos->type( name );
        if ( type == 0 )
            throw parse_exception_semantic_error( "\"" + name + "\" is an unknown type..." );
    }

    void ValueChangeParser::seenaliasdefinition()
    {
        DataSourceBase::shared_ptr expr = expressionparser.getResult();
        expressionparser.dropResult();
        //assert( !expressionparser.hasResult() );
        AttributeBase* alias;
        alias = type->buildAlias( valuename, expr.get() );
        if ( ! alias ) {
            this->cleanup();
            throw parse_exception_semantic_error(
                                                 "Attempt to define an alias of type "+type->getTypeName()+" to an expression of type "+expr->getTypeName()+"." );
        }
        mstore->setValue( alias );
        definedvalues.push_back( alias );
        definednames.push_back( valuename );
        alldefinednames.push_back( valuename );
        ActionInterface* nc(0);
        assigncommands.push_back( nc );
    }

    void ValueChangeParser::seenbaredefinition()
    {
        // type has been stored by calling 'seentype'
        // valuename has been stored by calling 'storename'
        AttributeBase* var;
        if (sizehint == -1 )
            var = type->buildVariable(valuename);
        else {
            var = type->buildVariable(valuename,sizehint);
        }
        sizehint = -1;
        mstore->setValue( var );
        definedvalues.push_back( var );
        definednames.push_back( valuename );
        alldefinednames.push_back( valuename );
    }

    void ValueChangeParser::seenvariabledefinition()
    {
        // build type.
        AttributeBase* var;
        if (sizehint == -1 )
            var = type->buildVariable(valuename);
        else {
            var = type->buildVariable(valuename,sizehint);
        }
        sizehint = -1;
        mstore->setValue( var );
        definedvalues.push_back( var );
        definednames.push_back( valuename );
        alldefinednames.push_back( valuename );

        if ( expressionparser.hasResult() ) {
            DataSourceBase::shared_ptr expr = expressionparser.getResult();
            ConditionInterface* cond = expressionparser.getCmdResult();
            expressionparser.dropResult();
            //assert( !expressionparser.hasResult() );
            try {
                ActionInterface* ac = var->getDataSource()->updateAction( expr.get() );
                assert(ac);
                assigncommands.push_back( ac );
                if (cond)
                    conditions.push_back(cond);
            }
            catch( const bad_assignment& ) {
                this->cleanup();
                throw parse_exception_semantic_error
                    ( "Attempt to initialize a var "+var->getDataSource()->getTypeName()+" with a "+ expr->getTypeName() + "." );
            }
        }
    }

    void ValueChangeParser::store(Service::shared_ptr o)
    {
        if (!o)
            return;
        for(std::vector<std::string>::iterator it = alldefinednames.begin();
            it != alldefinednames.end(); ++it) {
            // only add new values (which didn't come from 'load')
            if ( o->getValue( *it ) == 0 ) {
                o->setValue( mstore->getValue(*it)->clone() );
                //cout << "Storing var "+*it+" from " << mstore->getName() << " into " << o->getName() <<endl;
            }
        }
    }

    void ValueChangeParser::load(Service::shared_ptr s)
    {
        if (!s)
            return;
        vector<string> predefinednames = s->getAttributeNames();
        for(std::vector<std::string>::iterator it = predefinednames.begin();
            it != predefinednames.end(); ++it) {
            if (mstore->getValue(*it) == 0) {
                //cout << "Loading new var "+*it+" from " << s->getName() << " into " << mstore->getName() <<endl;
                mstore->setValue( s->getValue(*it)->clone() );
                alldefinednames.push_back( *it ); // such that it is also again removed from mstore !
            }
        }
    }

    void ValueChangeParser::cleanup()
    {
        for_each(assigncommands.begin(), assigncommands.end(), boost::lambda::bind(boost::lambda::delete_ptr(), boost::lambda::_1));
    }

    void ValueChangeParser::clear()
    {
        assigncommands.clear();
        conditions.clear();

        definedvalues.clear();

        definednames.clear();
    }

    void ValueChangeParser::reset()
    {
        // erase/delete added values from the context:
        for(std::vector<std::string>::iterator it = alldefinednames.begin();
            it != alldefinednames.end(); ++it) {
            mstore->removeValue( *it );
        }
        alldefinednames.clear();
        this->cleanup();
        this->clear();
        valuename = "";
        type = 0;
        sizehint = -1;
    }

    rule_t& ValueChangeParser::parser()
    {
        return valuechange_parsers;
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

    rule_t& ValueChangeParser::paramDefinitionParser()
    {
        return paramdefinition;
    }

    rule_t& ValueChangeParser::bareDefinitionParser()
    {
        return baredefinition;
    }
}
