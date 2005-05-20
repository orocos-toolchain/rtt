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

#include <execution/Parser.hpp>
#include <execution/Processor.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Factories.hpp>
#include <string>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class TypesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TypesTest );
    CPPUNIT_TEST( testEmptyProgram );
    CPPUNIT_TEST( testReturnProgram );
    CPPUNIT_TEST( testTypes );
    CPPUNIT_TEST( testOperators );
    CPPUNIT_TEST( testProperties );
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    TaskContext* tc;
    Processor processor;
    MethodFactoryInterface* createMethodFactory();
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
public:

    void setUp();
    void tearDown();

    void testEmptyProgram();
    void testReturnProgram();
    void testTypes();
    void testOperators();
    void testProperties();
};

#endif
