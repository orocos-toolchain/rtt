
#include "POAUtility.h"

PortableServer::POA_ptr
POAUtility::create_basic_POA(
    PortableServer::POA_ptr           parentPOAP,
    PortableServer::POAManager_ptr    POAManagerP,
    const char *                            POAName,
    CORBA::Boolean                    isMultiThread,
    CORBA::Boolean                    isPersistent
)
{
    // Create a policy list.
    CORBA::PolicyList policies;
    policies.length(4);
    CORBA::ULong i = 0;

    // Thread Policy
    PortableServer::ThreadPolicyValue threadPolicy;

    if (isMultiThread) {
        threadPolicy = PortableServer::ORB_CTRL_MODEL;
    }
    else {
        threadPolicy = PortableServer::SINGLE_THREAD_MODEL;
    }
    policies[i] = parentPOAP->create_thread_policy(threadPolicy);

    // Lifespan and IdAssignment Policies
    PortableServer::LifespanPolicyValue     lifeSpanPolicy;
    PortableServer::IdAssignmentPolicyValue idAssignPolicy;
    PortableServer::ImplicitActivationPolicyValue implicitActivationPolicy;

    if (isPersistent) {
        // Policies for 'Entity' objects
        lifeSpanPolicy = PortableServer::PERSISTENT;
        idAssignPolicy = PortableServer::USER_ID;
    }
    else {
        // Policies for 'Session' objects
        lifeSpanPolicy = PortableServer::TRANSIENT;
        idAssignPolicy = PortableServer::SYSTEM_ID;
    }

    implicitActivationPolicy = PortableServer::IMPLICIT_ACTIVATION;

    // Lifespan Policy
    i++;
    policies[i] = parentPOAP->create_lifespan_policy(lifeSpanPolicy);

    // IdAssignment Policy
    i++;
    policies[i] = parentPOAP->create_id_assignment_policy(idAssignPolicy);

    // IdUniqueness Policy        -  Default = UNIQUE_ID

    // ImplicitActivation Policy  -  Default = NO_IMPLICIT_ACTIVATION
    // Override by PS:
    i++;
    policies[i] = parentPOAP->create_implicit_activation_policy(implicitActivationPolicy);
    

    // RequestProcessing Policy   -  Default = USE_ACTIVE_OBJECT_MAP_ONLY

    // ServantRetention Policy    -  Default = RETAIN
 
    return parentPOAP->create_POA(POAName, POAManagerP, policies);
}


PortableServer::POA_ptr
POAUtility::create_service_POA(
    PortableServer::POA_ptr           parentPOAP,
    PortableServer::POAManager_ptr    POAManagerP,
    const char *                            POAName,
    CORBA::Boolean                    isMultiThread
)
{
    // Create a policy list.
    CORBA::PolicyList policies;
    policies.length(2);
    CORBA::ULong i = 0;

    // Thread Policy
    PortableServer::ThreadPolicyValue threadPolicy;

    if (isMultiThread) {
        threadPolicy = PortableServer::ORB_CTRL_MODEL;
    }
    else {
        threadPolicy = PortableServer::SINGLE_THREAD_MODEL;
    }
    policies[i] = parentPOAP->create_thread_policy(threadPolicy);

    // LifeSpan Policy            -  Default = TRANSIENT

    // IdAssignment Policy        -  Default = SYSTEM_ID

    // IdUniqueness Policy
    i++;
    policies[i] = parentPOAP->create_id_uniqueness_policy(
        PortableServer::MULTIPLE_ID
    );

    // ImplicitActivation Policy  -  Default = NO_IMPLICIT_ACTIVATION

    // RequestProcessing Policy   -  Default = USE_ACTIVE_OBJECT_MAP_ONLY

    // ServantRetention Policy    -  Default = RETAIN

    return parentPOAP->create_POA(POAName, POAManagerP, policies);
}


PortableServer::POA_ptr
POAUtility::create_servant_activator_POA(
    PortableServer::POA_ptr           parentPOAP,
    PortableServer::POAManager_ptr    POAManagerP,
    const char *                            POAName,
    CORBA::Boolean                    isMultiThread,
    CORBA::Boolean                    isPersistent
)
{
    // Create a policy list.
    CORBA::PolicyList policies;
    policies.length(4);
    CORBA::ULong i = 0;

    // Thread Policy
    PortableServer::ThreadPolicyValue threadPolicy;

    if (isMultiThread) {
        threadPolicy = PortableServer::ORB_CTRL_MODEL;
    }
    else {
        threadPolicy = PortableServer::SINGLE_THREAD_MODEL;
    }
    policies[i] = parentPOAP->create_thread_policy(threadPolicy);

    PortableServer::LifespanPolicyValue     lifeSpanPolicy;
    PortableServer::IdAssignmentPolicyValue idAssignPolicy;

    // Lifespan and IdAssignment Policies
    if (isPersistent) {
        // Policies for 'Entity' objects
        lifeSpanPolicy = PortableServer::PERSISTENT;
        idAssignPolicy = PortableServer::USER_ID;
    }
    else {
        // Policies for 'Session' objects
        lifeSpanPolicy = PortableServer::TRANSIENT;
        idAssignPolicy = PortableServer::SYSTEM_ID;
    }

    // Lifespan Policy
    i++;
    policies[i] = parentPOAP->create_lifespan_policy(lifeSpanPolicy);

    // IdAssignment Policy
    i++;
    policies[i] = parentPOAP->create_id_assignment_policy(idAssignPolicy);

    // IdUniqueness Policy        -  Default = UNIQUE_ID

    // ImplicitActivation Policy  -  Default = NO_IMPLICIT_ACTIVATION

    // RequestProcessing Policy
    i++;
    policies[i] = parentPOAP->create_request_processing_policy(
        PortableServer::USE_SERVANT_MANAGER
    );

    // ServantRetention Policy    -  Default = RETAIN

    return parentPOAP->create_POA(POAName, POAManagerP, policies);
}


PortableServer::POA_ptr
POAUtility::create_servant_locator_POA(
    PortableServer::POA_ptr           parentPOAP,
    PortableServer::POAManager_ptr    POAManagerP,
    const char *                            POAName,
    CORBA::Boolean                    isMultiThread,
    CORBA::Boolean                    isPersistent
)
{
    // Create a policy list.
    CORBA::PolicyList policies;
    policies.length(5);
    CORBA::ULong i = 0;

    // Thread Policy
    PortableServer::ThreadPolicyValue threadPolicy;

    if (isMultiThread) {
        threadPolicy = PortableServer::ORB_CTRL_MODEL;
    }
    else {
        threadPolicy = PortableServer::SINGLE_THREAD_MODEL;
    }
    policies[i] = parentPOAP->create_thread_policy(threadPolicy);

    PortableServer::LifespanPolicyValue     lifeSpanPolicy;
    PortableServer::IdAssignmentPolicyValue idAssignPolicy;

    // Lifespan and IdAssignment Policies
    if (isPersistent) {
        // Policies for 'Entity' objects
        lifeSpanPolicy = PortableServer::PERSISTENT;
        idAssignPolicy = PortableServer::USER_ID;
    }
    else {
        // Policies for 'Session' objects
        lifeSpanPolicy = PortableServer::TRANSIENT;
        idAssignPolicy = PortableServer::SYSTEM_ID;
    }

    // Lifespan Policy
    i++;
    policies[i] = parentPOAP->create_lifespan_policy(lifeSpanPolicy);

    // IdAssignment Policy
    i++;
    policies[i] = parentPOAP->create_id_assignment_policy(idAssignPolicy);

    // IdUniqueness Policy        -  Default = UNIQUE_ID

    // ImplicitActivation Policy  -  Default = NO_IMPLICIT_ACTIVATION

    // RequestProcessing Policy
    i++;
    policies[i] = parentPOAP->create_request_processing_policy(
        PortableServer::USE_SERVANT_MANAGER
    );

    // ServantRetention Policy
    i++;
    policies[i] = parentPOAP->create_servant_retention_policy(
        PortableServer::NON_RETAIN
    );

    return parentPOAP->create_POA(POAName, POAManagerP, policies);
}


PortableServer::POA_ptr
POAUtility::create_default_servant_POA(
    PortableServer::POA_ptr           parentPOAP,
    PortableServer::POAManager_ptr    POAManagerP,
    const char *                            POAName,
    CORBA::Boolean                    isMultiThread
)
{
    // Create a policy list.
    CORBA::PolicyList policies;
    policies.length(3);
    CORBA::ULong i = 0;

    // Thread Policy
    PortableServer::ThreadPolicyValue threadPolicy;

    if (isMultiThread) {
        threadPolicy = PortableServer::ORB_CTRL_MODEL;
    }
    else {
        threadPolicy = PortableServer::SINGLE_THREAD_MODEL;
    }
    policies[i] = parentPOAP->create_thread_policy(threadPolicy);

    // LifeSpan Policy            -  Default = TRANSIENT

    // IdAssignment Policy        -  Default = SYSTEM_ID

    // IdUniqueness Policy        -  Default = UNIQUE_ID

    // ImplicitActivation Policy  -  Default = NO_IMPLICIT_ACTIVATION

    // RequestProcessing Policy
    i++;
    policies[i] = parentPOAP->create_request_processing_policy(
        PortableServer::USE_DEFAULT_SERVANT
    );

    // ServantRetention Policy
    i++;
    policies[i] = parentPOAP->create_servant_retention_policy(
        PortableServer::NON_RETAIN
    );

    return parentPOAP->create_POA(POAName, POAManagerP, policies);
}


