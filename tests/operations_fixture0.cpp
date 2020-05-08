/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  operations_fixture0.cpp

                        operations_fixture0.cpp -  description
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

void OperationsFixture::createOperationCallerFactories0(TaskContext* target)
{
    Service::shared_ptr to = target->provides("methods");
    // ClientThread
    to->addOperation("m0r", &OperationsFixture::m0r, this).doc("M0r");
    to->addOperation("m0cr", &OperationsFixture::m0cr, this).doc("M0cr");
    to->addOperation("vm0", &OperationsFixture::vm0, this).doc("VoidM0");
    to->addOperation("m0", &OperationsFixture::m0, this).doc("M0");
    to->addOperation("m0except", &OperationsFixture::m0except, this).doc("M0Except");

    // OwnThread
    to->addOperation("o0r", &OperationsFixture::m0r, this, OwnThread).doc("M0r");
    to->addOperation("o0cr", &OperationsFixture::m0cr, this, OwnThread).doc("M0cr");
    to->addOperation("o0", &OperationsFixture::m0, this, OwnThread).doc("M0");
    to->addOperation("vo0", &OperationsFixture::vm0, this, OwnThread).doc("VoidM0");
    to->addOperation("o0except", &OperationsFixture::m0except, this, OwnThread).doc("M0Except");
}
