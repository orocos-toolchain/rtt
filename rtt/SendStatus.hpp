
#ifndef SENDSTATUS_HPP_
#define SENDSTATUS_HPP_

#include "rtt-config.h"

#include <ostream>
#include <istream>

namespace RTT {

/**
 * Returns the status of a send() invocation.
 */
enum SendStatus {
    SendFailure = -1, SendNotReady = 0, SendSuccess = 1
};

RTT_API std::ostream& operator<<(std::ostream& os, SendStatus fs);
RTT_API std::istream& operator>>(std::istream& os, SendStatus& fs);

}

#endif /* SENDSTATUS_HPP_ */
