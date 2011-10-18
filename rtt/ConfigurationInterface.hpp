/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  ConfigurationInterface.hpp

                        ConfigurationInterface.hpp -  description
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


#ifndef RTT_CONFIGURATIONINTERFACE_HPP
#define RTT_CONFIGURATIONINTERFACE_HPP

#include <memory>
#include <map>
#include "Attribute.hpp"
#include "internal/DataSources.hpp"
#include "base/DataObjectInterface.hpp"
#include "Property.hpp"
#include "PropertyBag.hpp"

namespace RTT
{
    /**
     * @brief A class for keeping track of Attribute, Constant
     * and Property objects of a TaskContext.
     * It allows plugins and remote components to browse the attributes and properties of a TaskContext.
     * @ingroup Services
     * @ingroup RTTComponentInterface
     * @ingroup CoreLibProperties
     */
    class RTT_API ConfigurationInterface
    {
    public:

        /**
         * Create an empty ConfigurationInterface.
         */
        ConfigurationInterface();
        ~ConfigurationInterface();

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
         * Adds a variable of any type as read/write attribute to the configuration interface.
         * An Alias is created which causes contents of the \a attr
         * variable always to be in sync
         * with the contents of the attribute object in the interface.
         * @param name The name of this attribute
         * @param attr The variable that will be aliased.
         */
        template<class T>
        bool addAttribute( const std::string& name, T& attr) {
            if ( !chkPtr("addAttribute", name, &attr) ) return false;
            Alias a(name, new internal::ReferenceDataSource<T>(attr));
            return this->addAttribute( a );
        }

        /**
         * Adds an existing attribute object to the configuration interface.
         * @param name The name of this attribute. \a attr will be initialized with this name.
         * @param attr The attribute to add
         * @return the Attribute object by reference, which you can further query or use.
         * @post attr.ready() == true
         */
        template<class T>
        Attribute<T>& addAttribute( const std::string& name, Attribute<T>& attr) {
            if ( !chkPtr("addAttribute", name, &attr) ) return attr;
            if ( !attr.ready() )
                attr = Attribute<T>(name);
            else
                attr.setName(name);
            this->addAttribute( attr );
            assert(attr.ready());
            return attr;
        }

        /**
         * Adds a variable of any type as read-only attribute to the configuration interface.
         * An Alias is created which causes contents of the
         * attribute always to be in sync
         * with the contents of \a cnst, but it can only be read through the interface.
         * @param name The name of this attribute
         * @param cnst The variable that will be aliased.
         */
        template<class T>
        bool addConstant( const std::string& name, const T& cnst) {
            if ( !chkPtr("addConstant", name, &cnst) ) return false;
            Alias a(name, new internal::ConstReferenceDataSource<T>(cnst));
            return this->addAttribute( a );
        }

        /**
         * Adds an existing constant object to the configuration interface.
         * @param name The name of this constant. \a cnst will be initialized with this name.
         * @param cnst The constant to add. If cnst has not been initialized, it will
         * be initialized to T(). You can change it later-on using the Constant's API.
         * @return the Constant object by reference, which you can further query or use.
         * @post cnst.ready() == true
         */
        template<class T>
        Constant<T>& addConstant( const std::string& name, Constant<T>& cnst) {
            if ( !chkPtr("addConstant", name, &cnst) ) return cnst;
            if ( !cnst.ready() )
                cnst = Constant<T>(name, T());
            else
                cnst.setName(name);
            this->addConstant( cnst );
            assert(cnst.ready());
            return cnst;
        }

        /**
         * Adds a variable of any type as a property to the configuration interface.
         * A Property is created which causes contents of the
         * property always to be in sync
         * with the contents of \a prop.
         * @param name The name of this property
         * @param prop The variable that will be aliased.
         * @return the Property object by reference, which you can further query or document.
         */
        template<class T>
        Property<T>& addProperty( const std::string& name, T& prop) {
            if ( !chkPtr("addProperty", name, &prop) ) return internal::NA<Property<T>& >::na();
            return this->properties()->addProperty( name, prop );
        }

        /**
         * Adds an existing property object to the configuration interface.
         * @param name The name of this property. \a prop will be initialized with this name.
         * @param prop The property to add
         * @return the Property object by reference, which you can further query or document.
         * @post prop.ready() == true
         */
        template<class T>
        Property<T>& addProperty( const std::string& name, Property<T>& prop) {
            if ( !chkPtr("addProperty", name, &prop) ) return prop;
            if ( !prop.ready() )
                prop = Property<T>(name);
            else
                prop.setName(name);
            this->properties()->addProperty( prop );
            assert(prop.ready());
            return prop;
        }

        /**
         * Add an base::AttributeBase which remains owned by the
         * user. This is a low-level function that can be used
         * if you already created an Attribute object that does
         * not belong yet to a service.
         *
         * @param a remains owned by the user, and becomes
         * served by the repository.
         */
        bool addAttribute( base::AttributeBase& a )
        {
            if ( !chkPtr("addAttribute", "AttributeBase", &a) ) return false;
            return a.getDataSource() ? setValue( a.clone() ) : false;
        }

        /**
         * Retrieve a Attribute by name. Returns zero if
         * no Attribute by that name exists.
         * @example
           Attribute<double> d_attr = getAttribute("Xval");
           @endexample
         * @see addAttribute to add an Attribute.
         */
        base::AttributeBase* getAttribute( const std::string& name ) const
        {
            return this->getValue( name );
        }

        /**
         * Remove an attribute from the repository.
         */
        void removeAttribute( const std::string& name );

        /**
         * Add a Constant with a given value.
         * @see getConstant
         */
        bool addConstant( base::AttributeBase& c)
        {
            return c.getDataSource() ? setValue( c.clone() ) :  false;
        }

        /**
         * Retrieve a Constant by name. Returns zero if
         * no Constant by that name exists.
         * @example
           Constant<double> d_const = getConstant("Xconst");
           @endexample
         * @see addConstant
         */
        base::AttributeBase* getConstant( const std::string& name ) const
        {
            return this->getValue( name );
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
        bool addProperty( base::PropertyBase& pb );

        /**
         * Remove a previously added Property and associated attribute.
         * @return false if no such property exists.
         */
        bool removeProperty( base::PropertyBase& p );

        /**
         * Get a Property with name \a name.
         *
         * @param  name The name of the property to search for.
         * @return The PropertyBase with this name, zero
         *         if it does not exist.
         */
        base::PropertyBase* getProperty(const std::string& name) const
        {
            return bag.find(name);
        }

        /**
         * Transfer the ownership of an attribute to the repository.
         * @param ab The attribute which becomes owned by this repository.
         * @return false if an Attribute with the same \a name already present.
         */
        bool setValue( base::AttributeBase* ab );

        /**
         * Get a pointer to the attribute with name \a name.  If no such value exists, this method
         * returns 0. It can be used to retrieve added constants or
         * attributes. Both Attribute and Constant
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
         * Return a new copy of this repository with the copy operation semantics.
         * @param instantiate set to true if you want a copy which will upon any future
         * copy return the same DataSources, thus 'fixating' or 'instantiating' the DataSources.
         * @see base::ActionInterface
         * @note this does not copy the properties() within this repository.
         */
        ConfigurationInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& repl, bool instantiate ) const;

        /**
         * Adds these attributes to this repository.
         */
        void loadValues( AttributeObjects const& new_values);

        /**
         * Returns all attributes in this repository.
         */
        AttributeObjects const& getValues() const {
            return values;
        }

        /**
         * Return the names of all attributes in this repository.
         */
        AttributeNames getAttributeNames() const;

        /**
         * Return a bag of all properties.
         */
        PropertyBag* properties();

    protected:
        bool chkPtr(const std::string &where, const std::string& name, const void* ptr);
        typedef std::vector<base::AttributeBase*> map_t;
        map_t values;
        PropertyBag bag;
    };
}

#endif
