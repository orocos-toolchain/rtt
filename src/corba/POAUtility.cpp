/***************************************************************************
  tag: FMTC  do nov 2 13:06:20 CET 2006  POAUtility.cpp 

                        POAUtility.cpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

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


