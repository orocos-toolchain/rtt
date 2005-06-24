/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  PropertyBag.hpp

                        PropertyBag.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef PI_PROPERTY_BAG
#define PI_PROPERTY_BAG

#include "PropertyBase.hpp"

#ifdef HAVE_VECTOR
#include <vector>
#endif

#ifdef HAVE_ALGORITHM
#include <algorithm>
#endif

#include <iostream>
#include <vector>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_CoreLib
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
     Property<ClassT>* pb = bag.getProperty<ClassT>( "name" ).
     @endverbatim
     * Both will return null if no such property exists.
	 * @see PropertyBase, Property
	 */
    class PropertyBag
    {
    public:
        typedef std::vector<PropertyBase*> PropertyContainerType;
        typedef PropertyContainerType::iterator iterator;
        typedef PropertyContainerType::const_iterator const_iterator;

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
         * Add a property to the container.
         * @param p Pointer to the property to be added.
         */
        void add(PropertyBase *p);

        /**
         * Remove a property from the container.
         * @param p Pointer to the property to be removed.
         */
        void remove(PropertyBase *p);

        /**
         * Removes all PropertyBases from this bag, without deleting
         * them. This is a not recursive function.
         */
        void clear();


        /**
         * List all properties.
         * @param names The container which will be used to store all
         * available property names in.
         */
        void list(std::vector<std::string> &names) const;

        /**
         * Return true if no properties are present in this bag.
         */
        bool empty() const
        {
            return _properties.empty();
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
            const_iterator i( std::find_if(_properties.begin(), _properties.end(), std::bind2nd(FindProp(), name ) ) );
            if ( i != _properties.end() )
                return dynamic_cast< Property<T>* >( *i );
            return 0;
        }

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

        const std::string& getType() const { return type;}

        void setType(const std::string& newtype) { type = newtype; }

        PropertyContainerType& getProperties() { return _properties; }
        const PropertyContainerType& getProperties() const { return _properties; }

        iterator begin() { return _properties.begin(); }
        const_iterator begin() const { return _properties.begin(); }
        iterator end() { return _properties.end(); }
        const_iterator end() const { return _properties.end(); }
    protected:
        PropertyContainerType _properties;

        /**
         * A function object for finding a Property by name.
         */
        struct FindProp : public std::binary_function<const PropertyBase*,const std::string, bool>
        {
            bool operator()(const PropertyBase* b1, const std::string& b2) const { return b1->getName() == b2; }
        };

        std::string type;
    };


    /**
     * Some helper functions to retrieve PropertyBases from
     * PropertyBags.
     *
     * @param bag The bag to look for a Property.
     * @param nameSequence A sequence of names, separated by
     *        a double colon indicating the path in the bag to a property,
     *        omitting the name of the \a bag itself.
     */
    PropertyBase* find(const PropertyBag& bag, const std::string& nameSequence, const std::string& separator = std::string("::") );


    /**
     * @group BagOperations These functions operate on the contents of bags,
     *        possibly modifying, deleting or creating new Property objects.
     * @{
     */

    /**
     * This function refreshes the values of the properties in one PropertyBag with
     * the values of the properties of another PropertyBag.
     * No new properties will be created.
     *
     * You can use this function to update the properties of a fixed bag.
     */
    bool refreshProperties(PropertyBag& target, const PropertyBag& source);

    /**
     * Refresh one Property in the target bag with the new value.
     */
    bool refreshProperty( PropertyBag& target, const PropertyBase& source);

    /**
     * This function copies (recursively) the Properties of one Bag into
     * another Bag. This may cause duplicate entries in \a target if \a source
     * contains Properties with the same name. Use updateProperties to avoid
     * duplicates and update exiting Properties.
     *
     */
    bool copyProperties(PropertyBag& target, const PropertyBag& source);

    /**
     * This function updates (recursively) the values of Property objects of one Bag with the
     * values of Property objects of another bag.
     * It creates new Property instances if a Property
     * is not present in the target and updates existing Properties.
     *
     * Use copyProperties if you want complete duplication.
     */
    bool updateProperties(PropertyBag& target, const PropertyBag& source);

    /**
     * This function iterates over a PropertyBag and deletes all Property objects in
     * it without recursion.
     *
     * @post All objects in this bag are deleted and no elements reside in the bag
     *       anymore.
     */
    void deleteProperties(PropertyBag& target);

    /**
     * This function flattens a PropertyBag recursively.
     * The names of the Propety objects of the included bags are
     * placed in this bag with the included bag's name prefixed. If the Property object
     * in that bag is also a bag, the same operation is performed recursively.
     * So any bag in \a target will show up at the
     * root of \a target with the path prefixed.
     */
    void flattenPropertyBag(PropertyBag& target, const std::string& separator="::");

    /**
     * @}
     */

    /**
     * Updating a bag is actually updating the bag.
     * @see updateProperties()
     */
    inline
    bool update(PropertyBag& a, const PropertyBag& b)
    {
        return updateProperties(a,b);
    }

    /**
     * Copying a bag is actually making a deep copy of the bag.
     * @see copyProperties()
     */
    inline
    bool copy(PropertyBag& a, const PropertyBag& b)
    {
        return copyProperties(a,b);
    }

    /**
     * Refresh a bag is actually refreshing a bag.
     * @see refreshProperties()
     */
    inline
    bool refresh(PropertyBag& a, const PropertyBag& b)
    {
        return refreshProperties(a,b);
    }

    /**
     * A helper class, like std::auto_ptr, meant to protect you from
     * forgetting to delete the properties in a property bag of which
     * you own the contents...  This class contains a PropertyBag, and
     * calls deleteProperties() on it in its destructor.
     */
    struct PropertyBagOwner
    {
        PropertyBagOwner()
            {
            }
        PropertyBagOwner( const PropertyBag& b )
            : bag( b )
            {
            }
        PropertyBag bag;
        ~PropertyBagOwner()
            {
                deleteProperties( bag );
            }
        PropertyBagOwner& operator=( const PropertyBag& b )
            {
                deleteProperties( bag );
                bag = b;
                return *this;
            }
    private:
        // don't copy PropertyBagOwner's !
        PropertyBagOwner( const PropertyBagOwner& );
    };
} // Namespace ORO_CoreLib
#endif
