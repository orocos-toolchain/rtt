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

OperationsFixture::OperationsFixture()
{
    ret = 0.0;
    i = -1;
    tc = new TaskContext("root");
    this->createOperationCallerFactories(tc);
    tc->provides()->addAttribute("ret", ret );
    caller = new TaskContext("caller");
    caller->start();
    tc->start();
}

OperationsFixture::~OperationsFixture()
{
    tc->stop();
    delete tc;
    delete caller;
}

void OperationsFixture::createOperationCallerFactories(TaskContext* target)
{
    Service::shared_ptr dat = Service::Create("test");
    dat->addAttribute("i", i);
    dat->addOperation("assert", &OperationsFixture::assertBool, this).doc("assert").arg("b", "bd");
    dat->addOperation("assertEqual", &OperationsFixture::assertEqual, this);
    dat->addOperation("increase", &OperationsFixture::increase, this).doc("Return increasing i");
    dat->addOperation("increaseCmd", &OperationsFixture::increase, this, OwnThread).doc("Return increasing i");
    dat->addOperation("resetI", &OperationsFixture::resetI, this).doc("ResetI i");
    dat->addOperation("assertMsg", &OperationsFixture::assertMsg, this).doc("Assert message").arg("bool", "").arg("text", "text");
    dat->addOperation("isTrue", &OperationsFixture::isTrue, this).doc("Identity function").arg("bool", "");
    dat->addOperation("fail", &OperationsFixture::fail, this).doc("Fails by throwing.");
    dat->addOperation("good", &OperationsFixture::good, this).doc("Returns true.");
    dat->addOperation("print", &OperationsFixture::print, this).doc("Print message");
    dat->addOperation("printNumber", &OperationsFixture::printNumber, this).doc("Print message + number");
    target->provides()->addService( dat );

    createOperationCallerFactories0(target);
    createOperationCallerFactories1(target);
    createOperationCallerFactories2(target);
    createOperationCallerFactories3(target);
    createOperationCallerFactories4(target);
    createOperationCallerFactories5(target);
    createOperationCallerFactories6(target);
    createOperationCallerFactories7(target);

}
