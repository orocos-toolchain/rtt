#ifndef BUFFERSTEST_H
#define BUFFERSTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <corelib/AtomicQueue.hpp>

using namespace ORO_CoreLib;

class Dummy;

class BuffersTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( BuffersTest );
    CPPUNIT_TEST( testAtomic );
    CPPUNIT_TEST( testAtomicCounted );
    CPPUNIT_TEST_SUITE_END();

    AtomicQueue<Dummy*>* aqueue;
public:

    void setUp();
    void tearDown();

    void testAtomic();
    void testAtomicCounted();
};

#endif
