/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  dispatch_test.hpp

                        dispatch_test.hpp -  description
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


#ifndef DISPATCHTEST_H
#define DISPATCHTEST_H

#include <scripting/Parser.hpp>
#include <CommandProcessor.hpp>
#include <TaskContext.hpp>
#include <string>
#include <SimulationActivity.hpp>

using namespace RTT;

class DispatchTest
{
public:
    Parser parser;
    TaskContext gtc;
    TaskContext mtc;
    TaskContext ltc;

    TaskObject* createObject(std::string a, CommandProcessor* cp);

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }

    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
    void doDispatch( const std::string& prog, TaskContext* );
    void finishDispatch( TaskContext* , std::string );
public:
    DispatchTest();
    ~DispatchTest(){ tearDown();};

    void setUp();
    void tearDown();

    void testParseDispatch();
    void testDispatchCondition();
    void testDispatchMany();
    void testDispatchFailure();
    void testDispatchAnd();
    void testDispatchTry();
    void testDispatchUntil();
    void testDispatchUntilFail();

};

#endif
