/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ValueParser.cxx

                        ValueParser.cxx -  description
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

#include "rtt/parser-debug.hpp"
#include "rtt/parse_exception.hpp"
#include "rtt/ValueParser.hpp"
#include "rtt/Attribute.hpp"

#include "rtt/TaskContext.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
using namespace std;

namespace RTT
{
    using boost::bind;
    using namespace detail;

    ValueParser::ValueParser( TaskContext* tc)
        : peerparser(tc), context(tc)
  {
    BOOST_SPIRIT_DEBUG_RULE( constant );
    BOOST_SPIRIT_DEBUG_RULE( const_double );
    BOOST_SPIRIT_DEBUG_RULE( const_int );
    BOOST_SPIRIT_DEBUG_RULE( const_bool );
    BOOST_SPIRIT_DEBUG_RULE( const_string );
    BOOST_SPIRIT_DEBUG_RULE( named_constant );

    // note the order is important: commonparser.identifier throws a
    // useful "cannot use x as identifier" error if it fails, so we
    // must first show all non-identifier rules.
    constant =
        const_double
      | const_int
      | const_bool
      | const_char
      | const_string
      | named_constant;

    const_double =
      strict_real_p [
        bind( &ValueParser::seendoubleconstant, this, _1 ) ];

    const_int =
      int_p [
        bind( &ValueParser::seenintconstant, this, _1 ) ];

    const_bool =
      ( str_p( "true" ) | "false" )[
        bind( &ValueParser::seenboolconstant, this, _1, _2 ) ];

    const_char = (ch_p('\'') >> ch_p('\\') >> ch_p('0') >> ch_p('\''))[bind( &ValueParser::seennull,this)] |
        confix_p( "'", (c_escape_ch_p[ bind( &ValueParser::seencharconstant, this, _1 ) ]) , "'" );

    const_string = confix_p(
      ch_p( '"' ), *(
        c_escape_ch_p[ bind( &ValueParser::push_str_char, this, _1 ) ]
        ), '"' )[ bind( &ValueParser::seenstring, this ) ];

    named_constant =
        ( str_p("done")[bind( &ValueParser::seennamedconstant, this, _1, _2 ) ]
          |
          ( peerparser.locator()[bind( &ValueParser::seenpeer, this) ]
            >> propparser.locator()
            >> commonparser.identifier[bind( &ValueParser::seennamedconstant, this, _1, _2 ) ]) ) 
        ;
  }

    void ValueParser::seenpeer() {
        // inform propparser of new peer :
//         std::cerr << "ValueParser: seenpeer : "<< peerparser.peer()->getName()
//                   <<" has props :" << (peerparser.peer()->attributes()->properties() != 0) << std::endl;
        propparser.setPropertyBag( peerparser.peer()->attributes()->properties() );
    }

  void ValueParser::seenboolconstant( iter_t begin, iter_t end )
  {
    std::string value( begin, end );
    assert( value == "true" || value == "false" );
    if ( value == "true" )
      ret =
        new ConstantDataSource<bool>( true );
    else
      ret =
        new ConstantDataSource<bool>( false );
  }

  void ValueParser::seennamedconstant( iter_t begin, iter_t end )
  {
    std::string name( begin, end );
    TaskContext* peer = peerparser.peer();
    peerparser.reset();
//     std::cerr << "ValueParser: seenvar : "<< name
//               <<" is bag : " << (propparser.bag() != 0) << " is prop: "<< (propparser.property() != 0) << std::endl;
    if ( propparser.bag() && propparser.property() ) {
        // nested property case :
        if ( ! propparser.bag()->find( name ) ) {
//             std::cerr << "In "<<peer->getName() <<" : " << name << " not present"<<std::endl;
            throw_(begin, "Property " + name + " not present in PropertyBag "+propparser.property()->getName()+" in "+ peer->getName()+".");
        }
        ret = propparser.bag()->find( name )->getDataSource();
        propparser.reset();
        return;
    }

    // non-nested property or attribute case :
    if ( peer->attributes()->hasAttribute( name ) ) {
      ret = peer->attributes()->getValue(name)->getDataSource();
      return;
    }
    if ( peer->attributes()->hasProperty( name ) ) {
        ret = peer->attributes()->properties()->find(name)->getDataSource();
        return;
    }

    //    std::cerr << "In "<<peer->getName() <<" : " << name << " not present"<<std::endl;
//         peerparser.peer()->debug(true);
//         peer->debug(true);
    throw_(begin, "Value " + name + " not defined in "+ peer->getName()+".");
    //      throw parse_exception_undefined_value( name );
  }

    void ValueParser::seennull()
    {
        ret = new ConstantDataSource<char>( '\0' );
    }

    void ValueParser::seencharconstant( iter_t c )
    {
        ret = new ConstantDataSource<char>( *c );
    }

  void ValueParser::seenintconstant( int i )
  {
    ret = new ConstantDataSource<int>( i );
  }

  void ValueParser::seendoubleconstant( double i )
  {
    ret = new ConstantDataSource<double>( i );
  }

  ValueParser::~ValueParser()
  {
    clear();
  }

  void ValueParser::clear()
  {
      propparser.reset();
  }

  rule_t& ValueParser::parser()
  {
    return constant;
  }

  void ValueParser::push_str_char( char c )
  {
    mcurstring += c;
  }

  void ValueParser::seenstring()
  {
    // due to a problem in Boost.spirit, the '"' terminating a
    // string will be in mcurstring, and we don't want it, so we
    // remove it..
    mcurstring.erase( mcurstring.end() - 1 );
    ret = new ConstantDataSource<const std::string&>( mcurstring );
    //deleter.reset( ret );
    mcurstring.clear();
  }
}
