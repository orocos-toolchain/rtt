#ifndef ORO_SENDHANDLEALIAS_HPP_
#define ORO_SENDHANDLEALIAS_HPP_

#include "../base/AttributeBase.hpp"
#include "../interface/OperationInterface.hpp"
#include "../rtt-fwd.hpp"


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
            interface::OperationInterfacePart* fact;
        public:
            SendHandleAlias(const std::string& name, base::DataSourceBase::shared_ptr d, interface::OperationInterfacePart* opf );

            base::DataSourceBase::shared_ptr getDataSource() const;
            interface::OperationInterfacePart* getFactory() const;

            SendHandleAlias* clone() const;

            SendHandleAlias* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacements, bool );
        };

    }
}

#endif /* ORO_SENDHANDLEALIAS_HPP_ */
