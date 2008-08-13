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

#include <cppunit/extensions/HelperMacros.h>

#include <scripting/Parser.hpp>
#include <ExecutionEngine.hpp>
#include <TaskContext.hpp>
#include <string>

using namespace RTT;

class TypesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TypesTest );
    //CPPUNIT_TEST( testTypes );
    CPPUNIT_TEST( testOperators );
//    CPPUNIT_TEST( testProperties );
//    CPPUNIT_TEST( testConversions );
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    TaskContext* tc;
    ActivityInterface* tsim;
    TaskObject* createMethodFactory();
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
    void executePrograms(const std::string& prog);
    void executeStates(const std::string& state);
public:

    void setUp();
    void tearDown();

    void testTypes();
    void testOperators();
    void testProperties();
    void testConversions();
};

#endif
