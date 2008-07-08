/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:52 CEST 2005  PropertyParser.hpp

                        PropertyParser.hpp -  description
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



#ifndef ORO_PROPERTYPARSER_HPP
#define ORO_PROPERTYPARSER_HPP

#include "parser-types.hpp"
#include "CommonParser.hpp"
#include <string>

namespace RTT
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
