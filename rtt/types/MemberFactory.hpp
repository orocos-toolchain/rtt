#ifndef ORO_MEMBER_FACTORY_HPP
#define ORO_MEMBER_FACTORY_HPP

#include "../base/DataSourceBase.hpp"
#include "../rtt-config.h"
#include "../internal/Reference.hpp"
#include <vector>
#include <string>

namespace RTT
{
    namespace types
    {

        class RTT_API MemberFactory
        {
        public:
        virtual ~MemberFactory() {};

        /**
         * @name Inspecting data structures.
         * Used to write a complex type to an external representation, like XML.
         * @{
         */

        /**
         * Tries to resize a data source in case it's a resizable sequence.
         * @return true if the resizing could be done, false otherwise.
         */
        virtual bool resize(base::DataSourceBase::shared_ptr arg, int size) const;

        /**
         * Returns the list of struct member names of this type.
         * In case this type is not a struct, returns an empty list.
         */
        virtual std::vector<std::string> getMemberNames() const;

        /**
         * Returns a member of a given data source struct identified by its name.
         * @param item The item of which to return a reference to a member
         * @param name The name of a member within \a item. Is a name of a member in case of a struct
         * or an index number in case of a sequence.
         * @return null if no such member exists, an assignable datasource referencing that member otherwise.
         */
        virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const;

        /**
         * Returns a member of a given data source identified by a data source \a id. This will be an
         * int (for indexing) or a string (for the member name).
         * @warning Currently this only works for a SequenceTypeInfo-like object. We don't
         * implement this for StructTypeInfo-like objects, since that would be a very expensive
         * implementation (memory wise).
         *
         * @param item The item of which to return a member
         * @param id   Or a string data source containing the name of a member if item is a struct,
         * Or an unsigned int data source containing the index of an element if item is a sequence
         */
        virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                           base::DataSourceBase::shared_ptr id) const;


        /**
         * Stores a reference to a member of a struct identified by its name.
         * This method does not allocate memory when \a item is assignable..
         * @param ref The reference object in which to store the reference.
         * @param item The item of which to return a reference to a member. It must be assignable, otherwise,
         * a reference to a copy of item will be returned. This copy \b will allocate memory.
         * @param name The name of a member within \a item.
         * @return false if no such member exists, true if ref got filled in otherwise.
         */
        virtual bool getMember(internal::Reference* ref, base::DataSourceBase::shared_ptr item, const std::string& name) const;
        /** @} */
        };

        typedef boost::shared_ptr<MemberFactory> MemberFactoryPtr;
    }
}
#endif
