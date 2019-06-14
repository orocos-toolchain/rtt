/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  operations_fixture.hpp

                        operations_fixture.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include <iostream>
#include <TaskContext.hpp>
#include "unit.hpp"

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

/**
 * A test fixture that registers some operations to a tc
 * and provides also a caller tc.
 */
class RTT_UNIT_API OperationsFixture {
public:
    OperationsFixture();

    TaskContext* tc, *caller;

    // ref/const-ref tests:
    double ret;
    double& m0r() { return ret; }
    const double& m0cr() { return ret; }

    // test const std::string& argument for command_ds
    bool comstr(const std::string& cs) { return !cs.empty(); }

    double m1r(double& a) { a = 2*a; return a; }
    double m1cr(const double& a) { return a; }

    // plain argument tests:
    void   vm0(void) { return; }
    double m0(void) { return -1.0; }
    double m1(int i) { if (i ==1) return -2.0; else return 2.0; }
    double m2(int i, double d) { if ( i == 1 && d == 2.0 ) return -3.0; else return 3.0; }
    double m3(int i, double d, bool c) { if ( i == 1 && d == 2.0 && c == true) return -4.0; else return 4.0; }
    double m4(int i, double d, bool c, std::string s) { if ( i == 1 && d == 2.0 && c == true && s == "hello") return -5.0; else return 5.0;  }
    double m5(int i, double d, bool c, std::string s, float f) { if ( i == 1 && d == 2.0 && c == true && s == "hello" && f == 5.0f) return -6.0; else return 6.0;  }
    double m6(int i, double d, bool c, std::string s, float f, char h) { if ( i == 1 && d == 2.0 && c == true && s == "hello" && f == 5.0f && h == 'a') return -7.0; else return 7.0;  }
    double m7(int i, double d, bool c, std::string s, float f, char h, unsigned int st) { if ( i == 1 && d == 2.0 && c == true && s == "hello" && f == 5.0f && h == 'a' && st == 7) return -8.0; else return 8.0;  }

    void *returnAddressOf(int &i) { return &i; }
    const void *returnAddressOfConst(const int &i) { return &i; }

    int sleepAndIncrement(int seconds) { sleep(seconds); return ++i; }

    //exception tests:
    void m0except(void) { throw std::runtime_error("exception"); }

    void print(const std::string& what) { cout << "print: " << what <<endl; }
    void printNumber(const std::string& what, int n) { cout << "print: " << what << n << endl; }

    bool fail() {
        throw std::runtime_error("OperationsFixture::fail() called.");
        return true; // should be ignored anyway.
    }

    bool good() {
        return true;
    }

    bool assertBool(bool b) {
        if (!b)
            throw std::runtime_error("OperationsFixture::assertBool( b ) failed (arg was false).");
        return b;
    }

    bool isTrue(bool b) {
        return b;
    }

    bool assertEqual(int a, int b)
    {
        if (a != b) {
            cerr << "AssertEqual failed: a != b " << a << " != " << b << "." << endl;
            throw std::runtime_error("OperationsFixture::assertEqual( a, b ) failed (a != b).");
        }
        return a == b;
    }
    bool assertMsg( bool b, const std::string& msg) {
        if ( b == false ) {
            cout << "Asserted :" << msg << endl;
            throw std::runtime_error("OperationsFixture::assertMsg( b, msg ) faild (b was false).");
        }
        return true; // allow to continue to check other commands.
    }
    int increase() { return ++i;}

    void resetI() { i = 0; }
    int getI() const { return i; }
    int i;


    ~OperationsFixture();

    void createOperationCallerFactories(TaskContext* target);
    void createOperationCallerFactories0(TaskContext* target);
    void createOperationCallerFactories1(TaskContext* target);
    void createOperationCallerFactories2(TaskContext* target);
    void createOperationCallerFactories3(TaskContext* target);
    void createOperationCallerFactories4(TaskContext* target);
    void createOperationCallerFactories5(TaskContext* target);
    void createOperationCallerFactories6(TaskContext* target);
    void createOperationCallerFactories7(TaskContext* target);
};
