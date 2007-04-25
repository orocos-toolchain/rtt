/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  PropertyBag.hpp

                        PropertyBag.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef PI_PROPERTY_BAG
#define PI_PROPERTY_BAG

#include "PropertyBase.hpp"

#include <vector>
#include <algorithm>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    template< class T>
    class Property;

	/**
	 * @brief A container for holding references to properties.
     *
	 * @section intro Introduction
     *
     * This class groups properties of different types into a single
	 * container. A PropertyBag object can handed to a Marshaller object
	 * which will serialize the contents of the PropertyBag.
     *
     * All operations on a PropertyBag are non recursive. The PropertyBag
     * itself is not aware of the possible hierarchical structure. You need
     * to use the global scope functions for handling hierarchical structures.
     *
     * A PropertyBag is not the owner of the properties within. It defines
     * a group of properties, nothing more. When this group of properties
     * contains another PropertyBag, this bag is considered as a 'pointer'
     * to other properties. Even such a bag is not owned by the PropertyBag.
     *
     * @section constr Constructing PropertyBags
     * It can happen in an application that a PropertyBag is filled with
     * dynamically created Property objects. The bag is not responsible for
     * cleaning up these objects once they are no longer needed. The application
     * which uses the PropertyBag has to find a way of deleting the possible
     * dangling pointers.
     *
     * @warning The elements in a PropertyBag are <em>unordered</em>. Operations on the
     * bag may change the relative order of the elements.
     *
     * @section usage Using PropertyBags
     * To add a Property<ClassT> \a prop to a PropertyBag \a bag, use 
     * @verbatim
     bag.add(&prop).
     @endverbatim
     * To retrieve this property, either use \a find or \a getProperty to retrieve
     * a Property by name :
     * @verbatim
     PropertyBase* pb = bag.find( "name" ).
     Property<ClassT> pb = bag.getProperty<ClassT>( "name" ).
     @endverbatim
     * Both will return null if no such property exists.
	 * @see PropertyBase, Property, BagOperations
     * @ingroup CoreLibProperties
	 */
    class PropertyBag
    {
    public:
        /**
         * The container in which the properties are stored.
         */
        typedef std::vector<PropertyBase*> Properties;
        /**
         * Deprecated, use Properties.
         */
        typedef Properties PropertyContainerType;
        /**
         * An iterator over the Properties.
         */
        typedef Properties::iterator iterator;
        /**
         * A const iterator over the Properties.
         */
        typedef Properties::const_iterator const_iterator;

        /**
         * The container to hold names of Properties.
         */
        typedef std::vector<std::string> Names;
        /**
         * The default constructor.
         */
        PropertyBag();

        /**
         * The typed constructor.
         *
         * @param type The type of PropertyBag.
         */
        PropertyBag( const std::string& _type);

        /**
         * @brief The copy constructor.
         * The copy constructor of the PropertyBag makes
         * non-deep copies of its elements.
         */
        PropertyBag( const PropertyBag& orig);

        /**
         * Add a valid property to the container. Analogous to addProperty.
         * @param p Pointer to the property to be added.
         */
        void add(PropertyBase *p);

        /**
         * Remove a property from the container. Analogous to removeProperty.
         * @param p Pointer to the property to be removed.
         */
        void remove(PropertyBase *p);

        /**
         * Add a valid property to the container.
         * @param p Pointer to the property to be added.
         * @return false if ! p->ready(), true otherwise.
         */
        bool addProperty(PropertyBase *p);

        /**
         * Remove a property from the container.
         * @param p Pointer to the property to be removed.
         */
        bool removeProperty(PropertyBase *p);

        /**
         * Removes all PropertyBases from this bag, without deleting
         * them. This is a not recursive function.
         */
        void clear();

        /**
         * List the names of all properties.
         * @param names The container which will be used to store all
         * available property names in.
         */
        void list(Names &names) const;

        /**
         * List the names of all properties.
         * @return A container of names.
         */
        Names list() const;

        /**
         * Return true if no properties are present in this bag.
         */
        bool empty() const
        {
            return mproperties.empty();
        }

        /**
         * Get a Property<T> with name \a name.
         *
         * @param  T The type of which this property is.
         * @param  name The name of the property to search for.
         * @return The Property<T> with this name, zero
         *         if it does not exist ( no such \a T or no such \a name )
         */
        template<class T>
        Property<T>* getProperty(const std::string& name) const
        {
            const_iterator i( std::find_if(mproperties.begin(), mproperties.end(), std::bind2nd(FindProp(), name ) ) );
            if ( i != mproperties.end() )
                return dynamic_cast< Property<T>* >( *i );
            return 0;
        }

        /**
         * Returns the \a i'th Property, starting from 0.
         * @return zero if \a i is out of range.
         */
        PropertyBase* getItem( int i) const
        {
            if ( i < 0 || i >= int(mproperties.size()) )
                return 0;
            return mproperties[i];
        }

        /**
         * Get the number of Properties in this bag.
         */
        size_t size() const { return mproperties.size(); }

        /**
         * Identify the contents of this bag through introspection.
         */
        void identify( PropertyIntrospection* pi ) const;

        /**
         * Identify the contents of this bag through introspection.
         */
        void identify( PropertyBagVisitor* pi ) const;

        /**
         * Find the PropertyBase with name \a name.
         *
         * @param  name The name of the property to search for.
         * @return The PropertyBase with this name, zero
         *         if it does not exist.
         */
        PropertyBase* find(const std::string& name) const;

        /**
         * This assignment assigns all
         * PropertyBases of another bag in this bag, making
         * an exact copy or the original.
         */
        PropertyBag& operator=(const PropertyBag& orig);

        /**
         * The update operator.
         * It updates this bag so it contains all PropertyBases
         * of another bag, removing own PropertyBases if duplicate names
         * exist in the source bag.
         */
        PropertyBag& operator<<=(const PropertyBag& source);

        /**
         * The 'add' operator.
         * Equivalent to add().
         * @param item The Property to add to this bag.
         */
        PropertyBag& operator<<( PropertyBase* item) { this->add(item); return *this; }

        const std::string& getType() const { return type;}

        void setType(const std::string& newtype) { type = newtype; }

        Properties& getProperties() { return mproperties; }
        const Properties& getProperties() const { return mproperties; }

        iterator begin() { return mproperties.begin(); }
        const_iterator begin() const { return mproperties.begin(); }
        iterator end() { return mproperties.end(); }
        const_iterator end() const { return mproperties.end(); }
    protected:
        Properties mproperties;

        /**
         * A function object for finding a Property by name.
         */
        struct FindProp : public std::binary_function<const PropertyBase*,const std::string, bool>
        {
            bool operator()(const PropertyBase* b1, const std::string& b2) const { return b1->getName() == b2; }
        };

        std::string type;
    };

    /*
     * @defgroup BagOperations Recursive Property Bag Operations.
     *        These functions operate recursively on the contents of bags,
     *        possibly modifying, deleting or creating new Property objects
     *        in the bag or in one of its sub-bags.
     * @ingroup CoreLibProperties
     * @{
     */

    /**
     * This function locates a Property in nested PropertyBags.
     *
     * @param bag The bag to look for a Property.
     * @param path A sequence of names, separated by
     *        \a separator indicating the path in the bag to a property,
     *        omitting the name of the \a bag itself.
     * @param separator The token to separate properties in the \a path,
     * Defaults to ".".
     * @ingroup CoreLibProperties
     */
    PropertyBase* findProperty(const PropertyBag& bag, const std::string& path, const std::string& separator = std::string(".") );

    /**
     * This function refreshes the values of the properties in one PropertyBag with
     * the values of the properties of another PropertyBag.
     * No new properties will be created.
     *
     * You can use this function to update the properties of a fixed bag.
     * @param target The bag in which the properties must be refreshed.
     * @param source The bag containing new values for \a target.
     * @param strict Set to true if each property of \a target must be updated with a property
     * or \a source. If a property from \a target is not present in \a source, this function
     * returns false.
     * @ingroup CoreLibProperties
     */
    bool refreshProperties(const PropertyBag& target, const PropertyBag& source, bool strict=false);

    /**
     * Refresh one Property in the target bag with the new value.
     * @param target The bag in which a property with name source->getName() can be found.
     * @param source A property which contains a new value for a property with the same
     * name in \a target.
     * @ingroup CoreLibProperties
     */
    bool refreshProperty(const PropertyBag& target, const PropertyBase& source);

    /**
     * This function copies (recursively) the Properties of one Bag into
     * another Bag. This may cause duplicate entries in \a target if \a source
     * contains Properties with the same name. Use updateProperties to avoid
     * duplicates and update exiting Properties.
     *
     * @param target The bag in which the properties must be copied.
     * @param source The bag containing properties for \a target.
     * @ingroup CoreLibProperties
     */
    bool copyProperties(PropertyBag& target, const PropertyBag& source);

    /**
     * This function updates (recursively) the values of Property objects of one Bag with the
     * values of Property objects of another bag.
     * It creates new Property instances if a Property
     * is not present in the target and updates existing Properties.
     *
     * Use copyProperties if you want complete duplication.
     * @param target The bag in which the properties must be updated.
     * @param source The bag containing new values, descriptions or properties for \a target.
     * @ingroup CoreLibProperties
     */
    bool updateProperties(PropertyBag& target, const PropertyBag& source);

    /**
     * This function updates (recursively) the values of a single Property object of one Bag with the
     * property of another bag.
     * It creates a new Property instances if the Property
     * is not present in the target and updates existing Properties.
     *
     * Use copyProperties if you want complete duplication.
     * @param target The bag in which the property must be updated.
     * @param source The bag containing \a prop as one of its children.
     * @param path The path to a property in \a source (or in a sub-bag of \a source), 
     * which must be created or updated in \a target.
     * @param separator The token to separate properties in the \a path,
     * @ingroup CoreLibProperties
     */
    bool updateProperty(PropertyBag& target, const PropertyBag& source, const std::string& path, const std::string& separator = ".");

    /**
     * This function refreshes (recursively) the values of a single Property object of one Bag with the
     * property of another bag.
     * It will \b not create a new Property instances if the Property
     * is not present in the target and hence fail in that case.
     *
     * @param target The bag in which the property exists and must be refreshed.
     * @param source The bag containing \a prop as one of its children.
     * @param path The path to a property in \a source (or in a sub-bag of \a source), 
     * which must be created or updated in \a target.
     * @param separator The token to separate properties in the \a path,
     * @ingroup CoreLibProperties
     */
    bool refreshProperty(PropertyBag& target, const PropertyBag& source, const std::string& path, const std::string& separator = ".");

    /**
     * This function iterates over a PropertyBag and deletes all Property objects in
     * it without recursion.
     *
     * @post All objects in this bag are deleted and no elements reside in the bag
     *       anymore.
     * @ingroup CoreLibProperties
     */
    void deleteProperties(PropertyBag& target);

    /**
     * This function iterates over a PropertyBag and recursively deletes all Property objects.
     *
     * @post All objects in this bag (and possible subbags) are deleted and no elements reside in the bag
     *       anymore.
     * @ingroup CoreLibProperties
     */
    void deletePropertyBag(PropertyBag& target);

    /**
     * This function flattens a PropertyBag recursively.
     * The names of the Propety objects of the included bags are
     * placed in this bag with the included bag's name prefixed. If the Property object
     * in that bag is also a bag, the same operation is performed recursively.
     * So any bag in \a target will show up at the
     * root of \a target with the path prefixed.
     * @ingroup CoreLibProperties
     */
    void flattenPropertyBag(PropertyBag& target, const std::string& separator=".");

    /**
     * @}
     */
} // Namespace RTT
#endif
