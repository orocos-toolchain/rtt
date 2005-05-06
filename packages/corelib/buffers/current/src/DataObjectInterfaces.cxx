#include "corelib/DataObjectInterfaces.hpp"

// This file is purely for testing the instantiations.

using namespace ORO_CoreLib;

namespace {
    struct Bar {
        double a,b,c;
    };
    double foo() {
        DataObject<double> b("b");
        DataObject<Bar> c("c");
        return b.get();
    }
}
