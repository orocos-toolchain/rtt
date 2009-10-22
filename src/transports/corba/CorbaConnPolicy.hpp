#include "../../ConnPolicy.hpp"
#include "DataFlowC.h"


/**
 * Converts a RTT ConnPolicy object to a Corba CConPolicy object.
 * Policy objects are used to setup data flow connections.
 * @param policy RTT policy
 * @return Corba policy
 */
RTT::corba::CConnPolicy toCORBA(RTT::ConnPolicy const& policy);

/**
 * Converts a Corba CConnPolicy object to a RTT ConPolicy object.
 * Policy objects are used to setup data flow connections.
 * @param policy Corba policy
 * @return RTT policy
 */
RTT::ConnPolicy toRTT(RTT::corba::CConnPolicy const& corba_policy);

