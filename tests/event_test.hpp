#ifndef EVENTTEST_H
#define EVENTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <corelib/Event.hpp>
#include <string>

class EventTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( EventTest );
    CPPUNIT_TEST( testEmpty );
    CPPUNIT_TEST( testSyncListener );
    CPPUNIT_TEST( testSyncListenerString );
    CPPUNIT_TEST( testSyncListenerThreadCompleter );
    CPPUNIT_TEST( testCompletionProcessor );
    CPPUNIT_TEST( testTask );
    CPPUNIT_TEST_SUITE_END();

    ORO_CoreLib::Event<void( void )>* t_event;
    ORO_CoreLib::Event<void( std::string )>* t_event_string;
    bool t_listener_value;
    bool t_completer_value;
    std::string t_listener_what;
    ORO_CoreLib::EventProcessor* event_proc;
public:
    void setUp();
    void tearDown();

    void reset();

    void listener();
    void listenerString(const std::string&);
    void completer();

    void testEmpty();
    void testSyncListener();
    void testSyncListenerString();
    void testSyncListenerSyncCompleter();
    void testSyncListenerThreadCompleter();
    void testCompletionProcessor();
    void testTask();
};

#endif  // EVENTTEST_H
