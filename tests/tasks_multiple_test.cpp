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
 
 

#include "tasks_multiple_test.hpp"
#include <corelib/TaskTimerInterface.hpp>
#include <corelib/ZeroTimeThread.hpp>
#include <corelib/ZeroLatencyThread.hpp>
#include <corelib/NonRealTimeThread.hpp>

#include <iostream>
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TasksMultipleTest );

    void TasksMultipleTest::setUp()
    {
        nr_of_p = TaskTimerInterface::MAX_TASKS/2;
        nr_of_np = TaskTimerInterface::MAX_TASKS/2;
        runningP = 1; 
        runningNp = 1; // see step() why it's 1 and not 0
        errInitNP = "Not all TaskNonPreemptible::init() or finalize() functions were called\n";
        errStepNP = "Not all TaskNonPreemptible::step() functions were called\n";
        errFinNP = "Not all TaskNonPreemptible::finalize() or finalize() functions were called\n";
        errInitP ="Not all TaskPreemptible::init() functions were called\n";
        errStepP ="Not all TaskPreemptible::step() functions were called\n";
        errFinP ="Not all TaskPreemptible::finalize() functions were called\n";

        np_tasks.reserve(nr_of_np); 
        p_tasks.reserve(nr_of_p);
        for (unsigned int i=0; i< nr_of_np/2; ++i) 
            np_tasks.push_back( new DummyNPTask( ZeroTimeThread::Instance()->getPeriod() ) );
        for (unsigned int i=0; i< nr_of_np/2; ++i) 
            np_tasks.push_back( new DummyNPTask( ZeroTimeThread::Instance()->getPeriod()*8) );
        for (unsigned int i=0; i< nr_of_p/3; ++i) 
            p_tasks.push_back( new DummyPTask( ZeroLatencyThread::Instance()->getPeriod()) );
        for (unsigned int i=0; i< nr_of_p/3; ++i) 
            p_tasks.push_back( new DummyPTask( ZeroLatencyThread::Instance()->getPeriod()*2) );
        for (unsigned int i=0; i< nr_of_p/3; ++i) 
            p_tasks.push_back( new DummyPTask( ZeroLatencyThread::Instance()->getPeriod()*7) );
        
        nr_of_p = p_tasks.size();
        nr_of_np = np_tasks.size();
        for (NPI i=np_tasks.begin(); i != np_tasks.end(); ++i) 
            {
                (*i)->start();
            }
        for (PI i=p_tasks.begin(); i != p_tasks.end(); ++i) 
            {
                (*i)->start();
            }
    }

    void TasksMultipleTest::testMultiple()
    {
        int runs = 0;

        while ( runs++ != 100 ) {
            if ( np_tasks[runningNp]->isRunning() )
                np_tasks[runningNp]->stop();
            if ( !np_tasks[runningNp - 1]->isRunning() )
                np_tasks[runningNp - 1]->start();

            if ( p_tasks[runningP]->isRunning() )
                p_tasks[runningP]->stop();
            if ( !p_tasks[runningP - 1]->isRunning() )
                p_tasks[runningP - 1]->start();

            if ( ++runningP == nr_of_p) runningP = 1;
            if ( ++runningNp == nr_of_np) runningNp = 1;
            if ( runs % 20 == 0 )
                sleep(1);
        }

        for (NPI i=np_tasks.begin(); i != np_tasks.end(); ++i) 
            {
                if ( (*i)->isRunning() )
                    (*i)->stop();
            }
        for (PI i=p_tasks.begin(); i != p_tasks.end(); ++i) 
            {
                if ( (*i)->isRunning() )
                    (*i)->stop();
            }

        for (unsigned int i=0; i < nr_of_np; ++i) 
            {
                CPPUNIT_ASSERT_EQUAL( np_tasks[i]->nrOfOKStarts(), np_tasks[i]->nrOfInits() );
                //CPPUNIT_ASSERT_EQUAL( np_tasks[i]->nrOfOKStarts() , np_tasks[i]->nrOfStarts() );   

                CPPUNIT_ASSERT_EQUAL( np_tasks[i]->nrOfOKStops(), np_tasks[i]->nrOfFinals() );
                //CPPUNIT_ASSERT_EQUAL( np_tasks[i]->nrOfOKStops() , np_tasks[i]->nrOfStops() );   
            }
        for (unsigned int i=0; i < nr_of_p; ++i) 
            {
                CPPUNIT_ASSERT_EQUAL( p_tasks[i]->nrOfOKStarts(), p_tasks[i]->nrOfInits() );
                //CPPUNIT_ASSERT_EQUAL( p_tasks[i]->nrOfOKStarts(), p_tasks[i]->nrOfStarts() );
                CPPUNIT_ASSERT_EQUAL( p_tasks[i]->nrOfOKStops(), p_tasks[i]->nrOfFinals() );
                //CPPUNIT_ASSERT_EQUAL( p_tasks[i]->nrOfOKStops(), p_tasks[i]->nrOfStops() );
            }

        int totInit=0, totSteps=0, totFins=0;

        // exclude first and last one from checking
        for (unsigned int i=1; i < nr_of_np-1; ++i) 
            {
                totInit += np_tasks[i]->nrOfInits();
                totSteps += np_tasks[i]->nrOfSteps();
                totFins += np_tasks[i]->nrOfFinals();
            }
        CPPUNIT_ASSERT_MESSAGE( errInitNP, totInit == totFins);
        CPPUNIT_ASSERT_MESSAGE( errStepNP, totSteps >= totInit );
        //CPPUNIT_ASSERT_MESSAGE(errFinNP, totFins == (nr_of_np-2) * 10 );

        totInit = totSteps = totFins = 0;

        for (unsigned int i=1; i < nr_of_p-1; ++i) 
            {
                totInit += p_tasks[i]->nrOfInits();
                totSteps += p_tasks[i]->nrOfSteps();
                totFins += p_tasks[i]->nrOfFinals();
            }
        CPPUNIT_ASSERT_MESSAGE(errInitP,totInit == totFins );
        CPPUNIT_ASSERT_MESSAGE(errStepP,totSteps >= totInit );
        //CPPUNIT_ASSERT_MESSAGE(errFinP, totFins == (nr_of_p-2) * 10 );
    }

    void TasksMultipleTest::tearDown()
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


