#ifndef SPECIALIZED_ACTIVITIES_H
#define SPECIALIZED_ACTIVITIES_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

class SpecializedActivities : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( SpecializedActivities );
    CPPUNIT_TEST( testFileDescriptorActivity );
    CPPUNIT_TEST_SUITE_END();

public:

    void testFileDescriptorActivity();
};

#endif

