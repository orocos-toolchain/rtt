/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  slave_test.cpp

                        slave_test.cpp -  description
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


#include "unit.hpp"

#include <rtt-fwd.hpp>

#include <rtt/TaskContext.hpp>
#include <rtt/Operation.hpp>
#include <rtt/OperationCaller.hpp>
#include <rtt/extras/SlaveActivity.hpp>

#include <rtt/os/Mutex.hpp>
#include <rtt/os/Condition.hpp>
#include <rtt/os/fosi.h>

using namespace std;
using namespace RTT::detail;
using namespace RTT;

class MasterComponent : public TaskContext
{
public:
    MasterComponent() : TaskContext("master")
      , activity(0)
    {
    }

    void updateHook()
    {
        BOOST_TEST_MESSAGE( "[ENTER] MasterComponent::updateHook()" );
        if (activity) activity->execute();
        BOOST_TEST_MESSAGE( "[EXIT]  MasterComponent::updateHook()" );
    }

    void execute(RTT::base::ActivityInterface *activity)
    {
        this->activity = activity;
    }

public:
    RTT::base::ActivityInterface *activity;
};

class SlaveComponent : public TaskContext
{
public:
    SlaveComponent() : TaskContext("slave")
      , own_thread_operation_called_counter(0)
    {
        this->addOperation("ownThreadOperation", &SlaveComponent::ownThreadOperation, this, RTT::OwnThread);
        slave_operation_caller.setCaller(this->engine());
    }

    void ownThreadOperation()
    {
        BOOST_TEST_MESSAGE( "[ENTER] SlaveComponent::ownThreadOperation()" );
        own_thread_operation_called_counter++;

        if (slave_operation_caller.ready()) {
            slave_operation_caller();
        }
        BOOST_TEST_MESSAGE( "[EXIT]  SlaveComponent::ownThreadOperation()" );
    }

public:
    unsigned int own_thread_operation_called_counter;
    RTT::OperationCaller<void()> slave_operation_caller;
};

class ClientComponent : public TaskContext
{
public:
    ClientComponent() : TaskContext("client")
      , callback_operation_called_counter(0)
    {
        this->addOperation("callSlaveOperation", &ClientComponent::callSlaveOperation, this, RTT::OwnThread);
        this->addOperation("callbackOperation", &ClientComponent::callbackOperation, this, RTT::OwnThread);
        slave_operation_caller.setCaller(this->engine());
    }

    void callSlaveOperation()
    {
        BOOST_TEST_MESSAGE( "[ENTER] ClientComponent::callSlaveOperation()" );
        BOOST_REQUIRE(slave_operation_caller.ready());
        slave_operation_caller();
        BOOST_TEST_MESSAGE( "[EXIT]  ClientComponent::callSlaveOperation()" );
    }

    void callbackOperation()
    {
        BOOST_TEST_MESSAGE( "[ENTER] ClientComponent::callbackOperation()" );
        callback_operation_called_counter++;
        BOOST_TEST_MESSAGE( "[EXIT]  ClientComponent::callbackOperation()" );
    }

public:
    unsigned int callback_operation_called_counter;
    RTT::OperationCaller<void()> slave_operation_caller;
};

/**
 * Tests operation calls and functions of components running in a SlaveActivity
 */
class SlaveActivityTest
{
public:
    SlaveActivityTest()
    {
        //RTT::Logger::Instance()->setLogLevel(RTT::Logger::Debug);

        slave.setActivity(new RTT::extras::SlaveActivity(master.getActivity()));
        master.execute(slave.getActivity());
        client.slave_operation_caller = slave.getOperation("ownThreadOperation");
    }

    ~SlaveActivityTest()
    {}

public:
    MasterComponent master;
    SlaveComponent slave;
    ClientComponent client;
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  SlaveTestSuite,  SlaveActivityTest )

// Test operation call on the SlaveComponent
BOOST_AUTO_TEST_CASE( testSlaveOperationCall )
{
    RTT::OperationCaller<void()> callSlaveOperation = client.getOperation("callSlaveOperation");
    BOOST_REQUIRE( callSlaveOperation.ready() );

    // Note: master is not running!
    RTT::SendHandle<void()> handle = callSlaveOperation.send();
    int wait = 5;
    while( !handle.collectIfDone() && wait-- ) {
        sleep(1);
    }
    BOOST_CHECK( handle.collectIfDone() );
    BOOST_CHECK_EQUAL( slave.own_thread_operation_called_counter, 1 );
    BOOST_CHECK_EQUAL( client.callback_operation_called_counter, 0 );
}

// Test operation call with a callback to the ClientComponent
BOOST_AUTO_TEST_CASE( testSlaveOperationCallWithCallback )
{
    RTT::OperationCaller<void()> callSlaveOperation = client.getOperation("callSlaveOperation");
    BOOST_REQUIRE( callSlaveOperation.ready() );

    // connect SlaveComponent::slave_operation_caller to ClientComponent::callbackOperation()
    slave.slave_operation_caller = client.getOperation("callbackOperation");
    BOOST_REQUIRE( slave.slave_operation_caller.ready() );

    // Note: master is not running!
    RTT::SendHandle<void()> handle = callSlaveOperation.send();
    int wait = 5;
    while( !handle.collectIfDone() && wait-- ) {
        sleep(1);
    }
    BOOST_CHECK( handle.collectIfDone() );
    BOOST_CHECK_EQUAL( slave.own_thread_operation_called_counter, 1 );
    BOOST_CHECK_EQUAL( client.callback_operation_called_counter, 1 );
}

BOOST_AUTO_TEST_SUITE_END()
