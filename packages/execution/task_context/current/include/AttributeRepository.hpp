/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  AttributeRepository.hpp 

                        AttributeRepository.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef ATTRIBUTEREPOSITORY_HPP
#define ATTRIBUTEREPOSITORY_HPP

#include <memory>
#include <map>
#include <corelib/Attribute.hpp>
#include <corelib/DataSources.hpp>
#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>

namespace ORO_Execution
{
    /**
     * @brief A class for keeping track of Attribute, ORO_CoreLib::Constant
     * and ORO_CoreLib::Property objects of a TaskContext.
     * It allows to store objects of these types and retrieve this type.
     * It is used by the script parsers to browse the attributes and properties of a TaskContext.
     */
    class AttributeRepository
    {
        typedef std::map<std::string, ORO_CoreLib::AttributeBase*> map_t;
        map_t values;
        ORO_CoreLib::PropertyBag* bag;

        // check the validity of an index
        template< class T>
        struct ArrayIndexChecker
            : public std::binary_function< T, int, bool>
        {
            bool operator()(const T& v, int i ) const
            {
                return i > -1 && i < (int)(v.size());
            }
        };

        template< class T>
        struct ArrayAssignChecker
            : public std::binary_function< T, T, bool>
        {
            bool operator()(const T& , const T& ) const
            {
                return true;
            }
        };
    public:

        /**
         * Create an empty AttributeRepository.
         */
        AttributeRepository();
        ~AttributeRepository();

        typedef std::vector<std::string> AttributeNames;

        /**
         * Erases the whole repository.
         */
        void clear();

        /**
         * Check if an attribute is present.
         * @deprecated by hasAttribute
         */
        bool isDefined( const std::string& name ) const;

        /**
         * Check if an attribute is present.
         */
        bool hasAttribute( const std::string& name ) const;

        /**
         * Check if a property is present.
         */
        bool hasProperty( const std::string& name ) const;

        /**
         * Add a Constant with a given value.
         * @see getConstant
         */
        template<class T>
        bool addConstant( const std::string& name, T value )
        {
            return setValue( name, new ORO_CoreLib::Constant<T>( value ) );
        }

        /**
         * Retrieve a Constant by name. Returns zero if 
         * no Constant<T> by that name exists.
         * Example : getConstant<double>("Xconst")
         * @see addConstant
         */
        template<class T>
        ORO_CoreLib::Constant<T>* getConstant( const std::string& name )
        {
            return dynamic_cast<ORO_CoreLib::Constant<T>*>( this->getValue( name ) );
        }

        /**
         * Add a Attribute with a given value.
         * @see getAttribute
         */
        template<class T>
        bool addAttribute( const std::string& name, T value )
        {
            return setValue( name, new ORO_CoreLib::Attribute<T>( value ));
        }

        /**
         * Add a Attribute with a given value.
         * @see getAttribute
         */
        template<class T>
        bool addAttribute( const std::string& name, std::vector<T> value )
        {
            // return by reference type.
            return setValue( name, new ORO_CoreLib::Attribute<const std::vector<T>&>( new ORO_CoreLib::IndexedValueDataSource<const std::vector<T>&,int,T,ArrayIndexChecker<std::vector<T> >, ArrayAssignChecker<std::vector<T> > >( value )));
        }

        /**
         * Add an ORO_CoreLib::AttributeBase which remains owned by the
         * user.
         */
        bool addAttribute( const std::string& name, ORO_CoreLib::AttributeBase* a )
        {
            return setValue( name, a->clone() );
        }


        /**
         * Add an ORO_CoreLib::Property<T> as an attribute, which then
         * becomes also available as a Attribute<T>. The value of the Property
         * and the Attribute will always be identical.
         * @return false if an attribute with the same name already exists.
         * @see getAttribute, removeProperty
         */
        template<class T>
        bool addProperty( ORO_CoreLib::Property<T>* p ) {
            ORO_CoreLib::AttributeBase* attr =  new ORO_CoreLib::Attribute<T>( p->getAssignableDataSource().get() );
            bool result = setValue( p->getName(), attr );
            if ( result ) {
                if ( bag == 0 )
                    bag = new ORO_CoreLib::PropertyBag();
                bag->add( p );
            } else
                delete attr;
            return result;
        }

        /**
         * Add an ORO_CoreLib::PropertyBase as a property.
         * This is an 'inferior' method compared to the above addProperty, since
         * the property does not become available as an attribute. The Parsers can work
         * a bit better with attributes than with properties, but this may improve in the
         * future.
         * @return false if an attribute or property with the same name already exists.
         * @see removeProperty
         */
        bool addProperty( ORO_CoreLib::PropertyBase* pb );

        /**
         * Remove a previously added Property and associated attribute.
         * @return false if no such property by that name exists.
         */
        bool removeProperty( ORO_CoreLib::PropertyBase* p );

        /**
         * Retrieve a Attribute by name. Returns zero if 
         * no Attribute<T> by that name exists.
         * Example : getAttribute<double>("Xval")
         * @see addAttribute, addProperty
         */
        template<class T>
        ORO_CoreLib::Attribute<T>* getAttribute( const std::string& name )
        {
            return dynamic_cast<ORO_CoreLib::Attribute<T>*>( this->getValue( name ) );
        }

        /**
         * Add a variable to the repository.
         * @return false if \a name already present.
         */
        bool setValue( const std::string& name, ORO_CoreLib::AttributeBase* pc );

        /**
         * Remove a variable to the repository.
         */
        void removeValue( const std::string& name );

        /**
         * Get the value with name \a name.  If no such value exists, this
         * returns 0.
         */
        ORO_CoreLib::AttributeBase* getValue( const std::string& name );

        /**
         * Return a new copy of this repository with the copy operation semantics.
         * @param instantiate set to true if you want a copy which will upon any future
         * copy return the same DataSources, thus 'fixating' or 'instantiating' the DataSources.
         * @see CommandInterface
         * @note this does not copy the properties() within this repository.
         */
        AttributeRepository* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& repl, bool instantiate ) const;

        /**
         * Return the names of all attributes.
         */
        AttributeNames names() const;
          
        /**
         * Return a bag of all properties.
         * @return null if none present.
         */
        ORO_CoreLib::PropertyBag* properties() const;
          
    };
}

#endif
