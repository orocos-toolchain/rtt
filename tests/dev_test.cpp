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
 
 

#include "dev_test.hpp"
#include "FakeAnalogDevice.hpp"
#include "FakeDigitalDevice.hpp"
#include "dev/AnalogInput.hpp"
#include "dev/AnalogOutput.hpp"
#include "dev/DigitalInput.hpp"
#include "dev/DigitalOutput.hpp"

#include <unistd.h>
#include <iostream>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DevTest );

using namespace RTT;

void 
DevTest::setUp()
{
}


void 
DevTest::tearDown()
{
}

void DevTest::testClasses()
{
    DigitalInput din(true, false); // init, invert
    DigitalOutput dout(false); // init.
    AnalogInput ain(0,0);
    AnalogOutput aout(0,0);


    CPPUNIT_ASSERT( din.isOn() );

    CPPUNIT_ASSERT( dout.isOn() == false);
    dout.setBit(true);
    CPPUNIT_ASSERT( dout.isOn() == true);
    dout.setBit(false);
    CPPUNIT_ASSERT( dout.isOn() == false);
}

void DevTest::testNaming()
{

    FakeAnalogDevice fad;
    FakeDigitalDevice fdd;

    AnalogInInterface* aii = AnalogInInterface::nameserver.getObject("FakeAnalogDevice");
    AnalogOutInterface* aoi = AnalogOutInterface::nameserver.getObject("FakeAnalogDevice");

    DigitalInInterface* dii = DigitalInInterface::nameserver.getObject("FakeDigitalDevice");
    DigitalOutInterface* doi = DigitalOutInterface::nameserver.getObject("FakeDigitalDevice");

    CPPUNIT_ASSERT( aii );
    CPPUNIT_ASSERT( aoi );
    CPPUNIT_ASSERT( dii );
    CPPUNIT_ASSERT( doi );

}

