/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCase.cxx 

                        TestCase.cxx -  description
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
 #include "TestCase.hpp"

namespace UnitTesting
{

    TestCase::TestCase( const std::string& name, TestSuite* suite, int nSteps ) 
        : caseName( name ), numberSteps( nSteps ), task(0, 10*1000*1000, this )
    {
        suite->addTest( this );
    }

    TestCase::~TestCase()
    {
        errors.clear();
    }

    const char* TestCase::caseNameGet()
    {
        return caseName.c_str();
    }

    bool TestCase::initialize()
    { return true; }

    void TestCase::step()
    {}

    void TestCase::finalize()
    {}

    void TestCase::error( const std::string& description )
    {
        errors.push_back( &description );
    }

    void TestCase::testAssert( bool condition, const std::string& errorStr )
    {
        if ( !condition )
            error( errorStr );
    }

    void TestCase::testAssert( ConditionInterface* condition, const std::string& errorStr )
    {
        testAssert( condition->evaluate(), errorStr );
    }

    /*
    void TestCase::testAssert( ConditionVariableInterface* cv, const std::string& errorStr )
    {
        testAssert( cv->conditionGet(), errorStr );
    }
    */

    std::vector<const std::string*>* TestCase::errorsGet()
    {
        return & errors;
    }

    int TestCase::errorsNumber()
    {
        return errors.size();
    }

    void TestCase::start( EventOperationInterface* ei )
    {
        caseEvent = ei;
        // Start the thread.
        task.start();
    }

    EventOperationInterface* TestCase::eventGet()
    {
        return caseEvent;
    }

    int TestCase::numberOfSteps()
    {
        return numberSteps;
    }

    /**
     * Start the thread.
     */
    bool TestCase::Task::start()
    {
        // Initialize <stepsToGo>
        stepsToGo = testCase->numberOfSteps();
        if ( !testCase->initialize() )
            return false;
        if ( !TaskPreemptible::start() )
        {
            stop(); return false;
        } 
        return true;
    }

    /**
     * Execute one step of the thread.
     */
    void TestCase::Task::step()
    {
        if ( stepsToGo == 0 )
        {
            if ( !stopped )
            {
                stop();
                stopped = true;
            }
        }

        else
        {
            stepsToGo--;
            testCase->step();
        }
    }

    /**
     * Stop the thread.
     */
    bool TestCase::Task::stop()
    {
        TaskPreemptible::stop();
        testCase->finalize();
        // Fire the event, so the runner can start the next test.
        testCase->eventGet()->fire();
        return true;
    }

    TestCase::Task::Task(secs s, nsecs ns, TestCase* t ) : TaskPreemptible(s,ns), testCase( t ), stopped( false )
    {
        //        taskNameSet( testCase->caseNameGet() );
    }

    TestCase::Task::~Task()
    {
    }

}
