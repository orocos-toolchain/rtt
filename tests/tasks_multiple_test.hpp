 
#ifndef TASKS_MULTIPLE_TEST_HPP
#define TASKS_MULTIPLE_TEST_HPP

#include <cppunit/extensions/HelperMacros.h>

#include <corelib/TaskNonRealTime.hpp>
#include <corelib/TaskPreemptible.hpp>
#include <corelib/TaskNonPreemptible.hpp>

using namespace ORO_CoreLib;


class DummyNPTask : public TaskNonPreemptible
{
    unsigned int inits;
    unsigned int steps;
    unsigned int fins;
public:
    DummyNPTask(double period) : TaskNonPreemptible(period), inits(0), steps(0), fins(0) {}
    bool initialize() { ++inits; return true;}
    void step() { ++steps; }
    void finalize() { ++fins; }
    unsigned int nrOfInits() { return inits;}
    unsigned int nrOfSteps() { return steps;}
    unsigned int nrOfFinals() { return fins;}
};

class DummyPTask : public TaskPreemptible
{
    unsigned int inits;
    unsigned int steps;
    unsigned int fins;
public:
    DummyPTask(double period) : TaskPreemptible(period), inits(0), steps(0), fins(0) {}
    bool initialize() { ++inits; return true;}
    void step() { ++steps; }
    void finalize() { ++fins; }
    unsigned int nrOfInits() { return inits;}
    unsigned int nrOfSteps() { return steps;}
    unsigned int nrOfFinals() { return fins;}
};


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
