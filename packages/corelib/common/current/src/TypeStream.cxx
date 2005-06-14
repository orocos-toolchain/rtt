#include <corelib/TypeStream.hpp>
#include <ostream>

namespace ORO_CoreLib
{
    std::ostream& operator<<(std::ostream& os, const std::vector<double>& v)
    {
        os << "{";
        for(unsigned int  i = 0; i < v.size(); ++i) {
            os << v[i];
            if (i + 1 != v.size() )
                os << ", ";
        }
        os << "}";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const ORO_CoreLib::Double6D& v)
    {
        os << "[";
        for(unsigned int  i = 0; i < v.size; ++i) {
            os << v[i];
            if (i + 1 != v.size )
                os << ", ";
        }
        os << "]";
        return os;
    }


}

