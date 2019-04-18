/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  operations_fixture.cpp

                        operations_fixture.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


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

class OperationCallerComponent : public TaskContext
{
    TaskContext* mtc;
public:
    OperationCallerComponent(TaskContext* tc) : TaskContext("caller"), mtc(tc) {

        // calls own thread operations :
        opc0 = tc->provides("methods")->getOperation("o0");
        opc0.setCaller( this->engine() );
        opc1 = tc->provides("methods")->getOperation("o1");
        opc1.setCaller( this->engine() );
        // client thread :
        opc2 = tc->provides("methods")->getOperation("m2");
        opc2.setCaller( this->engine() );
        opc3 = tc->provides("methods")->getOperation("m3");
        opc3.setCaller( this->engine() );

        // four combinations are possible:
        this->addOperation("o0", &OperationCallerComponent::m0, this, OwnThread);
        this->addOperation("o1", &OperationCallerComponent::m1, this, ClientThread);
        this->addOperation("o2", &OperationCallerComponent::m2, this, OwnThread);
        this->addOperation("o3", &OperationCallerComponent::m3, this, ClientThread);

    }

    void updateHook() {
        // m0 and m2 are executed in OwnThread
        // where m0 calls an OwnThread op of 'tc' and m2 calls a ClientThread op of 'tc'.

        // m1 and m3 are executed in ClientThread (so in GlobalEE when being sent),
        // where m1 calls an OwnThread op of 'tc' and m3 calls a ClientThread op of 'tc'.
        // but do set the caller to this component.
    }

    bool ready() {
        BOOST_REQUIRE( opc0.ready() );
        BOOST_REQUIRE( opc1.ready() );
        BOOST_REQUIRE( opc2.ready() );
        BOOST_REQUIRE( opc3.ready() );
        return opc0.ready() &&
               opc1.ready() &&
               opc2.ready() &&
               opc3.ready();
    }

    // plain argument tests:
    double m0(void) { return opc0(); }
    double m1(int i) { return opc1(i); }
    double m2(int i, double d) { return opc2(i,d); }
    double m3(int i, double d, bool c) { return opc3(i,d,c); }

    OperationCaller<double(void)> opc0;
    OperationCaller<double(int)> opc1;
    OperationCaller<double(int,double)> opc2;
    OperationCaller<double(int,double,bool)> opc3;

};


OperationsFixture::OperationsFixture()
{
    ret = 0.0;
    i = -1;
    tc = new TaskContext("root");
    this->createOperationCallerFactories(tc);
    tc->provides()->addAttribute("ret", ret );
    caller = new OperationCallerComponent(tc);
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
    Service::shared_ptr dat = Service::Create("test", target);
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

    createOperationCallerFactories0(target);
    createOperationCallerFactories1(target);
    createOperationCallerFactories2(target);
    createOperationCallerFactories3(target);
    createOperationCallerFactories4(target);
    createOperationCallerFactories5(target);
    createOperationCallerFactories6(target);
    createOperationCallerFactories7(target);

}
