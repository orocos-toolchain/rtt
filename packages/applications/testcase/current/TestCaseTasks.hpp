/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  TestCaseTasks.hpp 

                        TestCaseTasks.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
 
#ifndef TESTCASETASKS_HPP
#define TESTCASETASKS_HPP

#include <corelib/TaskPreemptible.hpp>
#include <corelib/TaskNonPreemptible.hpp>

#include "TestCase.hpp"

namespace UnitTesting
{
            
    using namespace ORO_CoreLib;

    class DummyNPTask : public TaskNonPreemptible
    {
        unsigned int inits;
        unsigned int steps;
        unsigned int fins;
    public:
        DummyNPTask(double period) : TaskNonPreemptible(period), inits(0), steps(0), fins(0) {}
        bool initialize() { ++inits; return true;}
         void step() { ++steps;}
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
    class TestCaseTasks :
                public TestCase
    {

        public:
            TestCaseTasks( TestSuite* suite );

            ~TestCaseTasks();

            virtual bool initialize();

            virtual  void step();

            virtual void finalize();

        private:
            // Forbidden!!!
            TestCaseTasks();

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

}

#endif 
