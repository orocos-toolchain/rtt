#ifndef TYPESTREAM_HPP
#define TYPESTREAM_HPP

#include <vector>
#include <corelib/MultiVector.hpp>
#include <ostream>

namespace ORO_CoreLib
{
    std::ostream& operator<<(std::ostream& os, const std::vector<double>& v);

    std::ostream& operator<<(std::ostream& os, const ORO_CoreLib::Double6D& v);

}
#endif
