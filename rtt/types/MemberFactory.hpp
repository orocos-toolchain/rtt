#ifndef ORO_MEMBER_FACTORY_HPP
#define ORO_MEMBER_FACTORY_HPP

#include "../base/DataSourceBase.hpp"
#include "../rtt-config.h"
#include <vector>
#include <string>

namespace RTT
{
    namespace types
    {

        class RTT_API MemberFactory
        {
        public:
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
         * Returns a member of a given data source struct identified by a data source id.
         * @param item The item of which to return a member
         * @param id   Or a string data source containing the name of a member if item is a struct,
         * Or an unsigned int data source containing the index of an element if item is a sequence
         */
        virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                           base::DataSourceBase::shared_ptr id) const;

        /** @} */
        };

        typedef boost::shared_ptr<MemberFactory> MemberFactoryPtr;
    }
}
#endif
