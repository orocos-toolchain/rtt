/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Property.hpp

                        Property.hpp -  description
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

#ifndef ORO_PROPERTY_HPP
#define ORO_PROPERTY_HPP

#include "rtt-config.h"
#include "Marshaller.hpp"
#include "PropertyBase.hpp"
#include "PropertyBag.hpp"
#include "PropertyCommands.hpp"
#include "DataSources.hpp"
#include "BuildType.hpp"
#include <boost/type_traits.hpp>

#include <string>
#include <ostream>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
	/**
	 * @brief A property represents a named value of any type with a description.
     *
	 * A property is a tuple of a name, a description and a variable of any
	 * type. It's purpose is to provide an easy to manipulate parameter of an
     * object by external entities. They can be grouped in PropertyBag objects
     * and a Property can contain a PropertyBag itself.
     *
     * If you do not provide a name nor description when constructing the
     * Property object, it will be uninitialised and ready() will return false.
     * Such an object may not be used (set(), get(),...) until it has been
     * initialised from another Property. Otherwise, an exception (or assert) will be
     * thrown.
     *
     * @param T The type of the data contained within the Property.
     * @ingroup CoreLibProperties
     * @ingroup RTTComponentInterface
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
         * Create an empty Property with no name, no description and no value.
         * @post ready() will return false. Initialise this Property with another Property object.
         */
        Property()
        {}

        /**
         * The constructor which initializes the property's value.
         * @param name The name which will be used to refer to the
         * property.
         * @param description The description of the property.
         * @param value The initial value of the property (optional).
         * @post ready() will always be true.
         */
        Property(const std::string& name, const std::string& description, param_t value = value_t() )
            : PropertyBase(name, description), _value( detail::BuildType<value_t>::Value( value ) )
        {
        }

        /**
         * Copy constructors copies the name, description and value
         * as \b deep copies.
         * @post ready() will be true if orig.ready() is true.
         */
        Property( const Property<T>& orig)
            : PropertyBase(orig.getName(), orig.getDescription()),
              _value( orig._value ? orig._value->clone() : 0 )
        {}

        /**
         * Create a Property \b mirroring another PropertyBase.
         * It copies the name and description, and \b shallow copies
         * the value.
         * @see ready() to inspect if the creation succeeded.
         */
        Property( PropertyBase* source)
            : PropertyBase(source ? source->getName() : "", source ? source->getDescription() : ""),
              _value( source ? AssignableDataSource<DataSourceType>::narrow(source->getDataSource().get() ) : 0 )
        {
        }

        /**
         * The constructor which initializes the property with a DataSource.
         * @param name The name which will be used to refer to the
         * property.
         * @param description The description of the property.
         * @param datasource A new data source to be acquired by this property for
         * storing its data.
         * @post ready() will be true if datasource is a valid pointer.
         */
        Property(const std::string& name, const std::string& description,
                 typename AssignableDataSource<DataSourceType>::shared_ptr datasource )
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
         * Construct a Property which mirrors a PropertyBase.
         * @param source A pointer to the property to mirror.
         */
        Property<T>& operator=( PropertyBase* source )
        {
            if ( source ) {
                this->setName( source->getName() );
                this->setDescription( source->getDescription() );
                typename AssignableDataSource<DataSourceType>::shared_ptr vptr
                    = AssignableDataSource<DataSourceType>::narrow(source->getDataSource().get() );
                if (vptr)
                    _value = vptr;
                else
                    _value = detail::BuildType<value_t>::Value() ;
            } else {
                this->setName( "" );
                this->setDescription( "" );
                _value = 0;
            }
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
         * Get a copy of the value of the property.
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
            return set();
        }

        /**
         * Read-only (const&) access to the value of the Property.
         */
        const_reference_t rvalue() const
        {
            return _value->rvalue();
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

        virtual void identify( PropertyIntrospection* pi);

        virtual void identify( PropertyBagVisitor* pi);

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
            if ( origin != 0 && _value )
                return new detail::UpdatePropertyCommand<T>(this, origin);
            return 0;
        }

        virtual bool refresh( const PropertyBase* other)
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 && _value ) {
                return this->refresh( *origin );
            }
            return false;
        }

        virtual CommandInterface* refreshCommand( const PropertyBase* other)
        {
            if ( !_value )
                return 0;
            // refresh is just an update of the datasource.
            DataSourceBase::shared_ptr sourcebase = other->getDataSource();
            return _value->updateCommand( sourcebase.get() );
        }

        virtual bool copy( const PropertyBase* other )
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 && _value ) {
                return this->copy( *origin );
            }
            return false;
        }

        virtual CommandInterface* copyCommand( const PropertyBase* other)
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 && _value )
                return new detail::CopyPropertyCommand<T>(this, origin);
            return 0;
        }

        /**
         * Copy the value, complete overwrite of this Property with orig.
         */
        bool copy( const Property<T>& orig)
        {
            if ( !ready() )
                return false;
            _description = orig.getDescription();
            _name = orig.getName();
            *this = orig.rvalue();
            return true;
        }

        /**
         * Update the value, optionally also the description if current
         * description is empty.
         */
        bool update( const Property<T>& orig)
        {
            if ( !ready() )
                return false;
            if ( _description.empty() )
                _description = orig.getDescription();
            *this = orig.rvalue();
            return true;
        }

        /**
         * Refresh only the value from a Property.
         * This is a real-time operation.
         */
        bool refresh( const Property<T>& orig)
        {
            if ( !ready() )
                return false;
            *this = orig.rvalue();
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

        virtual DataSourceBase::shared_ptr getDataSource() const {
            return _value;
        }

        typename AssignableDataSource<DataSourceType>::shared_ptr getAssignableDataSource() const {
            return _value;
        }

        virtual std::string getType() const {
            return DataSource<T>::GetType();
        }

        virtual const TypeInfo* getTypeInfo() const {
            return DataSource<T>::GetTypeInfo();
        }
    protected:
        typename AssignableDataSource<DataSourceType>::shared_ptr _value;
    };


    /**
     * Partial specialisations in case of PropertyBag.
     */
    template<>
    RTT_API bool Property<PropertyBag>::update( const Property<PropertyBag>& orig);

    template<>
    RTT_API bool Property<PropertyBag>::refresh( const Property<PropertyBag>& orig);

    template<>
    RTT_API bool Property<PropertyBag>::copy( const Property<PropertyBag>& orig);

    template<typename T>
    std::ostream& operator<<(std::ostream &os, Property<T> &p)
    {
#ifdef OS_HAVE_STREAMS
        os << p.getDataSource();
#endif
        return os;
    }

    template<class T>
    Property<T>* Property<T>::narrow( PropertyBase* prop ) {
        Property<T>* res = dynamic_cast<Property<T>*>( prop );
        if (res)
            return res->clone();
        // If this property is a proxy:
        int p_id = prop->getDataSource()->serverProtocol();
        if ( p_id ) {
            assert(false); // untested code.
#if 0
            T result;
            void* ret = propbase->getDataSource()->getBlob(p_id);
            if( A n y Conversion<T>::update( any.in() , result ) ) {
                return new Property<T>( propbase->getName(), propbase->getDescription(), result );
            }
#endif
        }
        return 0;
    }

#if !defined(ORO_EMBEDDED) && defined(__GNUC__)
    extern template class Property<double>;
    extern template class Property<bool>;
    extern template class Property<float>;
    extern template class Property<int>;
    extern template class Property<unsigned int>;
    extern template class Property<std::string>;
    extern template class Property<const std::string &>;
#endif
}

#include "PropertyIntrospection.hpp"

namespace RTT
{
    template< class T>
    void Property<T>::identify( PropertyIntrospection* pi)
    {
        pi->introspect( *this );
    }

    template< class T>
    void Property<T>::identify( PropertyBagVisitor* pi)
    {
        return PropertyBase::identify(pi);
    }

    template<>
    RTT_API void Property<PropertyBag>::identify( PropertyBagVisitor* pbi);
}

#endif
