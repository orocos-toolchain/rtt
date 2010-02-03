
#ifndef SENDSTATUS_HPP_
#define SENDSTATUS_HPP_

#include <ostream>
#include <istream>

namespace RTT {

/**
 * Returns the status of a send() invocation.
 */
enum SendStatus {
    SendFailure = -1, SendNotReady = 0, SendSuccess = 1
};

std::ostream& operator<<(std::ostream& os, SendStatus fs);
std::istream& operator>>(std::istream& os, SendStatus& fs);

}

#endif /* SENDSTATUS_HPP_ */
