/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.hpp 

                        generictask_test.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef GENERIC_TASK_H
#define GENERIC_TASK_H

#include <cppunit/extensions/HelperMacros.h>

#include <execution/TaskContext.hpp>
#include <execution/Factories.hpp>
#include <string>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class Generic_TaskTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Generic_TaskTest );
    CPPUNIT_TEST( testCommand );
    CPPUNIT_TEST( testMethod );
    CPPUNIT_TEST( testCommandFromDS );
    CPPUNIT_TEST( testDSCommand );
    CPPUNIT_TEST( testMethodFromDS );
    CPPUNIT_TEST( testMethodsC );
    CPPUNIT_TEST( testDSMethod );
    CPPUNIT_TEST( testCommandsC );
    CPPUNIT_TEST( testProperties );
    CPPUNIT_TEST( testPorts );
    CPPUNIT_TEST( testAttributes );
    CPPUNIT_TEST_SUITE_END();

    TaskContext* tc;
    ActivityInterface* tsim;
    MethodFactoryInterface* createMethodFactory();
    DataSourceFactoryInterface* createDataSourceFactory();
    CommandFactoryInterface* createCommandFactory();

    double m0() { return -d0(); }
    double m1(int i) { return -d1(i); }
    double m2(int i, double d) { return -d2(i,d); }
    double m3(int i, double d, bool c) { return -d3(i,d,c); }
    double m4(int i, double d, bool c, std::string s) { return -d4(i,d,c,s); }

    double d0() const { return 1.0; }
    double d1(int i) const { return 2.0; }
    double d2(int i, double d) const { return 3.0; }
    double d3(int i, double d, bool c) const { return 4.0; }
    double d4(int i, double d, bool c, std::string s) const { return 5.0; }

    bool cd0()  { return true; }
    bool cd1(int i)  { return true; }
    bool cd2(int i, double d)  { return true; }
    bool cd3(int i, double d, char c)  { return true; }
    bool cd4(int i, double d, char c, bool b)  { return true; }
    bool cn0() const  { return true; }
    bool cn1(int i) const  { CPPUNIT_ASSERT_EQUAL(1, i); return true; }
    bool cn2(int i, double d) const  { CPPUNIT_ASSERT_EQUAL(1, i); CPPUNIT_ASSERT_EQUAL(1.0,d); return true; }
    bool cn3(int i, double d, char c) const  { CPPUNIT_ASSERT_EQUAL(1, i); CPPUNIT_ASSERT_EQUAL(1.0,d); CPPUNIT_ASSERT_EQUAL('a',c); return true; }
    bool cn4(int i, double d, char c, bool b) const  {  CPPUNIT_ASSERT_EQUAL(1, i); CPPUNIT_ASSERT_EQUAL(1.0,d); CPPUNIT_ASSERT_EQUAL('a',c); CPPUNIT_ASSERT_EQUAL(true,b);return true; }

    bool assertBool( bool );

    void verifydispatch(DispatchInterface& com);
    void verifycommand(CommandC& com);

public:

    void setUp();
    void tearDown();

    void testMethodsC();
    void testCommandsC();
    void testProperties();
    void testAttributes();
    void testCommand();
    void testCommandFromDS();
    void testDSCommand();
    void testMethod();
    void testMethodFromDS();
    void testDSMethod();

    void testPorts();
};

#endif
