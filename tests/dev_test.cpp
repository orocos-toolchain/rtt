/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  dev_test.cpp

                        dev_test.cpp -  description
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

#include "tests/unit.hpp"

#include "tests/dev_test.hpp"
#include "tests/FakeAnalogDevice.hpp"
#include "tests/FakeDigitalDevice.hpp"
#include "rtt/extras/dev/AnalogInput.hpp"
#include "rtt/extras/dev/AnalogOutput.hpp"
#include "rtt/extras/dev/DigitalInput.hpp"
#include "rtt/extras/dev/DigitalOutput.hpp"


#include <iostream>

using namespace std;
using namespace RTT;

void
DevTest::setUp()
{
}


void
DevTest::tearDown()
{
}

BOOST_FIXTURE_TEST_SUITE( DevTestSuite, DevTest )

BOOST_AUTO_TEST_CASE( testClasses)
{
    bool din_state = true;
    DigitalInput din(din_state, false); // init, invert
    DigitalOutput dout(false); // init.
    AnalogInput ain(0,0);
    AnalogOutput aout(0,0);


    BOOST_CHECK( din.isOn() );

    BOOST_CHECK( dout.isOn() == false);
    dout.setBit(true);
    BOOST_CHECK( dout.isOn() == true);
    dout.setBit(false);
    BOOST_CHECK( dout.isOn() == false);
}

BOOST_AUTO_TEST_CASE( testNaming)
{

    FakeAnalogDevice fad;
    FakeDigitalDevice fdd;

    AnalogInInterface* aii = AnalogInInterface::nameserver.getObject("FakeAnalogDevice");
    AnalogOutInterface* aoi = AnalogOutInterface::nameserver.getObject("FakeAnalogDevice");

    DigitalInInterface* dii = DigitalInInterface::nameserver.getObject("FakeDigitalDevice");
    DigitalOutInterface* doi = DigitalOutInterface::nameserver.getObject("FakeDigitalDevice");

    BOOST_CHECK( aii );
    BOOST_CHECK( aoi );
    BOOST_CHECK( dii );
    BOOST_CHECK( doi );

}
BOOST_AUTO_TEST_SUITE_END()
