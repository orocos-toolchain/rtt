/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  PropertyParser.cxx

                        PropertyParser.cxx -  description
                           -------------------
    begin                : Sat May 21 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "rtt/scripting/PropertyParser.hpp"
#include "rtt/scripting/parser-debug.hpp"
#include "rtt/scripting/parse_exception.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/scripting/parser-types.hpp"

#include "rtt/Property.hpp"
#include "rtt/PropertyBag.hpp"
#include <boost/bind.hpp>

namespace RTT
{
    using boost::bind;
    using namespace detail;
    using namespace boost;


    error_status<> PropertyParser::handle_no_property(scanner_t const& scan, parser_error<PropertyErrors, iter_t>&e )
    {
        //std::cerr<<"Returning accept"<<std::endl;
        // ok, got as far as possible, _property contains the furthest we got.
        return error_status<>( error_status<>::accept, advance_on_error );
    }

    PropertyParser::PropertyParser(CommonParser& cp)
	: commonparser(cp), _bag(0), _property(0)
    {
	BOOST_SPIRIT_DEBUG_RULE( propertylocator );
	// find as far as possible a property without throwing an exception
	// outside our interface
	propertylocator =
	    !my_guard
	    ( +(commonparser.notassertingidentifier >> ".")[boost::bind( &PropertyParser::locateproperty, this, _1, _2 ) ])
	    [ boost::bind(&PropertyParser::handle_no_property, this, _1, _2) ];
    }

    void PropertyParser::setPropertyBag( PropertyBag* bg )
    {
        _bag = bg;
        _property = 0;
        advance_on_error = 0;
    }

    void PropertyParser::reset()
    {
        _property = 0;
        _bag      = 0;
        advance_on_error = 0;
    }

    void PropertyParser::locateproperty( iter_t begin, iter_t end )
    {
        std::string name( begin, end );
        name.erase( name.length() -1  ); // compensate for extra "."

        //std::cerr<< "PropParser: trying: "<< name;
        if ( _bag && _bag->find(name) ) {
            //std::cerr<< " found !" <<std::endl;
            // guaranteed to be non-null :
            PropertyBase* propbase = _bag->find( name );
            Property<PropertyBag>* propbag = dynamic_cast<Property<PropertyBag> *>( propbase );
            if ( propbag ) {
                //std::cerr<< "PropParser: is a bag." <<std::endl;
                // success
                advance_on_error += end.base() - begin.base();
                _bag = &(propbag->set());
                _property = propbase;
            }
            else {
                //std::cerr<< "PropParser: not a bag." <<std::endl;
                throw_(begin, bag_not_found );
                // a property was found, but it was not a bag.
                // Do not Consume the input
            }
            // if _property is not a bag, throw. The next
            // locateproperty will throw or if no more 'subprop.' found,
        }
        else {
            //std::cerr<< " not found !" <<std::endl;
            // do not consume input.
            throw_(begin, bag_not_found );
        }
    }

    rule_t& PropertyParser::locator()
    {
        return propertylocator;
    }
}

