#include "SendHandleAlias.hpp"

namespace RTT
{
    using namespace detail;

    SendHandleAlias::SendHandleAlias(const std::string& name,
                                     DataSourceBase::shared_ptr d,
                                     internal::OperationFactoryPart* opf) :
        base::AttributeBase(name), data(d), fact(opf)
    {
    }

    base::DataSourceBase::shared_ptr SendHandleAlias::getDataSource() const
    {
        return data;
    }

    internal::OperationFactoryPart* SendHandleAlias::getFactory() const
    {
        return fact;
    }

    SendHandleAlias* SendHandleAlias::clone() const
    {
        return new SendHandleAlias(mname, data.get(), fact);
    }
    SendHandleAlias* SendHandleAlias::copy(std::map<
            const base::DataSourceBase*, base::DataSourceBase*>& replacements,
                                           bool)
    {
        // instantiate does not apply.
        return new SendHandleAlias(mname, data->copy(replacements), fact);
    }

}
