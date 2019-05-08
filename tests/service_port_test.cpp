
#include <TaskContext.hpp>
#include <InputPort.hpp>
#include <OutputPort.hpp>
#include <Service.hpp>
#include <ServiceRequester.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

struct ServicePortFixture {};

// Registers the suite into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  ServicePortTestSuite,  ServicePortFixture )

/**
 * This test suite tests using ports in services
 */

class TestService : public Service {
public:
    InputPort<int> ip;
    InputPort<int> ip2;
    OutputPort<int> op;
    TestService(TaskContext* owner = 0) : Service("portservice", owner)
    {
        addPort("ip",ip).doc("ip");
        addPort("ip",ip2).doc("ip"); // overrides ip
        addPort("op",op).doc("op");
    }
};

class TestEventService : public Service {
public:
    InputPort<int> ip;
    InputPort<int> ip2;
    OutputPort<int> op;
    TestEventService(TaskContext* owner = 0) : Service("portservice", owner)
    {
        addEventPort("ip",ip).doc("ip");
        addEventPort("ip",ip2).doc("ip"); // overrides ip
        addPort("op",op).doc("op");
    }
};

BOOST_AUTO_TEST_CASE(testAddPort)
{
    TestService* ts = new TestService();
    Service::shared_ptr s( ts );
    TaskContext tc("tc");

    tc.provides()->addService( s );

    // check that last port is the real thing:
    BOOST_CHECK( tc.provides("portservice")->getPort("ip") == &ts->ip2 );

    BOOST_CHECK( tc.provides("portservice")->getPort("op") == &ts->op );
}

BOOST_AUTO_TEST_CASE(testAddPortWithOwner)
{
    TaskContext tc("tc");
    TestService* ts = new TestService( &tc );
    Service::shared_ptr s( ts );

    tc.provides()->addService( s );

    // check that last port is the real thing:
    BOOST_CHECK( tc.provides("portservice")->getPort("ip") == &ts->ip2 );

    BOOST_CHECK( tc.provides("portservice")->getPort("op") == &ts->op );
}


BOOST_AUTO_TEST_CASE(testAddEventPort)
{
    TestEventService* ts = new TestEventService();
    Service::shared_ptr s( ts );
    TaskContext tc("tc");

    tc.provides()->addService( s );

    // check that last port is the real thing:
    BOOST_CHECK( tc.provides("portservice")->getPort("ip") == &ts->ip2 );

    BOOST_CHECK( tc.provides("portservice")->getPort("op") == &ts->op );
}

BOOST_AUTO_TEST_CASE(testAddEventPortWithOwner)
{
    TaskContext tc("tc");
    TestEventService* ts = new TestEventService(&tc);
    Service::shared_ptr s( ts );

    tc.provides()->addService( s );

    // check that last port is the real thing:
    BOOST_CHECK( tc.provides("portservice")->getPort("ip") == &ts->ip2 );

    BOOST_CHECK( tc.provides("portservice")->getPort("op") == &ts->op );
}


#ifndef ORO_DISABLE_PORT_DATA_SCRIPTING

BOOST_AUTO_TEST_CASE(testUsePort)
{
    TestService* ts = new TestService();
    // should work because TestService already adds itself to children... but this memleaks
    Service::shared_ptr s1 = ts->provides();
    // should work as well because TestService inherits from enable_shared_from_raw :
    Service::shared_ptr s( ts );
    TaskContext tc("tc");

    tc.provides()->addService( s );

    ts->ip2.connectTo( &ts->op );

    // use operation interface of port:
    BOOST_REQUIRE( tc.provides()->hasService("portservice") );
    BOOST_REQUIRE( tc.provides("portservice")->hasService("op") );
    BOOST_REQUIRE( tc.provides("portservice")->provides("op")->hasOperation("write") );

    OperationCaller<WriteStatus(int const&)> write = tc.provides("portservice")->provides("op")->getOperation("write");
    BOOST_CHECK( write.ready() );
    BOOST_CHECK_EQUAL( write( 3 ), WriteSuccess );

    int result;
    OperationCaller<FlowStatus(int&)> read = tc.provides("portservice")->provides("ip")->getOperation("read");
    BOOST_CHECK( read.ready() );
    FlowStatus fs = read( result  );
    BOOST_CHECK_EQUAL( result, 3);
    BOOST_CHECK_EQUAL( fs, NewData );
}

BOOST_AUTO_TEST_CASE(testUsePortWithOwner)
{
    TaskContext tc("tc");
    TestService* ts = new TestService(&tc);
    Service::shared_ptr s( ts );

    tc.provides()->addService( s );

    ts->ip2.connectTo( &ts->op );

    // use operation interface of port:
    OperationCaller<void(int const&)> write = tc.provides("portservice")->provides("op")->getOperation("write");
    BOOST_CHECK( write.ready() );
    write( 3 );

    int result;
    OperationCaller<FlowStatus(int&)> read = tc.provides("portservice")->provides("ip")->getOperation("read");
    BOOST_CHECK( read.ready() );
    FlowStatus fs = read( result  );
    BOOST_CHECK_EQUAL( result, 3);
    BOOST_CHECK_EQUAL( fs, NewData );
}

#endif

BOOST_AUTO_TEST_SUITE_END()
