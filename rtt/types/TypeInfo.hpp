/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  TypeInfo.hpp

                        TypeInfo.hpp -  description
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

#ifndef ORO_CORELIB_TYPEINFO_HPP
#define ORO_CORELIB_TYPEINFO_HPP

#include <vector>
#include <string>
#include "../base/DataSourceBase.hpp"

namespace RTT
{ namespace types {


    /**
     * A class for representing a user type, and which can build
     * instances of that type.
     */
    class RTT_API TypeInfo
    {
    protected:
        typedef std::vector<TypeBuilder*> Constructors;
        typedef std::vector<TypeTransporter*> Transporters;
        Constructors constructors;
        Transporters transporters;
    public:
        virtual ~TypeInfo();
        /**
         * Return unique the type name.
         */
        virtual const std::string& getTypeName() const = 0;

        /**
         * @name Type building/factory functions
         * Used to create objects that hold data of a certain type.
         * @{
         */
        /**
         * Build a non modifyable instance of this type.
         * @param sizehint For variable size instances, use it to hint
         * the size of the instance.
         */
        virtual base::AttributeBase* buildConstant(std::string name,base::DataSourceBase::shared_ptr, int sizehint) const;
        virtual base::AttributeBase* buildConstant(std::string name,base::DataSourceBase::shared_ptr) const = 0;
        /**
         * Build a modifyable instance of this type.
         * @param sizehint For variable size instances, use it to hint
         * the size of the instance.
         */
        virtual base::AttributeBase* buildVariable(std::string name,int sizehint) const;
        virtual base::AttributeBase* buildVariable(std::string name) const = 0;

        /**
         * Tries to resize a data source in case it's a resizable sequence.
         * @return true if the resizing could be done, false otherwise.
         */
        virtual bool resize(base::DataSourceBase::shared_ptr arg, int size) const;

        /**
         * Constructor syntax: construct a internal::DataSource which returns an instance of data
         * depending on the given arguments.  When \a args is empty, the default 'variable'
         * is returned.
         */
        virtual base::DataSourceBase::shared_ptr construct(const std::vector<base::DataSourceBase::shared_ptr>& args) const;

        /**
         * Automatic conversion: convert a internal::DataSource to this type.
         * For example, for converting float -> double. If no valid conversion was found,
         * returns arg again, otherwise, a new data source.
         */
        virtual base::DataSourceBase::shared_ptr convert(base::DataSourceBase::shared_ptr arg) const;

        /**
         * Returns an assignable data source that uses arg as a store. This can only be done if
         * arg is an assignable data source itself or is a data source that returns a reference in get().
         * @return a new assignable data source, or \a arg if arg is assignable, or \a null if arg could
         * not be used for assignment.
         */
        virtual base::DataSourceBase::shared_ptr getAssignable(base::DataSourceBase::shared_ptr arg) const = 0;

        /**
         * Add a constructor/convertor object.
         */
        virtual void addConstructor(TypeBuilder* tb);

        /**
         * build an alias with b as the value.  If b is of the wrong type,
         * 0 will be returned..
         */
        virtual base::AttributeBase* buildAlias(std::string name, base::DataSourceBase::shared_ptr b ) const = 0;

        /**
         * Build a Property of this type.
         */
        virtual base::PropertyBase* buildProperty(const std::string& name, const std::string& desc, base::DataSourceBase::shared_ptr source = 0) const = 0;

        /**
         * Build an Attribute of this type.
         */
        virtual base::AttributeBase* buildAttribute(std::string name, base::DataSourceBase::shared_ptr source = 0 ) const = 0;

        /**
         * Build a internal::ValueDataSource of this type.
         */
        virtual base::DataSourceBase::shared_ptr buildValue() const = 0;

        /** Build a internal::ReferenceDataSource of this type, pointing to the given
         * pointer
         */
        virtual base::DataSourceBase::shared_ptr buildReference(void* ptr) const = 0;

        /**
         * Returns a DataSource that first executes an action and returns the result of another data source.
         * If \a source is an AssignableDataSource, an AssignableDataSource is returned of the same type, otherwise,
         * a plain DataSource is returned.
         */
        virtual base::DataSourceBase::shared_ptr buildActionAlias(base::ActionInterface* action, base::DataSourceBase::shared_ptr source) const = 0;
        /** @} */

        /**
         * @name Conversion to/from text
         * Used to convert data to human readable text and vice versa.
         * @{
         */
        /**
         * Output this datasource as a human readable string.
         * The default just writes the type name in parentheses to \a os.
         */
        virtual std::ostream& write(std::ostream& os, base::DataSourceBase::shared_ptr in ) const = 0;

        /**
         * Read a new value for this datasource from a human readable string.
         * The default does nothing.
         */
        virtual std::istream& read(std::istream& os, base::DataSourceBase::shared_ptr out ) const = 0;

        /**
         * Usability function which converts data to a string.
         */
        virtual std::string toString( base::DataSourceBase::shared_ptr in ) const;

        /**
         * Usability function which converts a string to data.
         */
        virtual bool fromString( const std::string& value, base::DataSourceBase::shared_ptr out ) const;
        /** @} */

        /**
         * @name Inspecting data structures.
         * Used to write a complex type to an external representation, like XML.
         * @{
         */

        /**
         * Returns the list of part names of this type.
         */
        virtual std::vector<std::string> getPartNames() const;

        /**
         * Returns a part of a given item identified by its name.
         * @param item The item of which to return a part
         * @param name The name of a part within \a item
         * @return null if no such part exists, an assignable datasource referencing that part otherwise.
         */
        virtual base::DataSourceBase::shared_ptr getPart(base::DataSourceBase::shared_ptr item, const std::string& name) const;

        /**
         * Returns a part of a given item identified by a data source id.
         * @param item The item of which to return a part
         * @param name Or a string containing the name of a part, Or an unsigned int containing the
         * index of the item to retrieve.
         */
        virtual base::DataSourceBase::shared_ptr getPart(base::DataSourceBase::shared_ptr item,
                                                         base::DataSourceBase::shared_ptr id) const;

        /**
         * Compose a type from a DataSourceBase containing its basic parts.
         * The default behavior tries to assign \a source to \a target. If that fails,
         * it tries to decompose \a target into its parts and update \a target with the contents of source.
         *
         * The default implementation in TemplateTypeInfo works for most types, but can be overridden in case there are
         * multiple versions/possibilities to make a \a target from a \a source. For example, in
         * order to support legacy formats.
         *
         * @param source A data source of the same type as \a target OR a PropertyBag that contains the parts of \a target
         * to be refreshed.
         * @param target A data source of the same type as this TypeInfo object which contains the data to be updated from \a source.
         * @return true if source could be updated, false otherwise.
         *
         * @see types::propertyDecomposition and types::typeDecomposition for the inverse function, decomposing a type into
         * datasources and hierarchical properties.
         */
        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr target) const = 0;
        /**
         * @}
         */

        /**
         * @name Distribution of objects
         * Used to transport data over a network.
         * @{
         */

        /**
         * Register a protocol for data transport over a network.
         */
        bool addProtocol(int protocol_id, TypeTransporter* tt);

        /**
         * Returns this type's transport for a given protocol.
         * Will print a warning when returning null. Use
         * hasProtocol() to query without warning.
         */
        TypeTransporter* getProtocol(int protocol_id) const;

        /**
         * Check if this type is transporable over a given protocol.
         */
        bool hasProtocol(int protocol_id) const;

        /**
         * Return a list of protocols that have transporters
         */
        std::vector<int> getTransportNames() const;

        virtual std::string getTypeIdName() const = 0;

        /**
         * Returns a new InputPort<T> object where T is the type represented by
         * this TypeInfo object.
         */
        virtual base::InputPortInterface* inputPort(std::string const& name) const = 0;

        /**
         * Returns a new OutputPort<T> object where T is the type represented by
         * this TypeInfo object.
         */
        virtual base::OutputPortInterface* outputPort(std::string const& name) const = 0;

        /**
         * Creates single data or buffered storage for this type.
         *
         * @param policy Describes the kind of storage requested by the user
         * @return a storage element.
         */
        virtual base::ChannelElementBase* buildDataStorage(ConnPolicy const& policy) const = 0;
        virtual base::ChannelElementBase* buildChannelOutput(base::InputPortInterface& port) const = 0;
        virtual base::ChannelElementBase* buildChannelInput(base::OutputPortInterface& port) const = 0;
};

}}

#endif
