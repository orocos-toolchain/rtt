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

void OperationsFixture::createOperationCallerFactories1(TaskContext* target)
{
    Service::shared_ptr to = target->provides("methods");
    // ClientThread
    to->addOperation("m1r", &OperationsFixture::m1r, this).doc("M1r");
    to->addOperation("m1cr", &OperationsFixture::m1cr, this).doc("M1cr");

    to->addOperation("m1", &OperationsFixture::m1, this).doc("M1").arg("a", "ad");

    // OwnThread
    to->addOperation("o1r", &OperationsFixture::m1r, this, OwnThread).doc("M1r");
    to->addOperation("o1cr", &OperationsFixture::m1cr, this, OwnThread).doc("M1cr");

    to->addOperation("o1", &OperationsFixture::m1, this, OwnThread).doc("M1").arg("a", "ad");
}
