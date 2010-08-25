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

void OperationsFixture::createOperationCallerFactories0(TaskContext* target)
{
    Service::shared_ptr to = target->provides("methods");
    // ClientThread
    to->addOperation("m0r", &OperationsFixture::m0r, this).doc("M0r");
    to->addOperation("m0cr", &OperationsFixture::m0cr, this).doc("M0cr");
    to->addOperation("vm0", &OperationsFixture::vm0, this).doc("VoidM0");
    to->addOperation("m0", &OperationsFixture::m0, this).doc("M0");

    // OwnThread
    to->addOperation("o0r", &OperationsFixture::m0r, this, OwnThread).doc("M0r");
    to->addOperation("o0cr", &OperationsFixture::m0cr, this, OwnThread).doc("M0cr");
    to->addOperation("o0", &OperationsFixture::m0, this, OwnThread).doc("M0");
}
