#ifndef PROPERTYTEST_H
#define PROPERTYTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
#include <geometry/MotionProperties.hpp>
#endif

using namespace ORO_CoreLib;

struct Composite {
    Composite() : a(0), b(-1), c(+1) {}
    void reset() { a = 0; b = -1; c = +1; }
    void change() { a = -1; b=+1; c=0; }
    bool isReset() { return a == 0 && b == -1 && c == +1;}
    bool isChanged() { return a == -1 && b == +1 && c == 0;}
    int a,b,c;
};

class PropertyTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( PropertyTest );
    CPPUNIT_TEST( testPrimitives );
    CPPUNIT_TEST( testBags );
    CPPUNIT_TEST( testBagOperations );
    CPPUNIT_TEST( testMarshalling );
    CPPUNIT_TEST_SUITE_END();

    PropertyBag bag1;
    PropertyBag bag2;
    PropertyBase* pb;
    Property<int>* pi1;
    Property<int>* pi2;
    Property<int>* pi1ref;
    Property<int>* pi2ref;
    int intref;
    Composite compref;
public:
    void setUp();
    void tearDown();

    void testPrimitives();
    void testBags();
    void testBagOperations();
    void testMarshalling();
};

#endif  // PROPERTYTEST_H
