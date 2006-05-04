#include "corelib/DataSources.hpp"

namespace ORO_CoreLib
{

    template<>
    void ValueDataSource<const std::string&>::set(  AssignableDataSource<const std::string&>::param_t t )
    {
        mdata = t.c_str();
    }

}
