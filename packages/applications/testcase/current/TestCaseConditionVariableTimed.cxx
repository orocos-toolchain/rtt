/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseConditionVariableTimed.cxx 

                        TestCaseConditionVariableTimed.cxx -  description
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
#include "corelib/TestCaseConditionVariableTimed.hpp"
#include <corelib/ConditionBool.hpp>

/*namespace UnitTesting
{

    TestCaseConditionVariableTimed::TestCaseConditionVariableTimed( TestSuite* suite ) :
        TestCase( "TestCaseConditionVariableTimed", suite, 1 ),
        cv1( &condition1, &lock , 0.1),cv2( &condition2, &lock , 0.1 ),
        errorValueNotTrue( "The value should be true but is not.\n" ),
        errorValueNotFalse( "The value should be false but is not.\n" )
    {
    }

    bool TestCaseConditionVariableTimed::initialize()
    {
    }

    void TestCaseConditionVariableTimed::step()
    {
        double timeout;
        bool value;

        // Make a ConditionVariableTimed object and test if it's initialized properly.
        value = ( cv1.conditionGet() ) ->evaluate();

        testAssert( !value, errorValueNotFalse );


        // Make a ConditionVariableTimed object and test if a value is set properly.
        {

            MutexLock locker( lock );
            condition2.setTrue();

            cv2.broadcast();
        }

        // The next evaluate() on the ConditionBool object should be true.
        testAssert( &cv2, errorValueNotTrue );
        return true; 
    }

    void TestCaseConditionVariableTimed::finalize()
    {
    }

    TestCaseConditionVariableTimed::~TestCaseConditionVariableTimed()
    {
    }

}*/

