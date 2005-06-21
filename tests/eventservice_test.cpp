#include <corelib/Event.hpp>
#include <corelib/Logger.hpp>
#include <corelib/PriorityThread.hpp>
#include <corelib/RunnableInterface.hpp>
#include <corelib/TaskSimulation.hpp>
#include <corelib/SimulationThread.hpp>
#include <corelib/TaskNonPeriodic.hpp>

#include <execution/EventService.hpp>
#include <execution/DataSourceGenerator.hpp>

#include "eventservice_test.hpp"
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( EventServiceTest );

using namespace ORO_Execution;
using namespace ORO_CoreLib;
using namespace boost;
using namespace std;

void 
EventServiceTest::setUp()
{
    t_event0 = new Event<void(void)>();
    t_event1 = new ORO_CoreLib::Event<void( std::string )>();
    t_event2 = new ORO_CoreLib::Event<void( std::string, double )>();
    t_event3 = new ORO_CoreLib::Event<void( std::string, double, bool )>();

    event_proc = new EventProcessor();
    event_proc->initialize();
    es = new EventService();
    reset();
}


void 
EventServiceTest::tearDown()
{
    delete t_event0;
    delete t_event1;
    delete t_event2;
    delete t_event3;
    delete es;
    event_proc->finalize();
    delete event_proc;
}

void EventServiceTest::listener0(void)
{
    t_listener_done = true;
}

void EventServiceTest::completer0(void)
{
    t_completer_done = true;
}

void EventServiceTest::listener1(const std::string& s)
{
    t_listener_string=s;
}
void EventServiceTest::completer1(const std::string&s) 
{
    t_completer_string=s;
}
void EventServiceTest::listener2(const std::string& s, double f )
{
    t_listener_string=s;
    t_listener_double=f;
}
void EventServiceTest::completer2(const std::string& s, double f)
{
    t_completer_string=s;
    t_completer_double=f;
}

void EventServiceTest::listener3(std::string s, double f, bool b)
{
    t_listener_string=s;
    t_listener_double=f;
    t_listener_bool=b;
}
void EventServiceTest::completer3(std::string s, double f, bool b )
{
    t_completer_string=s;
    t_completer_double=f;
    t_completer_bool=b;
}

void EventServiceTest::reset()
{
    t_listener_done = false;
    t_completer_done = false;
    t_listener_string = "";
    t_listener_double = 0.0;
    t_listener_bool = false;
    t_completer_string = "";
    t_completer_double = 0.0;
    t_completer_bool = false;
}

void EventServiceTest::testAddRemove()
{
    bool result;
    result = es->addEvent( "t_event0", t_event0 );
    CPPUNIT_ASSERT( result );
    result = es->addEvent( "t_event1", t_event1 );
    CPPUNIT_ASSERT( result );
    result = es->addEvent( "t_event2", t_event2 );
    CPPUNIT_ASSERT( result );
    result = es->addEvent( "t_event3", t_event3 );
    CPPUNIT_ASSERT( result );

    result = es->addEvent( "t_event0", t_event0 );
    CPPUNIT_ASSERT( result == false );

    result = es->removeEvent( "t_event0" );
    CPPUNIT_ASSERT( result );
    result = es->removeEvent( "t_event1" );
    CPPUNIT_ASSERT( result );
    result = es->removeEvent( "t_event2" );
    CPPUNIT_ASSERT( result );
    result = es->removeEvent( "t_event3" );
    CPPUNIT_ASSERT( result );

    result = es->removeEvent( "t_event0" );
    CPPUNIT_ASSERT( result == false );
}

void EventServiceTest::setup()
{
    es->addEvent( "t_event0", t_event0 );
    es->addEvent( "t_event1", t_event1 );
    es->addEvent( "t_event2", t_event2 );
    es->addEvent( "t_event3", t_event3 );
}
void EventServiceTest::testSetupSyn()
{
    this->setup();
    Handle h;
    h = es->setupSyn("t_event0", bind(&EventServiceTest::listener0,this), vector<DataSourceBase::shared_ptr>() );
    CPPUNIT_ASSERT( h );
    h = es->setupSyn("t_event1", bind(&EventServiceTest::listener0,this), GenerateDataSource()(ref(t_listener_string)));
    CPPUNIT_ASSERT( h );
    h = es->setupSyn("t_event2", bind(&EventServiceTest::listener0,this), GenerateDataSource()(ref(t_listener_string), ref(t_listener_double)));
    CPPUNIT_ASSERT( h );
    h = es->setupSyn("t_event3", bind(&EventServiceTest::listener0,this), GenerateDataSource()(ref(t_listener_string), ref(t_listener_double),ref(t_listener_bool)));
    CPPUNIT_ASSERT( h );

    this->cleanup();
}

void EventServiceTest::cleanup()
{
    es->removeEvent( "t_event0" );
    es->removeEvent( "t_event1" );
    es->removeEvent( "t_event2" );
    es->removeEvent( "t_event3" );
}
void EventServiceTest::testSetupAsyn()
{
    this->setup();
    Handle h;
    h = es->setupAsyn("t_event0", bind(&EventServiceTest::completer0,this), vector<DataSourceBase::shared_ptr>(),event_proc );
    CPPUNIT_ASSERT( h );
    h = es->setupAsyn("t_event1", bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_completer_string)),event_proc);
    CPPUNIT_ASSERT( h );
    h = es->setupAsyn("t_event2", bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_completer_string), ref(t_completer_double)),event_proc);
    CPPUNIT_ASSERT( h );
    h = es->setupAsyn("t_event3", bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_completer_string), ref(t_completer_double), ref(t_completer_bool) ),event_proc);
    CPPUNIT_ASSERT( h );

    this->cleanup();
}
void EventServiceTest::testSetupSynAsyn()
{
    this->setup();

    Handle h;
    h = es->setupSynAsyn("t_event0", bind(&EventServiceTest::listener0,this), bind(&EventServiceTest::completer0,this), vector<DataSourceBase::shared_ptr>(),event_proc );
    CPPUNIT_ASSERT( h );
    h = es->setupSynAsyn("t_event1", bind(&EventServiceTest::listener0,this), bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_listener_string)),event_proc);
    CPPUNIT_ASSERT( h );
    h = es->setupSynAsyn("t_event2", bind(&EventServiceTest::listener0,this), bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_listener_string), ref(t_listener_double)),event_proc);
    CPPUNIT_ASSERT( h );
    h = es->setupSynAsyn("t_event3", bind(&EventServiceTest::listener0,this), bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_listener_string), ref(t_listener_double), ref(t_listener_bool)),event_proc);
    CPPUNIT_ASSERT( h );

    this->cleanup();
}

void EventServiceTest::testSetupEmit()
{
    this->setup();

    DataSourceBase::shared_ptr r;
    r = ( es->setupEmit("t_event0", std::vector<DataSourceBase::shared_ptr>() ) );
    CPPUNIT_ASSERT( r );
    r = ( es->setupEmit("t_event1", GenerateDataSource()(std::string("hello")) ) );
    CPPUNIT_ASSERT( r );
    r = ( es->setupEmit("t_event2", GenerateDataSource()(std::string("hello"),0.1234) ) );
    CPPUNIT_ASSERT( r );
    r = ( es->setupEmit("t_event3", GenerateDataSource()(std::string("hello"),0.1234, true) ) );
    CPPUNIT_ASSERT( r );

    this->cleanup();
}

void EventServiceTest::testEmit0()
{
    this->setup();

    Handle h1, h2, h3;
    h1 = es->setupSyn("t_event0", bind(&EventServiceTest::listener0,this), vector<DataSourceBase::shared_ptr>() );
    h2 = es->setupAsyn("t_event0", bind(&EventServiceTest::completer0,this), vector<DataSourceBase::shared_ptr>(),event_proc );
    h3 = es->setupSynAsyn("t_event0", bind(&EventServiceTest::listener0,this), bind(&EventServiceTest::completer0,this), vector<DataSourceBase::shared_ptr>(),event_proc );
    DataSourceBase::shared_ptr r;
    r = ( es->setupEmit("t_event0", std::vector<DataSourceBase::shared_ptr>() ) );

    CPPUNIT_ASSERT( h1.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( t_listener_done );
    this->reset();
    CPPUNIT_ASSERT( h1.disconnect() );

    CPPUNIT_ASSERT( h2.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( !t_completer_done );
    event_proc->step();
    CPPUNIT_ASSERT( t_completer_done );
    this->reset();
    CPPUNIT_ASSERT( h2.disconnect() );

    CPPUNIT_ASSERT( h3.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( !t_completer_done );
    CPPUNIT_ASSERT( t_listener_done );
    event_proc->step();
    CPPUNIT_ASSERT( t_completer_done );
    this->reset();
    CPPUNIT_ASSERT( h3.disconnect() );

    this->cleanup();
}

void EventServiceTest::testEmit1()
{
    this->setup();

    Handle h1, h2, h3;
    DataSourceBase::shared_ptr r;

    h1 = es->setupSyn("t_event1", bind(&EventServiceTest::listener0,this), GenerateDataSource()(ref(t_listener_string)));
    h2 = es->setupAsyn("t_event1", bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_completer_string)),event_proc);
    h3 = es->setupSynAsyn("t_event1", bind(&EventServiceTest::listener0,this), bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_listener_string)),event_proc);
    r = ( es->setupEmit("t_event1", GenerateDataSource()(std::string("hello")) ) );

    CPPUNIT_ASSERT( h1.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( t_listener_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    this->reset();
    CPPUNIT_ASSERT( h1.disconnect() );

    CPPUNIT_ASSERT( h2.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( !t_completer_done );
    event_proc->step();
    CPPUNIT_ASSERT( t_completer_done );
    CPPUNIT_ASSERT( t_completer_string == std::string("hello") );
    this->reset();
    CPPUNIT_ASSERT( h2.disconnect() );

    CPPUNIT_ASSERT( h3.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( t_listener_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    CPPUNIT_ASSERT( !t_completer_done );
    this->reset();
    event_proc->step();
    CPPUNIT_ASSERT( t_completer_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    this->reset();
    CPPUNIT_ASSERT( h3.disconnect() );

    this->cleanup();
}
void EventServiceTest::testEmit2()
{
    this->setup();
    Handle h1, h2, h3;
    DataSourceBase::shared_ptr r;

    h1 = es->setupSyn("t_event2", bind(&EventServiceTest::listener0,this), GenerateDataSource()(ref(t_listener_string), ref(t_listener_double)));
    h2 = es->setupAsyn("t_event2", bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_completer_string), ref(t_completer_double)),event_proc);
    h3 = es->setupSynAsyn("t_event2", bind(&EventServiceTest::listener0,this), bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_listener_string), ref(t_listener_double)),event_proc);

    r = es->setupEmit("t_event2", GenerateDataSource()(std::string("hello"),0.1234) );


    CPPUNIT_ASSERT( h1.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( t_listener_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    CPPUNIT_ASSERT( t_listener_double == 0.1234 );
    this->reset();
    CPPUNIT_ASSERT( h1.disconnect() );

    CPPUNIT_ASSERT( h2.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( !t_completer_done );
    event_proc->step();
    CPPUNIT_ASSERT( t_completer_done );
    CPPUNIT_ASSERT( t_completer_string == std::string("hello") );
    CPPUNIT_ASSERT( t_completer_double == 0.1234 );
    this->reset();
    CPPUNIT_ASSERT( h2.disconnect() );

    CPPUNIT_ASSERT( h3.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( t_listener_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    CPPUNIT_ASSERT( t_listener_double == 0.1234 );
    CPPUNIT_ASSERT( !t_completer_done );
    this->reset();
    event_proc->step();
    CPPUNIT_ASSERT( t_completer_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    CPPUNIT_ASSERT( t_listener_double == 0.1234 );
    this->reset();
    CPPUNIT_ASSERT( h3.disconnect() );

    this->cleanup();
}

void EventServiceTest::testEmit3()
{
    this->setup();
    Handle h1, h2, h3;
    DataSourceBase::shared_ptr r;

    h1 = es->setupSyn("t_event3", bind(&EventServiceTest::listener0,this), GenerateDataSource()(ref(t_listener_string), ref(t_listener_double),ref(t_listener_bool)));
    h2 = es->setupAsyn("t_event3", bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_completer_string), ref(t_completer_double), ref(t_completer_bool) ),event_proc);
    h3 = es->setupSynAsyn("t_event3", bind(&EventServiceTest::listener0,this), bind(&EventServiceTest::completer0,this), GenerateDataSource()(ref(t_listener_string), ref(t_listener_double), ref(t_listener_bool)),event_proc);
    r = ( es->setupEmit("t_event3", GenerateDataSource()(std::string("hello"),0.1234, true) ) );


    CPPUNIT_ASSERT( h1.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( t_listener_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    CPPUNIT_ASSERT( t_listener_double == 0.1234 );
    CPPUNIT_ASSERT( t_listener_bool == true );
    this->reset();
    CPPUNIT_ASSERT( h1.disconnect() );

    CPPUNIT_ASSERT( h2.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( !t_completer_done );
    event_proc->step();
    CPPUNIT_ASSERT( t_completer_done );
    CPPUNIT_ASSERT( t_completer_string == std::string("hello") );
    CPPUNIT_ASSERT( t_completer_double == 0.1234 );
    CPPUNIT_ASSERT( t_completer_bool == true );
    this->reset();
    CPPUNIT_ASSERT( h2.disconnect() );

    CPPUNIT_ASSERT( h3.connect() );
    r->evaluate();
    CPPUNIT_ASSERT( t_listener_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    CPPUNIT_ASSERT( t_listener_double == 0.1234 );
    CPPUNIT_ASSERT( t_listener_bool == true );
    CPPUNIT_ASSERT( !t_completer_done );
    this->reset();
    event_proc->step();
    CPPUNIT_ASSERT( t_completer_done );
    CPPUNIT_ASSERT( t_listener_string == std::string("hello") );
    CPPUNIT_ASSERT( t_listener_double == 0.1234 );
    CPPUNIT_ASSERT( t_listener_bool == true );
    this->reset();
    CPPUNIT_ASSERT( h3.disconnect() );

    this->cleanup();
}
