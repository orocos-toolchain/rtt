/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  buffers_test.cpp 

                        buffers_test.cpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#include "buffers_test.hpp"
#include <unistd.h>
#include <iostream>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( BuffersTest );

using namespace ORO_CoreLib;

struct Dummy {
    Dummy(double a = 0.0, double b =1.0, double c=2.0) 
        :d1(a), d2(b), d3(c) {}
    double d1;
    double d2;
    double d3;
    bool operator==(const Dummy& d) const
    {
        return d.d1 == d1 && d.d2 == d2 && d.d3 == d3;
    }

//     volatile Dummy& operator=(const Dummy& d) volatile
//     {
//         d1 = d.d1;
//         d2 = d.d2;
//         d3 = d.d3;
//         return *this;
//     }
        
};

    std::ostream& operator<<( std::ostream& os, const Dummy& d )  {
        os << "(" << d.d1 <<","<<d.d2<<","<<d.d3<<")";
        return os;
    }

#define QS 10

void 
BuffersTest::setUp()
{
    aqueue = new AtomicQueue<Dummy*>(QS);
    aqueue->clear();

    lockfree = new BufferLockFree<Dummy>(QS);

    dataobj  = new DataObjectLockFree<Dummy>("name");
}


void 
BuffersTest::tearDown()
{
    aqueue->clear();
    delete aqueue;

    delete lockfree;

    delete dataobj;
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

    delete d;
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

    delete d;
}

void BuffersTest::testBufLockFree()
{
    /**
     * Single Threaded test for BufferLockFree.
     * This is a mixed white/black box test.
     */

    Dummy* d = new Dummy;
    Dummy* c = new Dummy(2.0, 1.0, 0.0);
    Dummy r;

    CPPUNIT_ASSERT( lockfree->read(r) == false );

    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );

    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *c );

    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->write( *c ) == false );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *c );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *c );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );

    // start writing again half-way
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );

    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *c );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *c );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *c );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *c );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *c );
    CPPUNIT_ASSERT( lockfree->read(r) );
    CPPUNIT_ASSERT( r == *d );

    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );
    CPPUNIT_ASSERT( lockfree->read(r) == false );

    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( *c ) );

    std::vector<Dummy> v;
    CPPUNIT_ASSERT( 5 == lockfree->read(v) );
    CPPUNIT_ASSERT( v[0] == *c );
    CPPUNIT_ASSERT( v[1] == *d );
    CPPUNIT_ASSERT( v[2] == *c );
    CPPUNIT_ASSERT( v[3] == *d );
    CPPUNIT_ASSERT( v[4] == *c );

    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( v ) == v.size() );
    CPPUNIT_ASSERT( lockfree->write( *c ) );
    CPPUNIT_ASSERT( lockfree->write( *d ) );
    CPPUNIT_ASSERT( lockfree->write( v ) == 1 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( lockfree->write( v ) == 0 );
    CPPUNIT_ASSERT( 10 == lockfree->read(v) );
    CPPUNIT_ASSERT( v[0] == *c );
    CPPUNIT_ASSERT( v[1] == *d );
    CPPUNIT_ASSERT( v[2] == *c );
    CPPUNIT_ASSERT( v[3] == *d );
    CPPUNIT_ASSERT( v[4] == *c );
    CPPUNIT_ASSERT( v[5] == *d );
    CPPUNIT_ASSERT( v[6] == *c );
    CPPUNIT_ASSERT( v[7] == *c );
    CPPUNIT_ASSERT( v[8] == *d );
    CPPUNIT_ASSERT( v[9] == *c );
    CPPUNIT_ASSERT( 0 == lockfree->read(v) );
    delete d;
    delete c;
}

void BuffersTest::testDObjLockFree()
{
    Dummy* c = new Dummy(2.0, 1.0, 0.0);
    Dummy  d;
    dataobj->Set( *c );
    CPPUNIT_ASSERT_EQUAL( *c, dataobj->Get() );
    int i = 0;
    while ( i != 3.5*DataObjectLockFree<Dummy>::MAX_THREADS ) { 
        dataobj->Set( *c );
        dataobj->Set( d );
        ++i;
    }
    CPPUNIT_ASSERT_EQUAL( d , dataobj->Get() );
    CPPUNIT_ASSERT_EQUAL( d , dataobj->Get() );

    delete c;
}
