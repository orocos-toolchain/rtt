/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  event_test.hpp 

                        event_test.hpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef EVENTTEST_H
#define EVENTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <Event.hpp>
#include <SlaveActivity.hpp>
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
    CPPUNIT_TEST( testBlockingTask );
    CPPUNIT_TEST( testEventArgs );
    CPPUNIT_TEST( testRTEvent );
    CPPUNIT_TEST( testSelfRemoval );
    CPPUNIT_TEST( testCrossRemoval );
    CPPUNIT_TEST_SUITE_END();

    RTT::Event<void( void )> t_event;
    RTT::Event<void( std::string )> t_event_string;
    RTT::Event<int( float, float )> t_event_float;
    bool t_listener_value;
    bool t_completer_value;
    std::string t_listener_what;
    RTT::EventProcessor* event_proc;
    RTT::SlaveActivity act;
    float float_sum, float_sub;
public:
    void setUp();
    void tearDown();

    void reset();

    void listener();
    void listenerString(const std::string&);
    void completer();

    int float_listener(float a, float b);
    int float_completer(float a, float b);

    void testEmpty();
    void testSyncListener();
    void testSyncListenerString();
    void testSyncListenerSyncCompleter();
    void testSyncListenerThreadCompleter();
    void testCompletionProcessor();
    void testTask();
    void testBlockingTask();
    void testEventArgs();
    void testRTEvent();
    void testSelfRemoval();
    void testCrossRemoval();
};


#endif  // EVENTTEST_H
