
#include "buffers_test.hpp"
#include <unistd.h>
#include <iostream>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( BuffersTest );

using namespace ORO_CoreLib;

class Dummy {};

#define QS 10

void 
BuffersTest::setUp()
{
    aqueue = new AtomicQueue<Dummy*>(QS);
    aqueue->clear();
}


void 
BuffersTest::tearDown()
{
    aqueue->clear();
    delete aqueue;
}

void BuffersTest::testAtomic()
{
    /**
     * Single Threaded test for AtomicQueue.
     */

    Dummy* d = new Dummy;
    Dummy* c = d;

    CPPUNIT_ASSERT( aqueue->capacity() == QS );
    CPPUNIT_ASSERT( aqueue->isFull() == false );
    CPPUNIT_ASSERT( aqueue->isEmpty() == true );
    CPPUNIT_ASSERT( aqueue->dequeue(c) == false );
    CPPUNIT_ASSERT( c == d );

    for ( int i = 0; i < QS; ++i)
        CPPUNIT_ASSERT( aqueue->enqueue( d ) == true);
    CPPUNIT_ASSERT( aqueue->isFull() == true );
    CPPUNIT_ASSERT( aqueue->isEmpty() == false );
    CPPUNIT_ASSERT( aqueue->enqueue( d ) == false );

    aqueue->dequeue( d );
    CPPUNIT_ASSERT( aqueue->isFull() == false );

    for ( int i = 0; i < QS - 1 ; ++i)
        CPPUNIT_ASSERT( aqueue->dequeue( d ) == true);
    CPPUNIT_ASSERT( aqueue->isFull() == false );
    CPPUNIT_ASSERT( aqueue->isEmpty() == true );

}
void BuffersTest::testAtomicCounted()
{
    /**
     * Single Threaded test for AtomicQueue.
     */

    Dummy* d = new Dummy;
    Dummy* c = d;

    CPPUNIT_ASSERT( aqueue->dequeueCounted(c) == 0 );
    CPPUNIT_ASSERT( c == d );

    // monotonic increasing counts
    int cache = 1;
    for ( int i = 0; i < QS; ++i)
        CPPUNIT_ASSERT( aqueue->enqueueCounted( d ) == cache++ );
    CPPUNIT_ASSERT( aqueue->isFull() == true );
    CPPUNIT_ASSERT( aqueue->isEmpty() == false );
    CPPUNIT_ASSERT( aqueue->enqueueCounted( d ) == 0 );

    CPPUNIT_ASSERT( aqueue->dequeueCounted( d ) == 1 );
    CPPUNIT_ASSERT( aqueue->isFull() == false );

    cache = 2;
    for ( int i = 0; i < QS - 1 ; ++i)
        CPPUNIT_ASSERT( aqueue->dequeueCounted( d ) == cache++ );
    CPPUNIT_ASSERT( aqueue->isFull() == false );
    CPPUNIT_ASSERT( aqueue->isEmpty() == true );

}
