/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  PropertyBag.cxx 

                        PropertyBag.cxx -  description
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

#pragma implementation
#include "corelib/PropertyBag.hpp"
#include "corelib/Property.hpp"

namespace ORO_CoreLib
{

    PropertyBase* find(const PropertyBag& bag, const std::string& nameSequence, const std::string& separator)
    {
        PropertyBase* result;
        Property<PropertyBag>*  result_bag;
        std::string token;
        std::string::size_type start = 0;
        if ( separator.length() != 0 && nameSequence.find(separator) == 0 ) // detect 'root' attribute
            start = separator.length();
        std::string::size_type len = nameSequence.find(separator, start);
        if (len != std::string::npos) {
            token = nameSequence.substr(start,len-start);
            start = len + separator.length();      // reset start to next token.
            if ( start >= nameSequence.length() )
                start = std::string::npos;
        }
        else {
            token = nameSequence.substr(start);
            start = std::string::npos; // do not look further.
        }
        result = bag.find(token);
        if (result != 0 ) // get the base with this name
        {
            result_bag = dynamic_cast<Property<PropertyBag>*>(result);
            if ( result_bag != 0 && start != std::string::npos ) {
                return find( result_bag->get(), nameSequence.substr( start ), separator );// a bag so search recursively
            }
            else
                return result; // not a bag, so it is a result.
        }
        return 0; // failure
    }

    void refreshProperties(PropertyBag& target, const PropertyBag& source)
    {
        //iterate over source, update PropertyBases
        PropertyBag::const_iterator it( source.getProperties().begin() );
        while ( it != source.getProperties().end() )
        {
            PropertyBase* mine = target.find( (*it)->getName() );
            if (mine != 0)
            {
                //std::cout <<"*******************refresh"<<std::endl;
                mine->update( (*it) );
            }
            ++it;
        }
    }

    void refreshProperty(PropertyBag& target, const PropertyBase& source)
    {
        PropertyBase* target_prop;
        // dynamic_cast ?
        if ( 0 != (target_prop = target.find( source.getName() ) ) )
            {
                target_prop->update( &source );
            }
    }

    void copyProperties(PropertyBag& target, const PropertyBag& source)
    {
        // Make a full deep copy.
        //iterate over source, update or clone PropertyBases
        PropertyBag::const_iterator it( source.getProperties().begin() );
        while ( it != source.getProperties().end() )
        {
            PropertyBase* mine = target.find( (*it)->getName() );
            // This is the mistake, the clone created a 'symlink' to the
            // exiting one and we copy into the original instead of 
            // a clone().
            if (mine != 0)
                mine->copy( (*it) );  // no need to make new one, just copy over existing one.
            else
            {
                // step 1 : clone a new instance (non deep copy)
                PropertyBase* temp = (*it)->create();  // 1. XXX bag refers to props
                // step 2 : deep copy clone with original.
                temp->copy( *it );                     // 2. XXX make copy in refers !
                // step 3 : add result to target bag.
                target.add( temp );
            }
            ++it;
        }
    }

    void deleteProperties(PropertyBag& target)
    {
        //iterate over target, delete PropertyBases
        PropertyBag::const_iterator it( target.getProperties().begin() );
        while ( it != target.getProperties().end() )
        {
            delete (*it);
            ++it;
        }
        target.clear();
    }

    void flattenPropertyBag(PropertyBag& target, const std::string& separator)
    {
        //iterate over target, recursively flatten bags.
        Property<PropertyBag>* result;
        PropertyBag::const_iterator it( target.getProperties().begin() );
        while ( it != target.getProperties().end() )
        {
            result = dynamic_cast< Property<PropertyBag>* >( *it );
            if ( result != 0 )
            {
                flattenPropertyBag( result->value(), separator );// a bag so flatten recursively
                // copy all elements from result to target.
                PropertyBag::const_iterator flat_it( result->value().getProperties().begin() ) ;
                if ( flat_it != result->value().getProperties().end() )
                {
                    while (flat_it != result->value().getProperties().end() )
                    {
                        (*flat_it)->setName( result->getName() + separator + (*flat_it)->getName() );
                        target.add( *flat_it );
                        result->value().remove( *flat_it );
                        flat_it =  result->value().getProperties().begin();
                    }
                    it = target.getProperties().begin(); // reset iterator
                    continue;                            // do not increase it
                } 
                // the bag is empty now, but it must stay in target.
            }
            ++it;
        }
    }

}
