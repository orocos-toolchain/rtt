/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseSimple.cxx 

                        TestCaseSimple.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#include "TestCaseSimple.hpp"

#include <os/fosi.h>
#include <iostream>

namespace UnitTesting
{
    using namespace std;

    TestCaseSimple::TestCaseSimple( TestSuite* suite, WriteInterface* output ) 
        : TestCase( "TestCaseSimple", suite, 13 ), out( output ), stepCounter( 0 ),
          errorAssert( "The assert construction doesn't work.\n" ),
          errorStep( "Step doesn't work.\n" )
    {
    }

    TestCaseSimple::~TestCaseSimple()
    {
        cout <<"TestCaseSimple::~TestCaseSimple"<<endl;
    }

    bool TestCaseSimple::initialize()
    {
        char message[ 50 ] = "Initialize TestCaseSimple...\n";
        out->write( message, strlen( message ) + 1 );
        return true;
    }

    void TestCaseSimple::step()
    {
        rtos_printf(".");
        stepCounter++;
        // Test if the assert construction works.
        isSet = true;
        testAssert( isSet, errorAssert );

    }

    void TestCaseSimple::finalize()
    {
        testAssert( stepCounter == 13, errorStep );
        char message[ 50 ] = "Finalize TestCaseSimple...\n";
        out->write( message, strlen( message ) + 1 );
    }

}
