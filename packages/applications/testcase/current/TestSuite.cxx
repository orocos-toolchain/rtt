/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestSuite.cxx 

                        TestSuite.cxx -  description
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
#include "TestSuite.hpp"

namespace UnitTesting
{

    TestSuite::TestSuite(WriteInterface* out)
    {
        tests.reserve( 32 );
        reset();
        createTests(out);
    }

    TestSuite::~TestSuite()
    {
        for ( TestList::iterator itl = tests.begin(); itl != tests.end(); ++itl)
            delete (*itl);
        tests.clear();
    }

    void TestSuite::addTest( TestCaseInterface* testCase )
    {
        tests.push_back( testCase );
    }

    TestCaseInterface* TestSuite::getNextTest()
    {
        // return old, increment to reach new
        return * ( nextTest++ );
    }

    int TestSuite::testsNumber()
    {
        return tests.size();
    }

    bool TestSuite::atEnd()
    {
        return ( nextTest == tests.end() );
    }

    void TestSuite::reset()
    {
        nextTest = tests.begin();
    }

    int TestSuite::errorsNumber()
    {
        int n = 0;

        for ( TestList::iterator i = tests.begin(); i != tests.end(); ++i )
        {
            n += ( *i ) ->errorsNumber();
        }

        return n;

    }

    int TestSuite::testsFailed()
    {
        int n = 0;
        reset();

        while ( !atEnd() )
        {
            n += ( getNextTest() ->errorsNumber() != 0 ? 1 : 0 );
        }

        return n;

    }

}

