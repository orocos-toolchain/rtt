#ifndef ORO_SENDHANDLEALIAS_HPP_
#define ORO_SENDHANDLEALIAS_HPP_

#include "../base/AttributeBase.hpp"
#include "../internal/OperationFactory.hpp"
#include "rtt-fwd.hpp"


namespace RTT {
    namespace scripting {
        /**
         * Keeps track of a DataSource which has a SendHandle
         * and the factory for creating the collect functions
         * associated with that handle.
         */
        class SendHandleAlias
            : public base::AttributeBase
        {
            base::DataSourceBase::shared_ptr data;
            internal::OperationFactoryPart* fact;
        public:
            SendHandleAlias(const std::string& name, base::DataSourceBase::shared_ptr d, internal::OperationFactoryPart* opf );

            base::DataSourceBase::shared_ptr getDataSource() const;
            internal::OperationFactoryPart* getFactory() const;

            SendHandleAlias* clone() const;

            SendHandleAlias* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacements, bool );
        };

    }
}

#endif /* ORO_SENDHANDLEALIAS_HPP_ */
