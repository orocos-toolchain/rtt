#ifndef RTT_VALUE_TYPE_FACTORY
#define RTT_VALUE_TYPE_FACTORY

#include "../base/DataSourceBase.hpp"
#include "../base/AttributeBase.hpp"
#include "../base/PropertyBase.hpp"
#include "../rtt-config.h"
#include <vector>
#include <string>

namespace RTT
{
    namespace types {

        /**
         * Creates all value objects of a certain type.
         */
        class RTT_API ValueFactory
        {
        public:
            virtual ~ValueFactory() {}
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

        };

        typedef boost::shared_ptr<ValueFactory> ValueFactoryPtr;
    }
}

#endif
