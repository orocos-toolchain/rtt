/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.hpp

                        generictask_test_2.hpp -  description
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


#ifndef GENERIC_TASK_2_H
#define GENERIC_TASK_2_H

#include <boost/test/unit_test.hpp>

#include <TaskContext.hpp>
#include <string>

using namespace RTT;
using namespace RTT;

class Generic_TaskTest_2
{
public:
    TaskContext* tc;
    ActivityInterface* tsim;
    TaskObject* createCommandFactory();

    // ref/const-ref tests:
    double ret;
    bool cn1r(double& a) { a = ret; return true; }
    bool cd1r(double& a) { a = ret * 2; return true; }
    bool cn1cr(const double& a) { ret = a; return true; }
    bool cd1cr(const double& a) { ret = a; return true; }

    // ref/const-ref of structs:
    struct CS
    {
        double x,y,z;
    };
    bool CScn1r(CS& a) {
        BOOST_CHECK(a.x == a.y + a.z );
        a.x = 2* a.y + 2* a.z;
        return true;
    }
    bool CScd1r(CS& a) { BOOST_CHECK(a.x == 2*a.y + 2*a.z ); return true; }
    bool CScn1cr(const CS& a) { BOOST_CHECK(a.x == a.y + a.z ); return true; }
    bool CScd1cr(const CS& a) { BOOST_CHECK(a.x == a.y + a.z ); return true; }

    // test const std::string& argument for command_ds
    bool comstr(const std::string& cs) { return !cs.empty(); }

    int cd0count,cd1count,cd2count,cd3count,cd4count;
    bool cd0()  { cd0count++; return true; }
    bool cd1(int i)  { cd1count++; return true; }
    bool cd2(int i, double d)  { cd2count++; return true; }
    bool cd3(int i, double d, char c)  { cd3count++; return true; }
    bool cd4(int i, double d, char c, bool b)  { cd4count++; return true; }
    bool cn0() const  { return true; }
    bool cn1(int i) const  { BOOST_CHECK_EQUAL(1, i); return true; }
    bool cn2(int i, double d) const  { BOOST_CHECK_EQUAL(1, i); BOOST_CHECK_EQUAL(1.0,d); return true; }
    bool cn3(int i, double d, char c) const  { BOOST_CHECK_EQUAL(1, i); BOOST_CHECK_EQUAL(1.0,d); BOOST_CHECK_EQUAL('a',c); return true; }
    bool cn4(int i, double d, char c, bool b) const  {  BOOST_CHECK_EQUAL(1, i); BOOST_CHECK_EQUAL(1.0,d); BOOST_CHECK_EQUAL('a',c); BOOST_CHECK_EQUAL(true,b);return true; }

    bool assertBool( bool );

    void verifydispatch(DispatchInterface& com);
    void verifycommand(CommandC& com);

public:
	Generic_TaskTest_2(){ setUp();};
	~Generic_TaskTest_2(){ tearDown();};
    void setUp();
    void tearDown();

};

#endif
