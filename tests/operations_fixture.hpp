
#include <iostream>
#include <TaskContext.hpp>
#include <interface/ServiceProvider.hpp>

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

/**
 * A test fixture that registers some operations to a tc
 * and provides also a caller tc.
 */
class OperationsFixture {
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
    double m0() { return -1.0; }
    double m1(int i) { if (i ==1) return -2.0; else return 2.0; }
    double m2(int i, double d) { if ( i == 1 && d == 2.0 ) return -3.0; else return 3.0; }
    double m3(int i, double d, bool c) { if ( i == 1 && d == 2.0 && c == true) return -4.0; else return 4.0; }
    double m4(int i, double d, bool c, std::string s) { if ( i == 1 && d == 2.0 && c == true && s == "hello") return -5.0; else return 5.0;  }

    bool assertBool(bool b) { return b; }

    ~OperationsFixture();

    ServiceProvider* createMethodFactory();
};
