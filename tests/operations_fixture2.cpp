#define BOOST_FUSION_INVOKE_MAX_ARITY 8
#define BOOST_FUSION_UNFUSED_MAX_ARITY 7

#include "operations_fixture.hpp"
#include <iostream>
#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include <Operation.hpp>
#include <Service.hpp>

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

void OperationsFixture::createOperationCallerFactories2(TaskContext* target)
{
    Service::shared_ptr to = target->provides("methods");
    // ClientThread
    to->addOperation("m2", &OperationsFixture::m2, this).doc("M2").arg("a", "ad").arg("a", "ad");

    // OwnThread
    to->addOperation("o2", &OperationsFixture::m2, this, OwnThread).doc("M2").arg("a", "ad").arg("a", "ad");
}
