/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestInit.cxx 

                        TestInit.cxx -  description
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
/**
* Code for initialisation and termination of the test module.
*/

#include "TestRunnerNonInteractive.hpp"
#include "TestSuite.hpp"
#include <corelib/WriteCout.hpp>
#include <corelib/Event.hpp>

#include <iostream>

using namespace ORO_CoreLib;

namespace UnitTesting
{

    TestSuite* suite;
    WriteCout* out;
#ifndef __KERNEL__ 
    // This event will wake up main() in userspace
    extern Event* quitEvent;
#endif
#ifndef __KERNEL__
    TestRunnerNonInteractive* runner;
#else
    TestRunnerNonInteractive* runner;
#endif

#ifdef __KERNEL__
    extern "C"
    {
#endif
        int testInit( int param )
        {
            std::cout <<"Creating Cout"<<std::endl;
            out = new WriteCout();
            std::cout <<"Creating Suite"<<std::endl;
            suite = new TestSuite(out);
#ifndef __KERNEL__
            runner = new TestRunnerNonInteractive( out, suite, quitEvent );
#else
            runner = new TestRunnerNonInteractive( out, suite );
#endif
            std::cout << "starting tests"<<std::endl;
            // Start the test runner.
            runner->startTests();
            return 0;
        }

        void testTerminate( void )
        {
            std::cout <<"Cleaning up Tests"<<std::endl;
            delete runner;
            delete suite;
            delete out;
        }

#ifdef __KERNEL__
    }

#endif

}

