#include "FlowStatus.hpp"

using namespace std;

namespace RTT { 
    std::ostream& operator<<(std::ostream& os, FlowStatus fs)
    {
        switch (fs) {
        case NoData:
            os << "NoData";
            break;
        case OldData:
            os << "OldData";
            break;
        case NewData:
            os << "NewData";
            break;
        }
        return os;
    }

    std::istream& operator>>(std::istream& is, FlowStatus& fs)
    {
        // default:
        fs = NoData;
        string s;
        is >> s;
        if (s == "OldData")
            fs = OldData;
        else if (s == "NewData")
            fs = NewData;

        return is;
    }
}
