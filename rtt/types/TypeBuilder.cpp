
#include "TypeBuilder.hpp"

namespace RTT
{
    using namespace std;
    using namespace detail;

    TypeBuilder::~TypeBuilder() {}

    base::DataSourceBase::shared_ptr TypeBuilder::convert(base::DataSourceBase::shared_ptr arg) const
    {
        return base::DataSourceBase::shared_ptr();
    }
}
