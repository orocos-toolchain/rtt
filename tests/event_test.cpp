
#include <corelib/Event.hpp>
#include <corelib/PriorityThread.hpp>

#include "event_test.hpp"
#include <boost/bind.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( EventTest );

using namespace ORO_CoreLib;

void 
EventTest::setUp()
{
    t_event = new Event<void(void)>();
    t_event_string = new Event<void(std::string)>();
    event_proc = new EventProcessor();
    reset();
}


void 
EventTest::tearDown()
{
    delete t_event;
    delete t_event_string;
    delete event_proc;
}

void EventTest::listener(void)
{
    t_listener_value = true;
}

void EventTest::listenerString(const std::string& what)
{
    t_listener_what = what;
}

void EventTest::completer(void)
{
    t_completer_value = true;
}

void EventTest::reset()
{
    t_listener_value = false;
    t_completer_value = false;
    t_listener_what = "";
}

void 
EventTest::testEmpty()
{
    t_event->fire();
}

void EventTest::testSyncListener()
{
    // No completer:
    reset();
    Handle h = t_event->connect( boost::bind(&EventTest::listener, this) );
    CPPUNIT_ASSERT( h.connected() );
    t_event->fire();
    h.disconnect();
    CPPUNIT_ASSERT( !h.connected() );

    CPPUNIT_ASSERT( t_listener_value );
    CPPUNIT_ASSERT( !t_completer_value );
}

void EventTest::testSyncListenerThreadCompleter()
{
    // in thread completer:
    reset();
    // Manually call step
    event_proc->step();
    Handle h = t_event->connect( boost::bind(&EventTest::listener,this),
                                 boost::bind(&EventTest::completer,this),
                                 event_proc );
    CPPUNIT_ASSERT( h.connected() );

    // Manually call step
    event_proc->step();

    t_event->fire();

    // Manually call step
    event_proc->step();

    // This will block until all completers are processed.
    h.disconnect();
    CPPUNIT_ASSERT( !h.connected() );

    // Manually call step
    event_proc->step();

    // now, both must be called.
    CPPUNIT_ASSERT( t_listener_value );
    CPPUNIT_ASSERT( t_completer_value );
}

void EventTest::testSyncListenerString()
{
    // No completer:
    reset();
    Handle h = t_event_string->connect( boost::bind(&EventTest::listenerString,this,_1) );
    CPPUNIT_ASSERT( h.connected() );
    t_event_string->fire( std::string("What") );
    h.disconnect();
    CPPUNIT_ASSERT( !h.connected() );

    CPPUNIT_ASSERT_EQUAL( t_listener_what, std::string("What") );
    CPPUNIT_ASSERT( !t_listener_value );
    CPPUNIT_ASSERT( !t_completer_value );
}

    

