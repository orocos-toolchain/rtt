/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  AttributeRepository.hpp

                        AttributeRepository.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
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


#ifndef ATTRIBUTEREPOSITORY_HPP
#define ATTRIBUTEREPOSITORY_HPP

#include <memory>
#include <map>
#include "../Attribute.hpp"
#include "../internal/DataSources.hpp"
#include "../base/DataObjectInterface.hpp"
#include "../Property.hpp"
#include "../PropertyBag.hpp"

namespace RTT
{ namespace interface {

    /**
     * @brief A class for keeping track of Attribute, Constant
     * and Property objects of a TaskContext.
     * It allows to store objects of these types and retrieve this type.
     * It is used by the script parsers to browse the attributes and properties of a TaskContext.
     */
    class RTT_API AttributeRepository
    {
        typedef std::vector<base::AttributeBase*> map_t;
        map_t values;
        /**
         * The bag is only constructed if queried for.
         */
        mutable PropertyBag* bag;

    public:

        /**
         * Create an empty AttributeRepository.
         */
        AttributeRepository();
        ~AttributeRepository();

        /**
         * A vector containing the names of all attribute objects
         * in this repository.
         */
        typedef std::vector<std::string> AttributeNames;

        /**
         * A vector containing pointers to all attribute objects
         * stored in this repository.
         */
        typedef std::vector<base::AttributeBase*> AttributeObjects;

        /**
         * Erases the whole repository.
         */
        void clear();

        /**
         * Check if an attribute is present in this interface.
         */
        bool hasAttribute( const std::string& name ) const;

        /**
         * Add an base::AttributeBase which remains owned by the
         * user.
         * @param a remains owned by the user, and becomes
         * served by the repository.
         */
        bool addAttribute( base::AttributeBase* a )
        {
            return a->getDataSource() && setValue( a->clone() );
        }

        /**
         * Retrieve a Attribute by name. Returns zero if
         * no Attribute<T> by that name exists.
         * @example
           Attribute<double> d_attr = getAttribute<double>("Xval");
           @endexample
         * @see addAttribute to add an Attribute.
         * @see getValue for a template-less variant of this function,
         * which also works.
         */
        template<class T>
        Attribute<T>* getAttribute( const std::string& name ) const
        {
            return dynamic_cast<Attribute<T>*>( this->getValue( name ) );
        }

        /**
         * Remove an attribute from the repository.
         */
        void removeAttribute( const std::string& name );

        /**
         * Add a Constant with a given value.
         * @see getConstant
         */
        bool addConstant( base::AttributeBase* c)
        {
            return c->getDataSource() && setValue( c->clone() );
        }

        /**
         * Retrieve a Constant by name. Returns zero if
         * no Constant<T> by that name exists.
         * @example
           Constant<double> d_const = getConstant<double>("Xconst");
           @endexample
         * @see addConstant
         */
        template<class T>
        Constant<T>* getConstant( const std::string& name ) const
        {
            return dynamic_cast<Constant<T>*>( this->getValue( name ) );
        }

        /**
         * Check if a property is present.
         */
        bool hasProperty( const std::string& name ) const;

        /**
         * Add an base::PropertyBase as a property.
         * @return false if a property with the same name already exists.
         * @see removeProperty
         */
        bool addProperty( base::PropertyBase* pb );

        /**
         * Remove a previously added Property and associated attribute.
         * @return false if no such property by that name exists.
         */
        bool removeProperty( base::PropertyBase* p );

        /**
         * Transfer the ownership of an attribute to the repository.
         * @param ab The attribute which becomes owned by this repository.
         * @return false if an Attribute with the same \a name already present.
         */
        bool setValue( base::AttributeBase* ab );

        /**
         * Get a pointer to the attribute with name \a name.  If no such value exists, this method
         * returns 0. It can be used to retrieve added constants,
         * attributes or data objects. Both Attribute and Constant
         * can work with this function.
         * @example
           Attribute<double> d_attr = getValue("Xval");
           @endexample
         */
        base::AttributeBase* getValue( const std::string& name ) const;

        /**
         * Delete a value added with setValue from the repository.
         */
        bool removeValue(const std::string& name );

        /**
         * Add a DataObject as an Attribute. This is especially useful
         * to add the thread-safe DataObjects as thread-safe attributes.
         * You can retrieve it through getValue().
         * @param doi The DataObject, which remains owned by the user.
         * @return true if doi->getName() is unique within this repository.
         */
        template<class T>
        bool addDataObject( base::DataObjectInterface<T>* doi) {
            return this->setValue( new Alias<T>(doi, doi->getName() ));
        }

        /**
         * Return a new copy of this repository with the copy operation semantics.
         * @param instantiate set to true if you want a copy which will upon any future
         * copy return the same DataSources, thus 'fixating' or 'instantiating' the DataSources.
         * @see base::ActionInterface
         * @note this does not copy the properties() within this repository.
         */
        AttributeRepository* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& repl, bool instantiate ) const;

        /**
         * Adds these attributes to this repository.
         */
        void loadValues( AttributeObjects const& new_values) {
            values.insert(values.end(), new_values.begin(), new_values.end());
        }

        /**
         * Returns all attributes in this repository.
         */
        AttributeObjects const& getValues() const {
            return values;
        }

        /**
         * Return the names of all attributes.
         * @deprecated by getAttributes()
         */
        AttributeNames names() const;

        /**
         * Return the names of all attributes in this repository.
         */
        AttributeNames getAttributes() const;

        /**
         * Return a bag of all properties.
         * @return null if none present.
         * @deprecated Properties should no longer be used as a part of the
         * AttributeRepository, but accessed using TaskContext::properties().
         */
        PropertyBag* properties() const;

    };
}}

#endif
