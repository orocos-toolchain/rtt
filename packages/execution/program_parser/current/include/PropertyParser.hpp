
#ifndef ORO_PROPERTYPARSER_HPP
#define ORO_PROPERTYPARSER_HPP

#include "parser-types.hpp"
#include "CommonParser.hpp"
#include <string>

namespace ORO_Execution 
{
    namespace detail {
    /**
     * Get the property and bag from an invocation path like
     * bag.subbag.prop .
     */
    class PropertyParser
    {
        CommonParser commonparser;
        rule_t propertylocator;
        PropertyBag*  _bag;
        PropertyBase* _property;

        void locateproperty( iter_t begin, iter_t end );
    public:
        PropertyParser();

        /**
         * Change the bag we want to traverse.
         */
        void setPropertyBag( PropertyBag* pb);

        /**
         * After reset, property() == 0 and
         * bag == 0.
         */
        void reset();

        /**
         * The locator tries to go as far as possible in the bag-to-property
         * path and will never throw. \a property() will contain the last
         * valid property found. If zero, nothing was found. If non-zero
         * \a bag() returns the bag type if \a property is a bag, or zero
         * if \a property() is not a bag. Thus \a property() contains the
         * 'name' of the deepest nested \a bag() found.
         */
        rule_t& locator();

        PropertyBase* property() const { return _property; }

        PropertyBag*  bag() const { return _bag; }
    };
}}
            
#endif
