/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:41 CET 2006  template_factory_test.hpp

                        template_factory_test.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef TEMPLATE_FACTORYTEST_H
#define TEMPLATE_FACTORYTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <MultiVector.hpp>
#include <scripting/Parser.hpp>
#include <CommandProcessor.hpp>
#include <TaskContext.hpp>
#include <SlaveActivity.hpp>
#include <string>

using namespace RTT;
using namespace RTT;

class Template_FactoryTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Template_FactoryTest );
    CPPUNIT_TEST( testMethods );
    CPPUNIT_TEST( testUserMethods );
    CPPUNIT_TEST( testCommands );
    CPPUNIT_TEST( testManual );
    CPPUNIT_TEST( testEventC );
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    TaskContext* tc;
    ActivityInterface* tsim;
    SlaveActivity act;
    CommandProcessor processor;
    TaskObject* createMethodFactory();
    TaskObject* createUserMethodFactory();
    TaskObject* createCommandFactory();

    RTT::Event<int( float, float )> t_event_float;
    int float_listener(float a, float b);
    int float_completer(float a, float b);
    RTT::EventProcessor* event_proc;
    float float_sum, float_sub;

    void executePrograms(const Parser::ParsedPrograms& pg_list );
    double m0() { return -d0(); }
    double m1(int i) { return -d1(i); }
    double m2(int i, double d) { return -d2(i,d); }
    double m3(int i, double d, bool c) { return -d3(i,d,c); }
    double m4(int i, double d, bool c, std::string s) { return -d4(i,d,c,s); }

    Double6D umd(Double6D d) { return d; }
    const std::vector<double>& umcrv(const std::vector<double>& d) { return d; }
    std::vector<double> umv( std::vector<double> d) { return d; }
    const std::vector<double> umcv(const std::vector<double> d) { return d; }
    std::vector<double>& umrv( std::vector<double>& d) { return d; }

    double d0() const { return 1.0; }
    double d1(int i) const { return 2.0; }
    double d2(int i, double d) const { return 3.0; }
    double d3(int i, double d, bool c) const { return 4.0; }
    double d4(int i, double d, bool c, std::string s) const { return 5.0; }

    bool cd0()  { return true; }
    bool cd1(int i)  { return true; }
    bool cd2(int i, double d)  { return true; }
    bool cd3(int i, double d, char c)  { return true; }
    bool cn0() const  { return true; }
    bool cn1(int i) const  { return true; }
    bool cn2(int i, double d) const  { return true; }
    bool cn3(int i, double d, char c) const  { return true; }

    bool assertBool( bool );
public:

    void setUp();
    void tearDown();

    void testMethods();
    void testUserMethods();
    void testData();
    void testCommands();
    void testEventC();

    void testManual();
};

#endif
