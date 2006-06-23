#include "corelib/DataSources.hpp"

namespace ORO_CoreLib
{

    template<>
    void ValueDataSource<std::string>::set( AssignableDataSource<std::string>::param_t t )
    {
        mdata = t.c_str();
    }

}
