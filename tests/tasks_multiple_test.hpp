/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  tasks_multiple_test.hpp 

                        tasks_multiple_test.hpp -  description
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
 
 
 
#ifndef TASKS_MULTIPLE_TEST_HPP
#define TASKS_MULTIPLE_TEST_HPP

#include <cppunit/extensions/HelperMacros.h>

#include <corelib/TaskNonRealTime.hpp>
#include <corelib/TaskPreemptible.hpp>
#include <corelib/TaskNonPreemptible.hpp>

using namespace ORO_CoreLib;


template<class T>
class DummyTask : public T
{
    unsigned int inits;
    unsigned int steps;
    unsigned int fins;

    unsigned int starts;
    unsigned int stops;
    unsigned int okstarts;
    unsigned int okstops;
public:
    DummyTask(double period)
        : T(period),
          inits(0), steps(0), fins(0),
          starts(0), stops(0),
          okstarts(0), okstops(0) {}
    
    bool start() {
        ++starts;
        if ( T::start() ) {
            ++okstarts;
            return true;
        }
        return false;
    }
    bool stop() {
        ++stops;
        if ( T::stop() ) {
            ++okstops;
            return true;
        }
        return false;
    }
    bool initialize() { ++inits; return true;}
    void step() { ++steps; }
    void finalize() { ++fins; }
    unsigned int nrOfStarts() { return starts;}
    unsigned int nrOfOKStarts() { return okstarts;}
    unsigned int nrOfInits() { return inits;}
    unsigned int nrOfSteps() { return steps;}
    unsigned int nrOfFinals() { return fins;}
    unsigned int nrOfStops() { return stops;}
    unsigned int nrOfOKStops() { return okstops;}
};

typedef DummyTask<TaskPreemptible> DummyPTask;
typedef DummyTask<TaskNonPreemptible> DummyNPTask;

/**
 * Test for starting and stopping tasks
 */
class TasksMultipleTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE( TasksMultipleTest );
    // Test Many Tasks :
    CPPUNIT_TEST( testMultiple );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();

    void testMultiple();

    void tearDown();

    std::vector<DummyNPTask*> np_tasks;

    typedef std::vector<DummyNPTask*>::iterator NPI;
    typedef std::vector<DummyPTask*>::iterator PI;

    std::vector<DummyPTask*> p_tasks;

    unsigned int nr_of_p;
    unsigned int nr_of_np;

    unsigned int runningP;
    unsigned int runningNp;

    std::string errInitNP;
    std::string errStepNP;
    std::string errFinNP;
    std::string errInitP;
    std::string errStepP;
    std::string errFinP;
};

#endif 
