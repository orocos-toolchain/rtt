#include "TemplateCompositionFactory.hpp"

using namespace RTT;
using namespace RTT::types;

base::DataSourceBase::shared_ptr CompositionFactory::convertType(base::DataSourceBase::shared_ptr source) const
{
    return base::DataSourceBase::shared_ptr();
}

base::DataSourceBase::shared_ptr CompositionFactory::decomposeType(base::DataSourceBase::shared_ptr source) const
{
    // return deprecated api in case user did not implement this.
    return convertType(source);
}

