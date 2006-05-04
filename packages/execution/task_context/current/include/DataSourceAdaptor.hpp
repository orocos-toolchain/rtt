#ifndef ORO_EXECUTION_DATASOURCEADAPTOR_HPP
#define ORO_EXECUTION_DATASOURCEADAPTOR_HPP

#include <corelib/DataSourceAdaptor.hpp>

namespace ORO_Execution
{
    namespace detail {
        using ORO_CoreLib::detail::DataSourceAdaptor;
        using ORO_CoreLib::detail::AssignableDataSourceAdaptor;
    }
    using ORO_CoreLib::AdaptDataSource;
    using ORO_CoreLib::AdaptAssignableDataSource;
}

#endif

