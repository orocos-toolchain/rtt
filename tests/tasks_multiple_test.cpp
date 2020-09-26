/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:50 CET 2005  tasks_multiple_test.cpp

                        tasks_multiple_test.cpp -  description
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

#include "tests/tasks_multiple_test.hpp"
#include "rtt/os/threads.hpp"
#include "rtt/Logger.hpp"

#include <iostream>
using namespace std;
using namespace RTT;


    void ActivitiesMultipleTest::setUp()
    {
        nr_of_p = TimerThread::MAX_ACTIVITIES/2;
        nr_of_np = TimerThread::MAX_ACTIVITIES/2;
        runningP = 1;
        runningNp = 1; // see step() why it's 1 and not 0
        errInitNP = "Not all NonPreemptibleActivity::init() or finalize() functions were called.";
        errStepNP = "Not all NonPreemptibleActivity::step() functions were called.";
        errFinNP = "Not all NonPreemptibleActivity::finalize() or finalize() functions were called.";
        errInitP ="Not all PreemptibleActivity::init() functions were called.";
        errStepP ="Not all PreemptibleActivity::step() functions were called.";
        errFinP ="Not all PreemptibleActivity::finalize() functions were called.";

        log(Info) << "Creating "<< nr_of_np << " High priority and "<< nr_of_p << " Low priority activities."<<endlog();

        np_tasks.reserve(nr_of_np);
        p_tasks.reserve(nr_of_p);
        for (unsigned int i=0; i< nr_of_np/2; ++i)
            np_tasks.push_back( new DummyNPTask( os::HighestPriority, 0.004 ) );
        for (unsigned int i=0; i< nr_of_np/2; ++i)
            np_tasks.push_back( new DummyNPTask( os::HighestPriority, 0.004*2) );
        for (unsigned int i=0; i< nr_of_p/3; ++i)
            p_tasks.push_back( new DummyPTask( os::HighestPriority - os::IncreasePriority, 0.032 ) );
        for (unsigned int i=0; i< nr_of_p/3; ++i)
            p_tasks.push_back( new DummyPTask( os::HighestPriority - os::IncreasePriority, 0.032*2) );
        for (unsigned int i=0; i< nr_of_p/3; ++i)
            p_tasks.push_back( new DummyPTask( os::HighestPriority - os::IncreasePriority, 0.032*7) );

        log(Info) << "Done."<<endlog();

        nr_of_p = p_tasks.size();
        nr_of_np = np_tasks.size();
    }

void ActivitiesMultipleTest::tearDown()
    {
        // stop to be sure that task does not access memory during destruction.
        for (NPI i=np_tasks.begin(); i != np_tasks.end(); ++i)
            {
                (*i)->stop();
                delete *i;
            }
        for (PI i=p_tasks.begin(); i != p_tasks.end(); ++i)
            {
                (*i)->stop();
                delete *i;
            }
    }


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( ActivitiesMultipleTestSuite, ActivitiesMultipleTest )

    BOOST_AUTO_TEST_CASE( testMultiple )
    {
        for (NPI i=np_tasks.begin(); i != np_tasks.end(); ++i)
            {
                BOOST_CHECK( (*i)->start() );
            }
        for (PI i=p_tasks.begin(); i != p_tasks.end(); ++i)
            {
                BOOST_CHECK( (*i)->start() );
            }

        int runs = 0;

        // we lower the 'load' if the period is too short.
        // this is a bit arbitrary.
        int correction = 1;

        while ( runs++ != 100/correction ) {
            if ( np_tasks[runningNp]->isRunning() )
                BOOST_CHECK( np_tasks[runningNp]->stop() );
            if ( !np_tasks[runningNp - 1]->isRunning() )
                BOOST_CHECK( np_tasks[runningNp - 1]->start() );

            if ( p_tasks[runningP]->isRunning() )
                BOOST_CHECK( p_tasks[runningP]->stop() );
            if ( !p_tasks[runningP - 1]->isRunning() )
                BOOST_CHECK( p_tasks[runningP - 1]->start() );

            if ( ++runningP == nr_of_p) runningP = 1;
            if ( ++runningNp == nr_of_np) runningNp = 1;
            if ( runs % (20/correction) == 0 )
                sleep(1);
        }

        for (NPI i=np_tasks.begin(); i != np_tasks.end(); ++i)
            {
                if ( (*i)->isRunning() )
                    BOOST_CHECK( (*i)->stop() );
            }
        for (PI i=p_tasks.begin(); i != p_tasks.end(); ++i)
            {
                if ( (*i)->isRunning() )
                    BOOST_CHECK( (*i)->stop() );
            }

        for (unsigned int i=0; i < nr_of_np; ++i)
            {
                BOOST_CHECK_EQUAL( np_tasks[i]->nrOfOKStarts(), np_tasks[i]->nrOfInits() );
                //BOOST_CHECK_EQUAL( np_tasks[i]->nrOfOKStarts() , np_tasks[i]->nrOfStarts() );

                BOOST_CHECK_EQUAL( np_tasks[i]->nrOfOKStops(), np_tasks[i]->nrOfFinals() );
                //BOOST_CHECK_EQUAL( np_tasks[i]->nrOfOKStops() , np_tasks[i]->nrOfStops() );
            }
        for (unsigned int i=0; i < nr_of_p; ++i)
            {
                BOOST_CHECK_EQUAL( p_tasks[i]->nrOfOKStarts(), p_tasks[i]->nrOfInits() );
                //BOOST_CHECK_EQUAL( p_tasks[i]->nrOfOKStarts(), p_tasks[i]->nrOfStarts() );
                BOOST_CHECK_EQUAL( p_tasks[i]->nrOfOKStops(), p_tasks[i]->nrOfFinals() );
                //BOOST_CHECK_EQUAL( p_tasks[i]->nrOfOKStops(), p_tasks[i]->nrOfStops() );
            }

        int totInit=0, totSteps=0, totFins=0;

        // exclude first and last one from checking
        for (unsigned int i=1; i < nr_of_np-1; ++i)
            {
                totInit += np_tasks[i]->nrOfInits();
                totSteps += np_tasks[i]->nrOfSteps();
                totFins += np_tasks[i]->nrOfFinals();
            }
        BOOST_CHECK_MESSAGE( totInit == totFins, errInitNP);
        BOOST_CHECK_MESSAGE( totSteps >= totInit, errStepNP );
        //BOOST_CHECK_MESSAGE(errFinNP, totFins == (nr_of_np-2) * 10 );

        totInit = totSteps = totFins = 0;

        for (unsigned int i=1; i < nr_of_p-1; ++i)
            {
                totInit += p_tasks[i]->nrOfInits();
                totSteps += p_tasks[i]->nrOfSteps();
                totFins += p_tasks[i]->nrOfFinals();
            }
        BOOST_CHECK_MESSAGE(totInit == totFins, errInitP );
        BOOST_CHECK_MESSAGE(totSteps >= totInit, errStepP );
        //BOOST_CHECK_MESSAGE(errFinP, totFins == (nr_of_p-2) * 10 );
    }

BOOST_AUTO_TEST_SUITE_END()
