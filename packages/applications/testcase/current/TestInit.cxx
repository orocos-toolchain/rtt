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

