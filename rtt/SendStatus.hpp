
#ifndef SENDSTATUS_HPP_
#define SENDSTATUS_HPP_

namespace RTT {

/**
 * Returns the status of a send() invocation.
 */
enum SendStatus {
    SendFailure = -1, SendNotReady = 0, SendSuccess = 1
};

}

#endif /* SENDSTATUS_HPP_ */
