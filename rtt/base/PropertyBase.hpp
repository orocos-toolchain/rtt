/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  PropertyBase.hpp

                        PropertyBase.hpp -  description
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

#ifndef PI_PROPERTYBASE
#define PI_PROPERTYBASE

#include <string>
#include "rtt/base/ActionInterface.hpp"
#include "rtt/base/DataSourceBase.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{ namespace base {

	/**
	 * Base class for all properties.
	 *
	 */
    class RTT_API PropertyBase
    {
    public:
        /**
         * The default destructor.
         */
        virtual ~PropertyBase();

        /**
         * The constructor.
         * @param name The name which will be used to refer to this
         * property.
         * @param description A more elaborate description of the
         * property.
         */
        PropertyBase( std::string name, std::string description);

        /**
         * Create an uninitialised PropertyBase.
         */
        PropertyBase();

        /**
         * Mirror another PropertyBase (name, description and value).
         * @param source A pointer to the property to mirror.
         */
        virtual PropertyBase& operator=( PropertyBase* source ) = 0;

        /**
         * Get the name of the property.
         * @return name of the property.
         */
        const std::string& getName() const { return _name; }

        /**
         * Set the name of the property.
         * @param name The name of the property.
         */
        void setName(const std::string& name);

        /**
         * Get a description of the property.
         * @return description of the property.
         */
        const std::string& getDescription() const { return _description; }

        /**
         * Set the description of the property.
         * @param desc The description of the property.
         */
        void setDescription(const std::string& desc);

        /**
         * Inspect if this Property is correctly initialised
         * and ready for usage. A Property may only be used
         * when ready() returns true, otherwise, an exception
         * is thrown.
         */
        bool ready() const { return this->getDataSource().get() != 0; }

        /**
         * A call on this method will lead to a call to the PropertyIntrospection
         * interface identifying this Property's proper type.
         * @deprecated DO NOT USE. Will be removed in 2.x release series.
         */
        virtual void identify( PropertyIntrospection* pi) = 0;

        /**
         * A call on this method will lead to a call to the PropertyBagIntrospection
         * interface identifying this Property's proper type.
         * @deprecated DO NOT USE. Will be removed in 2.x release series.
         */
        virtual void identify( PropertyBagVisitor* pi);

        /**
         * Update the value of this Property with the value of an \a other Property.
         * Update does a full update of the value, adding extra
         * information if necessary, or in case of a Property<PropertyBag> adding missing
         * Properties.
         *
         * Update only works on properties that are ready() and both have the same data type T.
         *
         * @return false if the Properties are of different type or if this->ready() == false.
         */
        virtual bool update( const PropertyBase* other ) = 0;

        /**
         * Refresh the value of this Property with the value of an \a other Property.
         * Refresh does only the minimal update of the value, not adding extra
         * information, or in case of a Property<PropertyBag> not adding extra
         * Properties.
         *
         * Refresh only works on properties that are ready() and both have the same data type T.
         *
         * @return false if the Properties are of different type.
         */
        virtual bool refresh( const PropertyBase* other ) = 0;

        /**
         * Copy an \a other Property onto this property.
         * Copy does a full update of the name, description and value, adding extra
         * information if necessary, or in case of a Property<PropertyBag> adding all
         * Properties.
         *
         * Copy only works on properties that are ready() and both have the same data type T.
         *
         * @return false if the Properties are of different type.
         */
        virtual bool copy( const PropertyBase* other ) = 0;

        /**
         * Composes the data of this property with information found in a
         * PropertyBag.
         * This only works for data types that defined a compose/decompose
         * function in their type info structure.
         */
        virtual bool compose( const PropertyBag& source);

        /**
         * Deliver an identical clone of this PropertyBase. The
         * original may be deleted and the clone can be transparantly
         * used in its place or vice versa.
         */
        virtual PropertyBase* clone() const = 0;

        /**
         * Create a new default instance of the PropertyBase.
         * This is a factory method to 'make something of the same type'.
         * The new PropertyBase has the same name and description as this.
         */
        virtual PropertyBase* create() const = 0;

        /**
         * Create a new instance of the PropertyBase with a custom data source.
         * This is a factory method to 'make something of the same type'.
         * The new PropertyBase has the same name and description as this.
         */
        virtual PropertyBase* create( const base::DataSourceBase::shared_ptr& datasource ) const = 0;

        /**
         * Get an assignable base::DataSource through which this PropertyBase
         * can be manipulated.
         */
        virtual DataSourceBase::shared_ptr getDataSource() const = 0;

        /**
         * Assign an external assignable base::DataSource to this property.
         * @param dsb The other data source
         * @return false if the Properties are of different type.
         */
        virtual bool setDataSource( const DataSourceBase::shared_ptr& dsb ) = 0;

        /**
         * Returns the type of this PropertyBase. Uses the
         * naming scheme of DataSourceTypeInfo.
         */
        virtual std::string getType() const = 0;

        /**
         * Returns the types::TypeInfo object of this Property.
         */
        virtual const types::TypeInfo* getTypeInfo() const = 0;

    protected:
        /**
         * A short name for this PropertyBase.
         */
        std::string _name;

        /**
         * A lengthy description for this PropertyBase.
         */
        std::string _description;
    };

    /**
     * Prints the value of a Property.
     * @param os The stream to print to
     * @param p The property whose value to print.
     */
    RTT_API std::ostream& operator<<(std::ostream &os, PropertyBase* p);


    }
} // namespace RTT
#endif
