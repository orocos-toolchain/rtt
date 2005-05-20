
#include "execution/PropertyParser.hpp"
#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"
#include "execution/TaskContext.hpp"
#include "execution/parser-types.hpp"

#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include <boost/bind.hpp>

namespace ORO_Execution
{
    using boost::bind;
    using namespace detail;
    using namespace ORO_CoreLib;

    namespace {
        enum PropertyErrors { bag_not_found };
        guard<PropertyErrors> my_guard;

        /**
         * set by locateproperty, read by handle_no_property
         */
        static std::ptrdiff_t advance_on_error = 0;
    }

    error_status<> handle_no_property(scanner_t const& scan, parser_error<PropertyErrors, iter_t>&e )
    {
        //std::cerr<<"Returning accept"<<std::endl;
        // ok, got as far as possible, _property contains the furthest we got.
        scan.first += advance_on_error;
        return error_status<>( error_status<>::accept, advance_on_error );
    }

        PropertyParser::PropertyParser()
            : _bag(0), _property(0)
        {
            BOOST_SPIRIT_DEBUG_RULE( propertylocator );
            // find as far as possible a property without throwing an exception
            // outside our interface
            propertylocator =
                !my_guard
                ( +(commonparser.notassertingidentifier >> ".")[bind( &PropertyParser::locateproperty, this, _1, _2 ) ])
                [ &handle_no_property ];
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
                advance_on_error += end - begin;
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
            
