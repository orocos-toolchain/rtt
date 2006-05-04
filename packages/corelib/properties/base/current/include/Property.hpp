/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Property.hpp 

                        Property.hpp -  description
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
 
#ifndef ORO_PROPERTY_HPP
#define ORO_PROPERTY_HPP

#include <pkgconf/corelib_properties.h>
#include "Marshaller.hpp"
#include "PropertyBase.hpp"
#include "PropertyBag.hpp"
#include "PropertyCommands.hpp"
#include "DataSources.hpp"
#include "DataSourceAdaptor.hpp"
#include <boost/type_traits.hpp>

#ifdef HAVE_STRING
#include <string>
#endif

#include <iostream>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifdef OROINT_OS_CORBA
#include "corba/CorbaConversion.hpp"
#endif

namespace ORO_CoreLib
{

    /**
     * @brief Helper functions for Property operations.
     *
     * These helper functions define for each type which
     * operators need to be applied to copy, refresh or update a type T.
     *
     * Remark that PropertyBag for example overloads these functions.
     * Standard STL Containers (like vector) don't need an overload ( operator= ).
     * All overloads must be in namespace ORO_CoreLib.
     *
     * @{
     */
    template< class T, class S>
    inline bool copy(T& a, const S& b)
    {
        a = b;
        return true;
    }

    template< class T, class S>
    inline bool update(T& a, const S& b)
    {
        a = b;
        return true;
    }

    template< class T, class S>
    inline bool refresh(T& a, const S& b)
    {
        a = b;
        return true;
    }
    /**
     * @}
     */

	/**
	 * @brief A property represents a value of any type.
     *
	 * A property is a tuple of a name, a description and a variable of any
	 * type. It's purpose is to provide an easy to manipulate parameter of an 
     * object by external entities. They can be grouped in PropertyBag objects
     * and a Property can contain a PropertyBag itself. See the configuration tool
     * to influence how the compiler should generate code for unkown
     * (external to Orocos) classes \a T.
     *
	 *
     * @param T The type of the data contained within the Property.
     * @see decomposeProperty and composeProperty if you want to use properties of
     * external types \a T.
	 */
    template<typename T>
    class Property
        : public PropertyBase
    {
    public:
        /**
         * The types of this property type.
         * value_t is always the 'bare' value type of T. From this type,
         * we derive the other (param, ref, ...) types.
         */
        typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type value_t;
        typedef typename boost::call_traits<value_t>::param_type param_t;
        typedef typename boost::call_traits<value_t>::reference reference_t;
        typedef typename boost::call_traits<value_t>::const_reference const_reference_t;
        typedef value_t DataSourceType;

        /**
         * The constructor which initializes the property's value.
         * @param name The name which will be used to refer to the
         * property.
         * @param description The description of the property.
         * @param value The initial value of the property (optional).
         */
        Property(const std::string& name, const std::string& description, param_t value = value_t() )
            : PropertyBase(name, description), _value( new ValueDataSource<DataSourceType>(value) )
        {}

        /**
         * Copy constructors copies the name, description and value
         * as deep copies.
         */
        Property( const Property<T>& orig)
            : PropertyBase(orig.getName(), orig.getDescription()),
              _value( new ValueDataSource<DataSourceType>( orig.get() ) )
        {}

        /**
         * The constructor which initializes the property with a DataSource.
         * @param name The name which will be used to refer to the
         * property.
         * @param description The description of the property.
         * @param datasource A new data source to be acquired by this property for
         * storing its data.
         */
        Property(const std::string& name, const std::string& description, AssignableDataSource<DataSourceType>* datasource )
            : PropertyBase(name, description), _value( datasource )
        {}

        /**
         * Set the property's value.
         * @param value The value to be set.
         * @return A reference to newly set property value.
         */
        Property<T>& operator=( param_t value )
        {
            _value->set( value );
            return *this;
        }

        /**
         * Update the value of this property with
         * another property.
         */
        Property<T>& operator<<=(Property<T> &p)
        {
            this->update( p );
            return *this;
        }

        /**
         * Get a copy of the value of the property.
         * @return A copy of the value of the property.
         */
        operator value_t() const
        {
            return _value->get();
        }

        /**
         * Get the value of the property.
         * @return The value of the property.
         */
        DataSourceType get() const
        {
            return _value->get();
        }

        /**
         * Access to the value of the Property. Identical to value().
         * @warning This function is not suitable
         * for remote (distributed) access of properties,
         * use operator=() or set( param_t v ) to assign a value.
         */
        reference_t set()
        {
            return _value->set();
        }

        /**
         * Set the value of the Property.
         */
        void set(param_t v)
        {
            _value->set(v);
        }

        /**
         * Access to the value of the Property. Identical to set().
         * @warning This function is not suitable
         * for remote (distributed) access of properties,
         * use operator=() or set( param_t v ) to assign a value.
         * 
         */
        reference_t value()
        {
            return _value->set();
        }

        /**
         * Use this method instead of dynamic_cast<> to cast from
         * PropertyBase to Property<T>. You need to delete the returned property
         * if it is no longer needed.
         * @param T The Desired property type, for example 'double'.
         * @param prop The property to narrow to Property<T>.
         * @return Null if prop is not convertible to type Property<T>,
         * a \b new Property<T> instance otherwise.
         */
        static Property<T>* narrow( PropertyBase* prop );

        virtual void identify( PropertyIntrospection* pi) const;
        
        virtual bool update( const PropertyBase* other) 
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 ) {
                return this->update( *origin );
            }
            return false;
        }

        virtual CommandInterface* updateCommand( const PropertyBase* other)
        {
            // try to update from identical type or from const_reference_t.
            const Property<T>* origin = dynamic_cast<const Property<T>* >( other );
            if ( origin != 0 )
                return new detail::UpdatePropertyCommand<T>(this, origin);
            const Property< const_reference_t >* corigin
                = dynamic_cast<const Property<const_reference_t >* >( other );
            if ( corigin != 0 )
                return new detail::UpdatePropertyCommand<T, const_reference_t>(this, corigin);
            return 0;
        }

        virtual bool refresh( const PropertyBase* other) 
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 ) {
                return this->refresh( *origin );
            }
            return false;
        }

        virtual CommandInterface* refreshCommand( const PropertyBase* other)
        {
            // refresh is just an update of the datasource.
            DataSourceBase::const_ptr sourcebase = other->getDataSource();
            return _value->updateCommand( sourcebase.get() );
        }

        virtual bool copy( const PropertyBase* other )
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 ) {
                return this->copy( *origin );
            }
            return false;
        }

        virtual CommandInterface* copyCommand( const PropertyBase* other)
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 )
                return new detail::CopyPropertyCommand<T>(this, origin);
            const Property< const_reference_t >* corigin
                = dynamic_cast<const Property<const_reference_t >* >( other );
            if ( corigin != 0 )
                return new detail::CopyPropertyCommand<T, const_reference_t>(this, corigin);
            return 0;
        }

        /**
         * Copy the value, complete overwrite of this Property with orig.
         */
        template<class S>
        bool copy( const Property<S>& orig)
        {
            _description = orig.getDescription();
            _name = orig.getName();
            *this = orig.get();
            return true;
        }

        /**
         * Update the value, optionally also the description if current
         * description is empty.
         */
        template<class S>
        bool update( const Property<S>& orig)
        {
            if ( _description.empty() )
                _description = orig.getDescription();
            *this = orig.get();
            return true;
        }

        /**
         * Refresh only the value from a Property.
         * This is a real-time operation.
         */
        template<class S>
        bool refresh( const Property<S>& orig)
        {
            *this = orig.getDataSource()->value();
            return true;
        }

        virtual Property<T>* clone() const
        {
            return new Property<T>(*this);
        }

        virtual Property<T>* create() const
        {
            return new Property<T>( _name, _description );
        }

        virtual AssignableDataSource<DataSourceType>* getDataSource() const {
            return _value.get();
        }

        virtual std::string getType() const {
            return DataSource<T>::GetType();
        }
    protected:
        typename AssignableDataSource<DataSourceType>::shared_ptr _value;
    };

    /**
     * Specialisation of Property<PropertyBag>.
     * PropertyBags are local objects. The get() method of this specialisation returns
     * a const reference to the internal PropertyBag, instead of a copy
     * as in the default Property implementation. This allows some real-time algorithms
     * to work with PropertyBag objects, as no copy is made, this is especially the case
     * for algorithms which use the refresh methods below, since refresh is intended
     * as a real-time value-update mechanism.
     */
    template<>
    class Property<PropertyBag>
        : public PropertyBase
    {
    public:
        /**
         * The types of this property type.
         * value_t is always the 'bare' value type of T. From this type,
         * we derive the other (param, ref, ...) types.
         */
        typedef PropertyBag value_t;
        typedef boost::call_traits<value_t>::param_type param_t;
        typedef boost::call_traits<value_t>::reference reference_t;
        typedef boost::call_traits<value_t>::const_reference const_reference_t;

        /**
         * The constructor which initializes the property's value.
         * @param name The name which will be used to refer to the
         * property.
         * @param description The description of the property.
         * @param value The initial value of the property (optional).
         */
        Property(const std::string& name, const std::string& description, param_t value = value_t() )
            : PropertyBase(name, description), _value( value )
        {}

        /**
         * Copy constructors copies the name, description and value
         * as deep copies.
         */
        Property( const Property<PropertyBag>& orig)
            : PropertyBase(orig.getName(), orig.getDescription()),
              _value()
        {}

        /**
         * Set the property's value.
         * @param value The value to be set.
         * @return A reference to newly set property value.
         */
        Property<PropertyBag>& operator=( param_t value )
        {
            _value = value;
            return *this;
        }

        /**
         * Update the value of this property with
         * another property.
         */
        Property<PropertyBag>& operator<<=(Property<PropertyBag> &p)
        {
            this->update( p );
            return *this;
        }

        virtual ~Property();

        /**
         * Get a copy of the value of the property.
         * @return A copy of the value of the property.
         */
        operator value_t() const
        {
            return _value;
        }

        /**
         * Get a reference to the PropertyBag.
         * @return The value of the property.
         */
        const_reference_t get() const
        {
            return _value;
        }

        /**
         * Access to PropertyBag of the Property. Identical to value().
         */
        reference_t set()
        {
            return _value;
        }

        /**
         * Set the PropertyBag of the Property.
         */
        void set(param_t v)
        {
            _value = v;
        }

        /**
         * Access to the PropertyBag of the Property. Identical to set().
         */
        reference_t value()
        {
            return _value;
        }

        /**
         * Use this method instead of dynamic_cast<> to cast from
         * PropertyBase to Property<PropertyBag>. You need to delete the returned property
         * if it is no longer needed.
         * @param prop The property to narrow to Property<PropertyBag>.
         * @return Null if prop is not convertible to type Property<PropertyBag>,
         * a \b new Property<PropertyBag> instance otherwise.
         */
        static Property<PropertyBag>* narrow( PropertyBase* prop );

        virtual void identify( PropertyIntrospection* pi) const;
        
        virtual bool update( const PropertyBase* other) 
        {
            const Property<PropertyBag>* origin = dynamic_cast< const Property<PropertyBag>* >( other );
            if ( origin != 0 ) {
                return this->update( *origin );
            }
            return false;
        }

        virtual CommandInterface* updateCommand( const PropertyBase* other)
        {
            const Property<PropertyBag>* origin = dynamic_cast<const Property<PropertyBag>* >( other );
            if ( origin != 0 )
                return new detail::UpdatePropertyCommand<PropertyBag>(this, origin);
            return 0;
        }

        virtual bool refresh( const PropertyBase* other) 
        {
            const Property<PropertyBag>* origin = dynamic_cast< const Property<PropertyBag>* >( other );
            if ( origin != 0 ) {
                return this->refresh( *origin );
            }
            return false;
        }

        virtual CommandInterface* refreshCommand( const PropertyBase* other)
        {
            const Property<PropertyBag>* origin = dynamic_cast<const  Property<PropertyBag>* >( other );
            if ( origin != 0 )
                return new detail::RefreshPropertyCommand<PropertyBag>(this, origin);
            return 0;
        }

        virtual bool copy( const PropertyBase* other )
        {
            const Property<PropertyBag>* origin = dynamic_cast< const Property<PropertyBag>* >( other );
            if ( origin != 0 ) {
                return this->copy( *origin );
            }
            return false;
        }

        virtual CommandInterface* copyCommand( const PropertyBase* other)
        {
            const Property<PropertyBag>* origin = dynamic_cast<const  Property<PropertyBag>* >( other );
            if ( origin != 0 )
                return new detail::CopyPropertyCommand<PropertyBag>(this, origin);
            return 0;
        }


        /**
         * Update the value, optionally also the description if current
         * description is empty.
         */
        template<class S>
        bool update( const Property<S>& orig)
        {
            if ( _description.empty() )
                _description = orig.getDescription();
            return updateProperties( this->_value, orig.get() );
        }

        /**
         * Refresh only the value from a Property.
         */
        template<class S>
        bool refresh( const Property<S>& orig)
        {
            return refreshProperties( this->_value, orig.get() );
        }

        /**
         * Make a full copy.
         */
        template<class S>
        bool copy( const Property<S>& orig)
        {
            _name = orig.getName();
            _description = orig.getDescription();
            return ORO_CoreLib::copy( _value, orig.get() );
        }

        virtual Property<PropertyBag>* clone() const
        {
            return new Property<PropertyBag>(*this);
        }

        virtual Property<PropertyBag>* create() const
        {
            return new Property<PropertyBag>( _name, _description );
        }

        virtual AssignableDataSource<PropertyBag>* getDataSource() const {
            return new ValueDataSource<PropertyBag>( _value );
        }

        virtual std::string getType() const {
            return DataSource<PropertyBag>::GetType();
        }
    protected:
        PropertyBag _value;
    private:
    };

    template<typename T>
    std::ostream& operator<<(std::ostream &os, Property<T> &p)
    {
        os << p.get();
        return os;
    }

    template<class T>
    Property<T>* Property<T>::narrow( PropertyBase* prop ) {
        Property<T>* res = dynamic_cast<Property<T>*>( prop );
        if (res)
            return res->clone();
#ifdef OROINT_CORBA
        if ( prop->getDataSource()->hasServer() ) {
            T result;
            CORBA::Any_var any = propbase->getDataSource()->getAny();
            if( AnyConversion<T>::update( any.in() , result ) ) {
                return new Property<T>( propbase->getName(), propbase->getDescription(), result );
            }
        } 
#endif
        return 0;
    }

    extern template class Property<double>;
    extern template class Property<bool>;
    extern template class Property<float>;
    extern template class Property<int>;
    extern template class Property<unsigned int>;
    extern template class Property<std::string>;
    extern template class Property<const std::string &>;
#ifdef OROINT_OS_CORBA
    extern template class Property<CORBA::Any>;
#endif
}

#include "PropertyIntrospection.hpp"

namespace ORO_CoreLib
{
    template< class T>
    void Property<T>::identify( PropertyIntrospection* pi) const
    {
        pi->introspect( *this );
    }

}

// By PS : these are confusing, thus removed.

/* example simple_hibernate.cpp
 * This example shows how to use simple properties in combination with a
 * marshaller. The SimpleMarshaller is used and the serialized data is
 * shown on standard output.
 */
/* example simple_incarnate.cpp
 * This example shows how to demarshall a stream containing simple properties.
 * The SimpleDemarshaller is used and the serialized data is read from standard input.
 */

/* example adding_property_types.cpp
 * This example shows that properties can be of any type, including user
 * defined types. When you want to be able to marshall these properties you
 * need to add support for decomposing objects of your newly defined type
 * into a treestructure of simple properties.
 * <br>
 * The example shows the class Rectangle being used as a property without
 * adding support for marshalling it.
 * The other added type, namely Vector, is being used as a property and
 * the decompose function was added so properties of type Vector can be
 * marshalled with any Marshaller.
 */
#endif
