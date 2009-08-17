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


#include <Event.hpp>
#include <extras/SlaveActivity.hpp>
#include <string>
#include <rtt-detail-fwd.hpp>

using namespace RTT::detail;

class EventTest
{
public:

    RTT::Event<void( void )> t_event;
    RTT::Event<void( std::string )> t_event_string;
    RTT::Event<int( float, float )> t_event_float;
    bool t_listener_value;
    bool t_completer_value;
    std::string t_listener_what;
    EventProcessor* event_proc;
    SlaveActivity act;
    float float_sum, float_sub;
public:

	EventTest(){ setUp();};
	~EventTest(){ tearDown();};

    void setUp();
    void tearDown();

    void reset();

    void listener();
    void listenerString(const std::string&);
    void completer();

    int float_listener(float a, float b);
    int float_completer(float a, float b);
};

#endif  // EVENTTEST_H
