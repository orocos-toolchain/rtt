/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseTasks.cxx 

                        TestCaseTasks.cxx -  description
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
#include "TestCaseTasks.hpp"

namespace UnitTesting
{

    TestCaseTasks::TestCaseTasks( TestSuite* suite ) :
        TestCase( "TestCaseTasks", suite, 100 ), 
        nr_of_p(EventPeriodic::MAX_LISTENERS-2), nr_of_np(EventPeriodic::MAX_LISTENERS-2), runningP(1), runningNp(1), // see step() why it's 1 and not 0
        errInitNP("Not all TaskNonPreemptible::init() or finalize() functions were called\n"),
        errStepNP("Not all TaskNonPreemptible::step() functions were called\n"),
        errFinNP("Not all TaskNonPreemptible::finalize() or finalize() functions were called\n"),
        errInitP("Not all TaskPreemptible::init() functions were called\n"),
        errStepP("Not all TaskPreemptible::step() functions were called\n"),
        errFinP("Not all TaskPreemptible::finalize() functions were called\n")
    {
        np_tasks.reserve(nr_of_np); 
        p_tasks.reserve(nr_of_p);
        for (unsigned int i=0; i< nr_of_np/2; ++i) 
            np_tasks.push_back( new DummyNPTask(0.001) );
        for (unsigned int i=0; i< nr_of_np/2; ++i) 
            np_tasks.push_back( new DummyNPTask(0.01) );
        for (unsigned int i=0; i< nr_of_p/3; ++i) 
            p_tasks.push_back( new DummyPTask(0.005) );
        for (unsigned int i=0; i< nr_of_p/3; ++i) 
            p_tasks.push_back( new DummyPTask(0.01) );
        for (unsigned int i=0; i< nr_of_p/3; ++i) 
            p_tasks.push_back( new DummyPTask(0.1) );
        
        nr_of_p = p_tasks.size();
        nr_of_np = np_tasks.size();
    }

    TestCaseTasks::~TestCaseTasks()
    {
        for (NPI i=np_tasks.begin(); i != np_tasks.end(); ++i) 
            {
                delete *i;
            }
        for (PI i=p_tasks.begin(); i != p_tasks.end(); ++i) 
            {
                delete *i;
            }
    }

    bool TestCaseTasks::initialize()
    {
        //ZeroTimeThread::Instance()->stop();
        //ZeroLatencyThread::Instance()->stop();
        
        for (NPI i=np_tasks.begin(); i != np_tasks.end(); ++i) 
            {
                (*i)->start();
            }
        for (PI i=p_tasks.begin(); i != p_tasks.end(); ++i) 
            {
                (*i)->start();
            }
        
        //ZeroTimeThread::Instance()->start();
        //ZeroLatencyThread::Instance()->start();
        return true;
    }

    void TestCaseTasks::step()
    {
        np_tasks[runningNp]->stop();
        np_tasks[runningNp - 1]->start();

        p_tasks[runningP]->stop();
        p_tasks[runningP - 1]->start();

        if ( ++runningP == nr_of_p) runningP = 1;
        if ( ++runningNp == nr_of_np) runningNp = 1;
    }

    void TestCaseTasks::finalize()
    {
        int totInit=0, totSteps=0, totFins=0;
        for (unsigned int i=0; i < nr_of_np; ++i) np_tasks[i]->stop();
        for (unsigned int i=0; i < nr_of_p; ++i) p_tasks[i]->stop();

        // exclude first and last one from checking
        for (unsigned int i=1; i < nr_of_np-1; ++i) 
            {
                totInit += np_tasks[i]->nrOfInits();
                totSteps += np_tasks[i]->nrOfSteps();
                totFins += np_tasks[i]->nrOfFinals();
            }
        testAssert(totInit == totFins, errInitNP );
        testAssert(totSteps >= totInit , errStepNP );
        //testAssert(totFins == (nr_of_np-2) * 10 , errFinNP );

        totInit = totSteps = totFins = 0;

        for (unsigned int i=1; i < nr_of_p-1; ++i) 
            {
                totInit += p_tasks[i]->nrOfInits();
                totSteps += p_tasks[i]->nrOfSteps();
                totFins += p_tasks[i]->nrOfFinals();
            }
        testAssert(totInit == totFins , errInitP );
        testAssert(totSteps >= totInit ,errStepP);
        //testAssert(totFins == (nr_of_p-2) * 10 , errFinP);
    }

}
