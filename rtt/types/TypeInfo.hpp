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
#include "../base/ChannelElementBase.hpp"
#include "../internal/SharedConnection.hpp"

#ifndef NO_TYPE_INFO_FACTORY_FUNCTIONS
#include "MemberFactory.hpp"
#include "ValueFactory.hpp"
#include "CompositionFactory.hpp"
#ifndef NO_TYPE_INFO_STREAMS
#include "StreamFactory.hpp"
#endif
#endif


namespace RTT
{ namespace types {
    typedef boost::shared_ptr<internal::ConnFactory> ConnFactoryPtr;

    /**
     * A class for representing a user type, and which can build
     * instances of that type. Once you get hold of a TypeInfo
     * object pointer, this pointer will be valid during the whole
     * lifetime of the process. 
     */
    class RTT_API TypeInfo
    {
    public:
        typedef const std::type_info * TypeId;

        TypeInfo(const std::string& name) : mtypenames(1,name) {}

        ~TypeInfo();
        /**
         * Return the type name which was first registered.
         * Do not rely on this function to return always the same string,
         * as type loading order changes may return different names.
         * Use getTypeNames() to get all aliases as well or getTypeIdName()
         * to return the compiler generated C++ type name (non-portable
         * accross compilers).
         */
        const std::string& getTypeName() const { return mtypenames[0]; }

        /**
         * Returns all registered names and aliases of this type.
         */
        std::vector<std::string> getTypeNames() const;

        /**
         * Adds an alias to the this type.
         * @param alias an alternative name for this type which will
         * be returned in getTypeNames()
         */
        void addAlias(const std::string& alias);

        /**
         * Returns true if this type is known by the type system under
         * the given name.
         * @param type_name the name to query for
         * @return true if this alias exists for this type.
         */
        bool isType(const std::string& name);
        
        /**
         * Returns the compiler generated type id pointer.
         */
        TypeId getTypeId() const { return mtid; }

        /**
         * Returns the compiler generated type name (non portable accross compilers!).
         */
        const char * getTypeIdName() const { return mtid_name; }

        /**
         * Constructor syntax: construct a internal::DataSource which returns an instance of data
         * depending on the given arguments.  When \a args is empty, the default 'variable'
         * is returned.
         */
        base::DataSourceBase::shared_ptr construct(const std::vector<base::DataSourceBase::shared_ptr>& args) const;

        /**
         * Automatic conversion: convert a internal::DataSource to this type.
         * For example, for converting float -> double. If no valid conversion was found,
         * returns arg again, otherwise, a new data source.
         */
        base::DataSourceBase::shared_ptr convert(base::DataSourceBase::shared_ptr arg) const;

        /**
         * Add a constructor/convertor object.
         */
        void addConstructor(TypeConstructor* tb);

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

#ifndef NO_TYPE_INFO_FACTORY_FUNCTIONS
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
        base::AttributeBase* buildConstant(std::string name,base::DataSourceBase::shared_ptr source, int sizehint) const
        {
            return mdsf ? mdsf->buildConstant(name, source, sizehint) : 0;
        }

        base::AttributeBase* buildConstant(std::string name,base::DataSourceBase::shared_ptr source) const
        {
            return mdsf ? mdsf->buildConstant(name, source) : 0;
        }
        /**
         * Build a modifyable instance of this type.
         * @param sizehint For variable size instances, use it to hint
         * the size of the instance.
         */
        base::AttributeBase* buildVariable(std::string name,int sizehint) const;
        base::AttributeBase* buildVariable(std::string name) const
        {
            return mdsf ? mdsf->buildVariable(name) : 0;
        }

        /**
         * Tries to resize a data source in case it's a resizable sequence.
         * @return true if the resizing could be done, false otherwise.
         */
        bool resize(base::DataSourceBase::shared_ptr arg, int size) const
        {
            return mmembf ? mmembf->resize(arg, size) : false;
        }

        /**
         * build an alias with b as the value.  If b is of the wrong type,
         * 0 will be returned..
         */
        base::AttributeBase* buildAlias(std::string name, base::DataSourceBase::shared_ptr b ) const
        {
            return mdsf ? mdsf->buildAlias(name, b) : 0;
        }

        /**
         * Build a Property of this type.
         */
        base::PropertyBase* buildProperty(const std::string& name, const std::string& desc, base::DataSourceBase::shared_ptr source = 0) const
        {
            return mdsf ? mdsf->buildProperty(name, desc, source) : 0;
        }

        /**
         * Build an Attribute of this type.
         */
        base::AttributeBase* buildAttribute(std::string name, base::DataSourceBase::shared_ptr source = 0 ) const
        {
            return mdsf ? mdsf->buildAttribute(name, source) : 0;
        }

        /**
         * Build a internal::ValueDataSource of this type.
         */
        base::DataSourceBase::shared_ptr buildValue() const
        {
            return mdsf ? mdsf->buildValue() : base::DataSourceBase::shared_ptr();
        }

        /** Build a internal::ReferenceDataSource of this type, pointing to the given
         * pointer
         */
        base::DataSourceBase::shared_ptr buildReference(void* ptr) const
        {
            return mdsf ? mdsf->buildReference(ptr) : base::DataSourceBase::shared_ptr();
        }

        /**
         * Returns a DataSource that first executes an action and returns the result of another data source.
         * If \a source is an AssignableDataSource, an AssignableDataSource is returned of the same type, otherwise,
         * a plain DataSource is returned.
         */
        base::DataSourceBase::shared_ptr buildActionAlias(base::ActionInterface* action, base::DataSourceBase::shared_ptr source) const
        {
            return mdsf ? mdsf->buildActionAlias(action,source) : base::DataSourceBase::shared_ptr();
        }

        /** @} */

#ifndef NO_TYPE_INFO_STREAMS
        /**
         * @name Conversion to/from text
         * Used to convert data to human readable text and vice versa.
         * @{
         */
        /**
         * Output this datasource as a human readable string.
         * The default just writes the type name in parentheses to \a os.
         */
        std::ostream& write(std::ostream& os, base::DataSourceBase::shared_ptr in ) const
        {
            if ( mstrf ) {
                return mstrf->write(os,in);
            } else {
#ifdef OS_HAVE_STREAMS
                std::string output = std::string("(")+ in->getTypeName() +")";
                os << output;
#endif
                return os;
            }
        }

        /**
         * Read a new value for this datasource from a human readable string.
         * The default does nothing.
         */
        std::istream& read(std::istream& os, base::DataSourceBase::shared_ptr out ) const
        {
            return mstrf ? mstrf->read(os, out) : os;
        }


        /**
         * Usability function which converts data to a string.
         */
        std::string toString( base::DataSourceBase::shared_ptr in ) const
        {
            return mstrf ? mstrf->toString(in) : std::string();
        }

        /**
         * Usability function which converts a string to data.
         */
        bool fromString( const std::string& value, base::DataSourceBase::shared_ptr out ) const
        {
            return mstrf ? mstrf->fromString(value, out) : false;
        }

        /**
         * Returns true if this type is directly streamable
         * using read()/write() or toString()/fromString().
         */
        bool isStreamable() const
        {
            return mstrf ? mstrf->isStreamable() : false;
        }
        /** @} */
#endif
        /**
         * @name Inspecting data structures.
         * Used to write a complex type to an external representation, like XML.
         * @{
         */

        /**
         * Returns the list of struct member names of this type.
         * In case this type is not a struct, returns an empty list.
         */
        std::vector<std::string> getMemberNames() const
        {
            return mmembf ? mmembf->getMemberNames() : std::vector<std::string>();
        }

        /**
         * Returns a member of a given data source struct identified by its name.
         * @param item The item of which to return a reference to a member
         * @param name The name of a member within \a item. Is a name of a member in case of a struct
         * or an index number in case of a sequence.
         * @return null if no such member exists, an assignable datasource referencing that member otherwise.
         */
        base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const
        {
            return mmembf ? mmembf->getMember(item,name) : base::DataSourceBase::shared_ptr();
        }

        /**
         * Stores a reference to a member of a struct identified by its name.
         * This method does not allocate memory when \a item is assignable..
         * @param ref The reference object in which to store the reference.
         * @param item The item of which to return a reference to a member. It must be assignable, otherwise,
         * a reference to a copy of item will be returned. This copy \b will allocate memory.
         * @param name The name of a member within \a item.
         * @return false if no such member exists, true if ref got filled in otherwise.
         */
        bool getMember(internal::Reference* ref, base::DataSourceBase::shared_ptr item, const std::string& name) const
        {
            return mmembf ? mmembf->getMember(ref, item,name) : false;
        }

        /**
         * Returns a reference to a member of a given data source struct identified by its name.
         * @param item The item of which to return a reference to a member
         * @param name The name of a member within \a item. Is a name of a member in case of a struct
         * or an index number in case of a sequence.
         * @return false if no such member exists, true if ref got filled in otherwise.
         */
        base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                   base::DataSourceBase::shared_ptr id) const
        {
            return mmembf ? mmembf->getMember(item,id) : base::DataSourceBase::shared_ptr();
        }

        /**
         * Compose a type (target) from a DataSourceBase (source) containing its members.
         * The default behavior tries to assign \a source to \a target. If that fails,
         * it tries to decompose \a target into its members and update the members of \a target with the contents of source.
         *
         * The default implementation in TemplateTypeInfo works for most types, but can be overridden in case there are
         * multiple versions/possibilities to make a \a target from a \a source. For example, in
         * order to support legacy formats or in order to do the inverse of decomposeType().
         *
         * @param source A data source of the same type as \a target OR a PropertyBag that contains the parts of \a target
         * to be refreshed.
         * @param target A data source of the same type as this TypeInfo object which contains the data to be updated from \a source.
         * @return true if source could be updated, false otherwise.
         *
         * @see types::propertyDecomposition and types::typeDecomposition for the inverse function, decomposing a type into
         * datasources and hierarchical properties.
         * @see decomposeType to do the inverse operation.
         */
        bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr target) const
        {
            return mcompf ? mcompf->composeType(source,target) : false;
        }

        /**
         * Specialize this function to return an alternate type which represents this one in a compatible way.
         * For example, a short converts to an long or an enum to an int or a string.
         * If your return a datasource containing a property bag, then this function should do the inverse of
         * composeType: the returned property bag contains all parts of the current type (\a source) which can be modified and merged back
         * into this type with composeType. Mathematically: composeType( decomposeType( A ), B); assert( A == B );
         * @return null in order to indicate that decomposition through getMember() may be tried. You may return \a source itself in order
         * to prevent any further decomposition of your type (using getMember(), which is used as fall-back by the rest
         * of the software. For example, to avoid that a string is decomposed
         * into a sequence of chars, or to avoid that a primitive type like 'int' is further queried.
         */
        base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const
        {
            return mcompf ? mcompf->decomposeType(source) : base::DataSourceBase::shared_ptr();
        }

        /**
         * Specialize this function to return an alternate type which represents this one in a compatible way.
         * For example, a short converts to an long or an enum to an int or a string.
         * @return null if this type is not convertible to anything else.
         * @deprecated by decomposeType. We want to rename convertType to decomposeType. This function is left
         * here for transitional purposes.
         */
        base::DataSourceBase::shared_ptr convertType(base::DataSourceBase::shared_ptr source) const
        {
            return mcompf ? mcompf->convertType(source) : base::DataSourceBase::shared_ptr();
        }

        /**
         * @}
         */

        /**
         * Returns a new InputPort<T> object where T is the type represented by
         * this TypeInfo object.
         */
        base::InputPortInterface* inputPort(std::string const& name) const;

        /**
         * Returns a new OutputPort<T> object where T is the type represented by
         * this TypeInfo object.
         */
        base::OutputPortInterface* outputPort(std::string const& name) const;

        /**
         * Creates single data or buffered storage for this type.
         *
         * @param policy Describes the kind of storage requested by the user
         * @return a storage element.
         */
        base::ChannelElementBase::shared_ptr buildDataStorage(ConnPolicy const& policy) const;
        base::ChannelElementBase::shared_ptr buildChannelOutput(base::InputPortInterface& port, ConnPolicy const& policy) const;
        base::ChannelElementBase::shared_ptr buildChannelInput(base::OutputPortInterface& port, ConnPolicy const& policy) const;
        internal::SharedConnectionBase::shared_ptr buildSharedConnection(base::OutputPortInterface *output_port, base::InputPortInterface *input_port, ConnPolicy const& policy) const;

#endif // NO_TYPE_INFO_FACTORY_FUNCTIONS

        void setTypeId(TypeId tid) {
            mtid = tid;
            mtid_name = tid->name();
        }

        /**
         * Installs a new port factory such that in-process data
         * can be communicated between components.
         */
        void setPortFactory( ConnFactoryPtr cf ) { if (cf) mconnf = cf; }

        ConnFactoryPtr getPortFactory() const { return mconnf; }

        void setMemberFactory( MemberFactoryPtr mf ) { if (mf) mmembf = mf; }

        MemberFactoryPtr getMemberFactory() const { return mmembf; }

        void setValueFactory( ValueFactoryPtr dsf) { if (dsf) mdsf = dsf; }

        ValueFactoryPtr getValueFactory() const { return mdsf; }

        void setCompositionFactory( CompositionFactoryPtr cf) { if(cf) mcompf = cf; }

        CompositionFactoryPtr getCompositionFactory() const { return mcompf; }

        void setStreamFactory( StreamFactoryPtr sf ) { if (sf) mstrf = sf; }

        StreamFactoryPtr getStreamFactory() const { return mstrf; }
    protected:
        typedef std::vector<TypeConstructor*> Constructors;
        typedef std::vector<TypeTransporter*> Transporters;
        Constructors constructors;
        Transporters transporters;
        std::vector<std::string> mtypenames;
        const char* mtid_name;
        TypeId mtid;
        boost::shared_ptr<internal::ConnFactory> mconnf;
        boost::shared_ptr<MemberFactory> mmembf;
        boost::shared_ptr<ValueFactory> mdsf;
        boost::shared_ptr<CompositionFactory> mcompf;
        boost::shared_ptr<StreamFactory> mstrf;
    };

}}

#endif
