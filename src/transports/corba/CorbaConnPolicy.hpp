#include "../../internal/ConnPolicy.hpp"
#include "DataFlowC.h"

static RTT::corba::CConnPolicy toCORBA(RTT::internal::ConnPolicy const& policy)
{
    RTT::corba::CConnPolicy corba_policy;
    corba_policy.type        = RTT::corba::CConnectionModel(policy.type);
    corba_policy.init        = policy.init;
    corba_policy.lock_policy = RTT::corba::CLockPolicy(policy.lock_policy);
    corba_policy.pull        = policy.pull;
    corba_policy.size        = policy.size;
    corba_policy.data_size   = policy.data_size;
    corba_policy.transport   = policy.transport;
    corba_policy.name_id     = CORBA::string_dup( policy.name_id.c_str() );
    return corba_policy;
}

static RTT::internal::ConnPolicy toRTT(RTT::corba::CConnPolicy const& corba_policy)
{
    RTT::internal::ConnPolicy policy;
    policy.type        = corba_policy.type;
    policy.init        = corba_policy.init;
    policy.lock_policy = corba_policy.lock_policy;
    policy.pull        = corba_policy.pull;
    policy.size        = corba_policy.size;
    policy.data_size   = corba_policy.data_size;
    policy.transport   = corba_policy.transport;
    policy.name_id     = corba_policy.name_id;
    return policy;
}

