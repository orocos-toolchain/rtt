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


#ifndef EVENT_SERVICE_TEST_H
#define EVENT_SERVICE_TEST_H


#include <Event.hpp>
#include <interface/EventService.hpp>
#include <extras/SlaveActivity.hpp>
#include <string>

using namespace RTT::detail;

class EventServiceTest
{
public:
    RTT::Event<void( void )> t_event0;
    RTT::Event<void( std::string )> t_event1;
    RTT::Event<void( std::string, double )> t_event2;
    RTT::Event<void( std::string, double, bool )> t_event3;
    bool t_listener_done;
    bool t_completer_done;
    std::string t_listener_string;
    double t_listener_double;
    bool t_listener_bool;
    std::string t_completer_string;
    double t_completer_double;
    bool t_completer_bool;
    EventProcessor* event_proc;
    EventService* es;
    SlaveActivity act;
public:
	EventServiceTest(){ setUp(); };
	~EventServiceTest(){ tearDown(); };

    void setUp();
    void tearDown();

    void setup();
    void reset();
    void cleanup();

    void listener0();
    void completer0();
    void listener1(const std::string&);
    void completer1(const std::string&);
    void listener2(const std::string&, double);
    void completer2(const std::string&, double);
    void listener3(std::string, double, bool);
    void completer3(std::string, double, bool);

    void testAddRemove();
    void testSetupSyn();
    void testSetupAsyn();
    void testSetupEmit();
    void testEmit0();
    void testEmit1();
    void testEmit2();
    void testEmit3();
    void testEventC();
};

#endif  // EVENTTEST_H
