/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  types_test.hpp

                        types_test.hpp -  description
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


#ifndef TYPESTEST_H
#define TYPESTEST_H

#include <scripting/Parser.hpp>
#include <scripting/ScriptingService.hpp>
#include <ExecutionEngine.hpp>
#include <TaskContext.hpp>
#include <string>
#include <iostream>

using namespace RTT;
using namespace RTT::detail;

class TypesTest
{
public:
    Parser parser;
    TaskContext* tc;
    ScriptingService* sa;
    Service::shared_ptr createOperationCallerFactory();
    bool assertBool( bool );
    bool assertEqual( double, double );
    bool assertMsg( bool, const std::string& msg);
    void print(int i) { std::cout<<"print:"<<i<<std::endl;}
    void printb(bool i) { std::cout<<"printb:"<<i<<std::endl;}
    int pass(int i) { return i; }
    void executePrograms(const std::string& prog);
    void executeStates(const std::string& state);
public:

	TypesTest(){ setUp();};
	~TypesTest(){ tearDown(); };
    void setUp();
    void tearDown();

    void testTypes();
    void testOperators();
    void testProperties();
    void testConversions();
};

#endif
