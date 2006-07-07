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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "corelib/PropertyBag.hpp"
#include "corelib/Property.hpp"
#include "corelib/Logger.hpp"

namespace ORO_CoreLib
{

    PropertyBag::PropertyBag( )
        : mproperties(), type("type_less")
    {}

    PropertyBag::PropertyBag( const std::string& _type)
        : mproperties(), type(_type)
    {}

    PropertyBag::PropertyBag( const PropertyBag& orig)
        : mproperties( orig.getProperties() ), type( orig.getType() )
    {
    }

    void PropertyBag::add(PropertyBase *p)
    {
        this->addProperty(p);
    }

    void PropertyBag::remove(PropertyBase *p)
    {
        this->removeProperty(p);
    }

    bool PropertyBag::addProperty(PropertyBase *p)
    {
        mproperties.push_back(p);
        return true;
    }

    bool PropertyBag::removeProperty(PropertyBase *p)
    {
        iterator i = mproperties.begin();
        i = mproperties.end();
        i = std::find(mproperties.begin(), mproperties.end(), p);
        if ( i != mproperties.end() ) {
            mproperties.erase(i);
            return true;
        }
        return false;
    }

    void PropertyBag::clear()
    {
        mproperties.clear();
    }

    void PropertyBag::list(std::vector<std::string> &names) const
    {
        for (
             const_iterator i = mproperties.begin();
             i != mproperties.end();
             i++ )
            {
                names.push_back( (*i)->getName() );
            }
    }

    std::vector<std::string> PropertyBag::list() const
    {
        std::vector<std::string> names;
        for (
             const_iterator i = mproperties.begin();
             i != mproperties.end();
             i++ )
            {
                names.push_back( (*i)->getName() );
            }
        return names;
    }

    void PropertyBag::identify( PropertyIntrospection* pi ) const
    {
        for ( const_iterator i = mproperties.begin();
              i != mproperties.end();
              i++ )
            {
                (*i)->identify(pi);
            }
    }

    void PropertyBag::identify( PropertyBagVisitor* pi ) const
    {
        for ( const_iterator i = mproperties.begin();
              i != mproperties.end();
              i++ )
            {
                (*i)->identify(pi);
            }
    }

    PropertyBase* PropertyBag::find(const std::string& name) const
    {
        const_iterator i( std::find_if(mproperties.begin(), mproperties.end(), std::bind2nd(PropertyBag::FindProp(), name ) ) );
        if ( i != mproperties.end() )
            return ( *i );
        return 0;
    }

    PropertyBag& PropertyBag::operator=(const PropertyBag& orig)
    {
        mproperties.clear();

        const_iterator i = orig.getProperties().begin();
        while (i != orig.getProperties().end() )
            {
                add( (*i) );
                ++i;
            }
        this->setType( orig.getType() );
        return *this;
    }

    PropertyBag& PropertyBag::operator<<=(const PropertyBag& source)
    {
        //iterate over orig, update or clone PropertyBases
        const_iterator it(source.getProperties().begin());
        while ( it != source.getProperties().end() )
            {
                PropertyBase* mine = find( (*it)->getName() );
                if (mine != 0)
                    remove(mine);
                add( (*it) );
                ++it;
            }
        this->setType( source.getType() );
        return *this;
    }

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

    bool refreshProperties(const PropertyBag& target, const PropertyBag& source)
    {
        //iterate over source, update PropertyBases
        PropertyBag::const_iterator it( source.getProperties().begin() );
        while ( it != source.getProperties().end() )
        {
            PropertyBase* mine = target.find( (*it)->getName() );
            if (mine != 0)
            {
                //std::cout <<"*******************refresh"<<std::endl;
                if ( mine->refresh( (*it) ) == false ) {
                    Logger::log() << Logger::Error;
                    Logger::log() << "refreshProperties: Could not refresh Property "
                                  << mine->getType() << " "<< (*it)->getName()
                                  << ": type mismatch, can not update with type "
                                  << (*it)->getType() << Logger::endl;
                    return false;
                }
            }
            ++it;
        }
        return true;
    }

    bool refreshProperty(const PropertyBag& target, const PropertyBase& source)
    {
        PropertyBase* target_prop;
        // dynamic_cast ?
        if ( 0 != (target_prop = target.find( source.getName() ) ) )
            {
                return target_prop->refresh( &source );
            }
        return false;
    }

    bool copyProperties(PropertyBag& target, const PropertyBag& source)
    {
        // Make a full deep copy.
        //iterate over source, clone all PropertyBases
        PropertyBag::const_iterator it( source.getProperties().begin() );
        while ( it != source.getProperties().end() )
        {
            // step 1 : clone a new instance (non deep copy)
            PropertyBase* temp = (*it)->create();
            // step 2 : deep copy clone with original.
            temp->copy( *it );
            // step 3 : add result to target bag.
            target.add( temp );
            ++it;
        }
        return true;
    }

    bool updateProperties(PropertyBag& target, const PropertyBag& source)
    {
        // check type consistency...
        // this was probably intended to convert old xml formats to new ones.
        // but deleting in target seems just very wrong...
#if 0
        if ( target.getType() != source.getType() ) {
            // if different types, discard the old contents and
            // put in the new contents...
            // update type info...
            // delete old type contents...
            flattenPropertyBag(target);
            deleteProperties(target);
            // now continue 'updating'
        }
#endif

        target.setType( source.getType() );

        // Make an updated if present, create if not present
        //iterate over source, update or clone PropertyBases
        PropertyBag::const_iterator it( source.getProperties().begin() );
        while ( it != source.getProperties().end() )
        {
            PropertyBase* mine = target.find( (*it)->getName() );
            if (mine != 0) {
#ifndef NDEBUG
                Logger::log() << Logger::Debug;
                Logger::log() << "updateProperties: updating Property "
                              << (*it)->getType() << " "<< (*it)->getName()
                              << "." << Logger::endl;
#endif
                  // no need to make new one, just update existing one
                if ( mine->update( (*it) ) == false ) {
                    // try composition:
                    if ( mine->getTypeInfo()->composeType( (*it)->getDataSource(), mine->getDataSource() ) == false ) {
                        Logger::log() << Logger::Error;
                        Logger::log() << "updateProperties: Could not update, nor compose Property "
                                      << mine->getType() << " "<< (*it)->getName()
                                      << ": type mismatch, can not update with type "
                                      << (*it)->getType() << Logger::endl;
                        return false;
                    }
                }
                // ok.
            }
            else
            {
#ifndef NDEBUG
                Logger::log() << Logger::Debug;
                Logger::log() << "updateProperties: created Property "
                              << (*it)->getType() << " "<< (*it)->getName()
                              << "." << Logger::endl;
#endif
                // step 1 : clone a new instance (non deep copy)
                PropertyBase* temp = (*it)->create();
                // step 2 : deep copy clone with original, will never fail.
                temp->update( (*it) );
                // step 3 : add result to target bag.
                target.add( temp );
            }
            ++it;
        }
        return true;
    }

    void deleteProperties(PropertyBag& target)
    {
        //recursive delete.
        PropertyBag::const_iterator it( target.getProperties().begin() );
        while ( it != target.getProperties().end() )
        {
            delete (*it);
            ++it;
        }
        target.clear();
    }

    void deletePropertyBag(PropertyBag& target)
    {
        //iterate over target, delete PropertyBases
        PropertyBag::const_iterator it( target.getProperties().begin() );
        while ( it != target.getProperties().end() )
        {
            Property<PropertyBag>* result = dynamic_cast< Property<PropertyBag>* >( *it );
            if ( result != 0 )
                deletePropertyBag( result->value() );
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
