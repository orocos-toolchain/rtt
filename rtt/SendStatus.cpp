#include "SendStatus.hpp"

#include <string>

using namespace std;

namespace RTT {
    std::ostream& operator<<(std::ostream& os, SendStatus fs)
    {
        switch (fs) {
        case SendFailure:
            os << "SendFailure";
            break;
        case SendNotReady:
            os << "SendNotReady";
            break;
        case SendSuccess:
            os << "SendSuccess";
            break;
        }
        return os;
    }

    std::istream& operator>>(std::istream& is, SendStatus& fs)
    {
        // default:
        fs = SendFailure;
        std::string s;
        is >> s;
        if (s == "SendNotReady")
            fs = SendNotReady;
        else if (s == "SendSuccess")
            fs = SendSuccess;

        return is;
    }
}

