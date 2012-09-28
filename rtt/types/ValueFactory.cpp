#include "ValueFactory.hpp"
#include "rtt-config.h"

namespace RTT
{
    using namespace std;
    using namespace detail;
    using namespace internal;

    AttributeBase* ValueFactory::buildVariable(std::string name, int ) const {
        return this->buildVariable(name);
    }

    AttributeBase* ValueFactory::buildConstant(std::string name,DataSourceBase::shared_ptr dsb, int ) const {
        return this->buildConstant(name, dsb );
    }
}
