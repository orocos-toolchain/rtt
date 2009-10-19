#ifndef ORO_FLOW_STATUS_HPP
#define ORO_FLOW_STATUS_HPP

#include <ostream>
#include <istream>

namespace RTT {
    /**
     * Returns the status of a data flow read.
     * NoData means that the channel is disconnected or never written to.
     * NewData means that the returned data is new data.
     * OldData means that the returned data was already read.
     */
    enum FlowStatus { NoData = 0, OldData = 1, NewData = 2};

    std::ostream& operator<<(std::ostream& os, FlowStatus fs);
    std::istream& operator>>(std::istream& os, FlowStatus& fs);
}

#endif
