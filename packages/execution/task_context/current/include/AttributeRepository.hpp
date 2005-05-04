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
#include "TaskAttribute.hpp"

namespace ORO_Execution
{
    class TaskAttributeBase;

    /**
     * @brief A class for keeping track of TaskAttribute, TaskConstant, TaskProperty
     * and ORO_CoreLib::Property objects of a TaskContext.
     * It allows to store objects of these types and retrieve this type.
     * It is used by the script parsers to browse the attributes and properties of a TaskContext.
     */
    class AttributeRepository
    {
        typedef std::map<std::string, TaskAttributeBase*> map_t;
        map_t values;
        ORO_CoreLib::PropertyBag* bag;
    public:

        /**
         * Create an empty AttributeRepository.
         */
        AttributeRepository();
        ~AttributeRepository();

        /**
         * Erases the whole repository.
         */
        void clear();

        /**
         * Check if a value is present.
         */
        bool isDefined( const std::string& name ) const;

        /**
         * Add a TaskConstant with a given value.
         * @see getConstant
         */
        template<class T>
        bool addConstant( const std::string& name, T value )
        {
            return setValue( name, new TaskConstant<T>( value ) );
        }

        /**
         * Retrieve a TaskConstant by name. Returns zero if 
         * no TaskConstant<T> by that name exists.
         * Example : getConstant<double>("Xconst")
         * @see addConstant
         */
        template<class T>
        TaskConstant<T>* getConstant( const std::string& name )
        {
            return dynamic_cast<TaskConstant<T>*>( this->getValue( name ) );
        }

        /**
         * Add a TaskAttribute with a given value.
         * @see getAttribute
         */
        template<class T>
        bool addAttribute( const std::string& name, T value )
        {
            return setValue( name, new TaskAttribute<T>( value ));
        }

        /**
         * Add an ORO_CoreLib::Property<T> as an attribute, which then
         * becomes also available as a TaskAttribute<T>. The value of the Property
         * and the TaskAttribute will always be identical.
         * @see getAttribute, removeProperty
         */
        template<class T>
        bool addProperty( ORO_CoreLib::Property<T>* p ) {
            TaskAttributeBase* attr =  new TaskAttribute<T>( p->createDataSource() );
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
         * Remove a previously added Property and associated attribute.
         */
        bool removeProperty( ORO_CoreLib::PropertyBase* p );

        /**
         * Retrieve a TaskAttribute by name. Returns zero if 
         * no TaskAttribute<T> by that name exists.
         * Example : getAttribute<double>("Xval")
         * @see addAttribute, addProperty
         */
        template<class T>
        TaskAttribute<T>* getAttribute( const std::string& name )
        {
            return dynamic_cast<TaskAttribute<T>*>( this->getValue( name ) );
        }

#if 0
        Most likely to be removed, since duplicate functionality + not compiling
        /**
         * Add a TaskProperty, which will become accessible as both
         * ORO_CoreLib::Property and TaskAttribute.
         * @see getProperty, getAttribute
         */
        template<class T>
        bool addProperty( TaskProperty<T>* p ) {
            return setValue( p->getName(), p);
        }

        /**
         * Retrieve a TaskProperty by name. Returns zero if 
         * no TaskProperty<T> by that name exists.
         * Example : getProperty<double>("Xprop")
         * @see addProperty
         */
        template<class T>
        TaskProperty<T>* getProperty( const std::string& name )
        {
            return dynamic_cast<TaskProperty<T>*>( this->getValue( name ) );
        }
#endif

        /**
         * Add a variable to the repository.
         * @return false if \a name already present.
         */
        bool setValue( const std::string& name, TaskAttributeBase* pc );

        /**
         * Remove a variable to the repository.
         */
        void removeValue( const std::string& name );

        /**
         * Get the value with name name.  If no such value exists, this
         * returns 0.
         */
        TaskAttributeBase* getValue( const std::string& name );

        /**
         * Return a new copy of this repository with the copy operation semantics.
         * @see CommandInterface
         */
        AttributeRepository* copy( std::map<const DataSourceBase*, DataSourceBase*>& repl ) const;

        /**
         * Return a list of all attributes.
         */
        std::vector<std::string> attributes() const;
          
        /**
         * Return a bag of all properties.
         * @return null if none present.
         */
        ORO_CoreLib::PropertyBag* properties() const;
          
    };
}

#endif
