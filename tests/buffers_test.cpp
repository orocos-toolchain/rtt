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
#include <boost/scoped_ptr.hpp>

using namespace std;

#include <boost/test/unit_test.hpp>

using namespace RTT;


std::ostream& operator<<( std::ostream& os, const Dummy& d )  {
	os << "(" << d.d1 <<","<<d.d2<<","<<d.d3<<")";
	return os;
}

#define QS 10

void
BuffersTest::setUp()
{
    aqueue = new AtomicQueue<Dummy*>(QS);

    lockfree = new BufferLockFree<Dummy>(QS);

    dataobj  = new DataObjectLockFree<Dummy>("name");

    //mslist =  new SortedList<Dummy>();

    listlockfree = new ListLockFree<Dummy>(10, 4);

    mpool = new MemoryPool<Dummy>(QS);
    vpool = new MemoryPool<std::vector<Dummy> >(QS, std::vector<Dummy>(QS) );

    fmpool = new FixedSizeMemoryPool<Dummy>(QS);
    fvpool = new FixedSizeMemoryPool<std::vector<Dummy> >(QS, std::vector<Dummy>(QS));
}


void
BuffersTest::tearDown()
{
    aqueue->clear();
    delete aqueue;

    delete lockfree;

    delete dataobj;

    //delete mslist;

    delete listlockfree;

    delete mpool;
    delete vpool;
    delete fmpool;
    delete fvpool;
}


void addOne(Dummy& d)
{
    ++d.d1;
    ++d.d2;
    ++d.d3;
}

void subOne(Dummy& d)
{
    --d.d1;
    --d.d2;
    --d.d3;
}


struct LLFWorker : public RTT::OS::RunnableInterface
{
    bool stop;
    typedef ListLockFree<Dummy> T;
    T* mlst;
    int i;
    int appends;
    int erases;
    LLFWorker(T* l ) : stop(false), mlst(l), i(1) {}
    bool initialize() {
        stop = false; i = 1;
        appends = 0; erases = 0;
        return true;
    }
    void step() {
        while (stop == false ) {
            //cout << "Appending, i="<<i<<endl;
            while ( mlst->append( Dummy(i,i,i) ) ) { ++i; ++appends; }
            //cout << "Erasing, i="<<i<<endl;
            while ( mlst->erase( Dummy(i-1,i-1,i-1) ) ) { --i; ++erases; }
        }
        //cout << "Stopping, i="<<i<<endl;
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

struct LLFGrower : public RTT::OS::RunnableInterface
{
    bool stop;
    typedef ListLockFree<Dummy> T;
    T* mlst;
    int i;
    LLFGrower(T* l ) : stop(false), mlst(l), i(1) {}
    bool initialize() {
        stop = false; i = 1;
        return true;
    }
    void step() {
        // stress growing of list during append/erase.
        while (stop == false && i < 2500 ) {
            // reserve is quite slow.
            mlst->reserve(i);
            ++i;
        }
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

struct AQWorker : public RTT::OS::RunnableInterface
{
    bool stop;
    typedef QueueType T;
    T* mlst;
    int appends;
    int erases;
    AQWorker(T* l ) : stop(false), mlst(l),appends(0), erases(0) {}
    bool initialize() {
        stop = false;
        appends = 0; erases = 0;
        return true;
    }
    void step() {
        Dummy* orig = new Dummy( 1,2,3);
        Dummy* d = orig;
        while (stop == false ) {
            //cout << "Appending, i="<<i<<endl;
            if ( mlst->enqueue( d ) ) { ++appends; }
            //cout << "Erasing, i="<<i<<endl;
            if ( mlst->dequeue( d ) ) { ++erases; }
        }
        delete orig;
        //cout << "Stopping, i="<<i<<endl;
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

struct AQGrower : public RTT::OS::RunnableInterface
{
    bool stop;
    typedef QueueType T;
    T* mlst;
    int i;
    AQGrower(T* l ) : stop(false), mlst(l), i(0) {}
    bool initialize() {
        stop = false; i = 0;
        return true;
    }
    void step() {
        // stress full queue
        Dummy* orig = new Dummy( 1,2,3);
        Dummy* d = orig;
        while (stop == false ) {
            if ( mlst->enqueue(d) )
                ++i;
        }
        delete orig;
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

BOOST_FIXTURE_TEST_SUITE( BuffersTestSuite, BuffersTest )

BOOST_AUTO_TEST_CASE( testAtomic )
{
    /**
     * Single Threaded test for AtomicQueue.
     */
    Dummy* d = new Dummy();
    Dummy* c = d;

    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->capacity() );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(0), aqueue->size() );
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_CHECK( aqueue->isEmpty() == true );
    BOOST_CHECK( aqueue->dequeue(c) == false );
    BOOST_CHECK( c == d );

    for ( int i = 0; i < QS; ++i) {
        BOOST_CHECK( aqueue->enqueue( d ) == true);
        BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(i+1), aqueue->size() );
    }
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->capacity() );
    BOOST_CHECK( aqueue->isFull() == true );
    BOOST_CHECK( aqueue->isEmpty() == false );
    BOOST_CHECK( aqueue->enqueue( d ) == false );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->size() );

    aqueue->dequeue( d );
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS-1), aqueue->size() );

    for ( int i = 0; i < QS - 1 ; ++i) {
        BOOST_CHECK( aqueue->dequeue( d ) == true);
        BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS - 2 - i), aqueue->size() );
    }
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_CHECK( aqueue->isEmpty() == true );

    delete d;
}

BOOST_AUTO_TEST_CASE( testAtomicCounted )
{
    /**
     * Single Threaded test for AtomicQueue.
     */

    Dummy* d = new Dummy;
    Dummy* e = new Dummy;
    Dummy* c = d;

    BOOST_CHECK( aqueue->dequeueCounted(c) == 0 );
    BOOST_CHECK( c == d );

    // monotonic increasing counts
    int cache = 1;
    for ( int i = 0; i < QS; ++i)
        BOOST_CHECK( aqueue->enqueueCounted( d ) == cache++ );
    BOOST_CHECK( aqueue->isFull() == true );
    BOOST_CHECK( aqueue->isEmpty() == false );
    BOOST_CHECK( aqueue->enqueueCounted( d ) == 0 );

    BOOST_CHECK( aqueue->dequeueCounted( d ) == 1 );
    BOOST_CHECK( aqueue->isFull() == false );

    cache = 2;
    for ( int i = 0; i < QS - 1 ; ++i)
        BOOST_CHECK( aqueue->dequeueCounted( d ) == cache++ );
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_CHECK( aqueue->isEmpty() == true );

    BOOST_CHECK( aqueue->enqueueCounted( d ) != 0 );
    BOOST_CHECK( aqueue->enqueueCounted( e ) != 0 );
    BOOST_CHECK( aqueue->enqueueCounted( d ) != 0 );
    c = 0;
    BOOST_CHECK( aqueue->dequeueCounted( c ) != 0 );
    BOOST_CHECK( c == d );
    BOOST_CHECK( aqueue->dequeueCounted( c ) != 0 );
    BOOST_CHECK( c == e );
    BOOST_CHECK( aqueue->dequeueCounted( c ) != 0 );
    BOOST_CHECK( c == d );

    delete e;
    delete d;
}

BOOST_AUTO_TEST_CASE( testBufLockFree )
{
    /**
     * Single Threaded test for BufferLockFree.
     * This is a mixed white/black box test.
     */

    Dummy* d = new Dummy;
    Dummy* c = new Dummy(2.0, 1.0, 0.0);
    Dummy r;

    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->front() == r ); // return default if empty.

    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );

    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *c );

    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->write( *c ) == false );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->front() == *c );

    // start writing again half-way
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->front() == *c );

    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) );
    BOOST_CHECK( r == *d );

    BOOST_CHECK( lockfree->front() == *d );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );
    BOOST_CHECK( lockfree->read(r) == false );

    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->front() == *c );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->write( *c ) );

    std::vector<Dummy> v;
    BOOST_CHECK( 5 == lockfree->read(v) );
    BOOST_CHECK( v[0] == *c );
    BOOST_CHECK( v[1] == *d );
    BOOST_CHECK( v[2] == *c );
    BOOST_CHECK( v[3] == *d );
    BOOST_CHECK( v[4] == *c );
    BOOST_CHECK( lockfree->front() == *d );

    BufferBase::size_type sz = 10;
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->write( v ) == (int)v.size() );
    BOOST_CHECK( lockfree->write( *c ) );
    BOOST_CHECK( lockfree->write( *d ) );
    BOOST_CHECK( lockfree->write( v ) == 1 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_CHECK( lockfree->write( v ) == 0 );
    BOOST_REQUIRE_EQUAL( sz, lockfree->read(v) );
    BOOST_CHECK( v[0] == *c );
    BOOST_CHECK( v[1] == *d );
    BOOST_CHECK( v[2] == *c );
    BOOST_CHECK( v[3] == *d );
    BOOST_CHECK( v[4] == *c );
    BOOST_CHECK( v[5] == *d );
    BOOST_CHECK( v[6] == *c );
    BOOST_CHECK( v[7] == *c );
    BOOST_CHECK( v[8] == *d );
    BOOST_CHECK( v[9] == *c );
    BOOST_CHECK( 0 == lockfree->read(v) );
    delete d;
    delete c;
}

BOOST_AUTO_TEST_CASE( testDObjLockFree )
{
    Dummy* c = new Dummy(2.0, 1.0, 0.0);
    Dummy  d;
    dataobj->Set( *c );
    BOOST_REQUIRE_EQUAL( *c, dataobj->Get() );
    int i = 0;
    while ( i != 3.5*DataObjectLockFree<Dummy>::MAX_THREADS ) {
        dataobj->Set( *c );
        dataobj->Set( d );
        ++i;
    }
    BOOST_REQUIRE_EQUAL( d , dataobj->Get() );
    BOOST_REQUIRE_EQUAL( d , dataobj->Get() );

    delete c;
}

BOOST_AUTO_TEST_CASE( testMemoryPool )
{
    // Test initial conditions.
    MemoryPool<Dummy>::size_type sz = QS;
    // for MemoryPool
    BOOST_REQUIRE_EQUAL( sz, mpool->size() );
    BOOST_REQUIRE_EQUAL( sz, vpool->size() );
    // for FixedSizeMemoryPool
    BOOST_REQUIRE_EQUAL( sz, fmpool->size() );
    BOOST_REQUIRE_EQUAL( sz, fvpool->size() );
    // Capacity test
    BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz, vpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz, fmpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz, fvpool->capacity() );

    // test default initialiser:
    for (MemoryPool<Dummy>::size_type i = 0; i <3*sz; ++i ) {
        // MemoryPool:
        std::vector<Dummy>* v = vpool->allocate();
        std::vector<Dummy>::size_type szv = QS;
        BOOST_REQUIRE_EQUAL( szv, v->size() );
        BOOST_REQUIRE_EQUAL( szv, v->capacity() );
        BOOST_CHECK(vpool->deallocate( v ));

        // FixedSizeMemoryPool:
        v = fvpool->allocate();
        BOOST_REQUIRE_EQUAL( szv, v->size() );
        BOOST_REQUIRE_EQUAL( szv, v->capacity() );
        BOOST_CHECK( fvpool->deallocate( v ) );
    }

    // test Allocation.
    std::vector<Dummy*> mpv;
    // MemoryPool:
    for (MemoryPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        mpv.push_back( mpool->allocate() );
        BOOST_REQUIRE_EQUAL( sz - i -1 , mpool->size());
        BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    }
    for (MemoryPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        BOOST_CHECK(mpool->deallocate( mpv.front() ));
        mpv.erase( mpv.begin() );
        BOOST_REQUIRE_EQUAL( i + 1 , mpool->size());
        BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    }
    BOOST_CHECK( mpv.size() == 0 );
    // FixedSizeMemoryPool:
    for (MemoryPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        mpv.push_back( fmpool->allocate() );
        BOOST_REQUIRE_EQUAL( sz - i -1 , fmpool->size());
        BOOST_REQUIRE_EQUAL( sz, fmpool->capacity() );
    }
    for (MemoryPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        BOOST_CHECK( fmpool->deallocate( mpv.front() ) );
        mpv.erase( mpv.begin() );
        BOOST_REQUIRE_EQUAL( i + 1 , fmpool->size());
        BOOST_REQUIRE_EQUAL( sz, fmpool->capacity() );
    }

    // test capacity increasing:
    for (MemoryPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        mpool->reserve();
        vpool->reserve();
    }
    BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz, mpool->size() );
    BOOST_REQUIRE_EQUAL( sz, vpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz, vpool->size() );

    // the following reserve() allocates new chunks.
    mpool->reserve();
    vpool->reserve();
    BOOST_REQUIRE_EQUAL( sz + 2*sz, mpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz + 2*sz, mpool->size() );
    BOOST_REQUIRE_EQUAL( sz + 2*sz, vpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz + 2*sz, vpool->size() );
    for (MemoryPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        mpool->reserve();
        vpool->reserve();
    }
    BOOST_REQUIRE_EQUAL( sz + 2*sz, mpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz + 2*sz, mpool->size() );
    BOOST_REQUIRE_EQUAL( sz + 2*sz, vpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz + 2*sz, vpool->size() );

    // Extra:
    // test default initialiser for extra reserved chunks:
    std::vector< std::vector<Dummy>* > vv;
    for (MemoryPool<Dummy>::size_type i = 0; i < sz+ 2*sz; ++i ) {
        // MemoryPool:
        std::vector<Dummy>* v = vpool->allocate();
        std::vector<Dummy>::size_type szv = QS;
        vv.push_back( v );
        BOOST_REQUIRE_EQUAL( szv, v->size() );
        BOOST_REQUIRE_EQUAL( szv, v->capacity() );
    }
    BOOST_REQUIRE_EQUAL( MemoryPool<Dummy>::size_type(0), vpool->size() );
    for (MemoryPool<Dummy>::size_type i = 0; i < sz+ 2*sz; ++i ) {
        // MemoryPool:
        BOOST_CHECK( vpool->deallocate( vv.back() ) );
        vv.erase( vv.end() - 1 );
    }

}

#if 0
BOOST_AUTO_TEST_CASE( testSortedList )
{
    // 7 elements.
    mslist->reserve(7);
    BOOST_CHECK( mslist->empty() );

    // empty list has no keys.
    BOOST_CHECK( mslist->hasKey(Dummy()) == false );

    // empty list fails to erase key.
    BOOST_CHECK( mslist->erase(Dummy()) == false );

    // insert element once
    BOOST_CHECK( mslist->insert(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == true );

    BOOST_CHECK( mslist->insert(Dummy(1,2,1)) == false );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == true );

    // erase element once
    BOOST_CHECK( mslist->erase(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == false );
    BOOST_CHECK( mslist->erase(Dummy(1,2,1)) == false );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == false );

    BOOST_CHECK( mslist->insert(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,2)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,3)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,4)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,5)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,6)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,7)) == true );

    BOOST_CHECK( mslist->hasKey(Dummy(1,2,4)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,7)) == true );

    BOOST_CHECK( mslist->erase(Dummy(1,2,7)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,7)) == false );

    BOOST_CHECK( mslist->erase(Dummy(1,2,4)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,4)) == false );

    mslist->applyOnData( &addOne );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,2)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,3)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,4)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,6)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,7)) == true );

    mslist->applyOnData( &subOne );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,2)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,3)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,5)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,6)) == true );

    BOOST_CHECK( mslist->erase(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->erase(Dummy(1,2,6)) == true );
    BOOST_CHECK( mslist->erase(Dummy(1,2,5)) == true );
    BOOST_CHECK( mslist->erase(Dummy(1,2,2)) == true );
    BOOST_CHECK( mslist->erase(Dummy(1,2,3)) == true );

    BOOST_CHECK( mslist->empty() );
}
#endif

BOOST_AUTO_TEST_CASE( testListLockFree )
{
    LLFWorker* aworker = new LLFWorker( listlockfree );
    LLFWorker* bworker = new LLFWorker( listlockfree );
    LLFWorker* cworker = new LLFWorker( listlockfree );
    LLFGrower* grower = new LLFGrower( listlockfree );

    {
        boost::scoped_ptr<SingleThread> athread( new SingleThread(20,"ThreadA", aworker ));
        boost::scoped_ptr<SingleThread> bthread( new SingleThread(20,"ThreadB", bworker ));
        boost::scoped_ptr<SingleThread> cthread( new SingleThread(20,"ThreadC", cworker ));
        boost::scoped_ptr<SingleThread> gthread( new SingleThread(20,"ThreadG", grower ));

        // avoid system lock-ups
        athread->setScheduler(ORO_SCHED_OTHER);
        bthread->setScheduler(ORO_SCHED_OTHER);
        cthread->setScheduler(ORO_SCHED_OTHER);
        gthread->setScheduler(ORO_SCHED_OTHER);

        athread->start();
        bthread->start();
        cthread->start();

        sleep(1);
        gthread->start();
        sleep(1);
        gthread->stop();
        sleep(1);

        athread->stop();
        bthread->stop();
        cthread->stop();
    }

#if 0
    cout << "Athread appends: " << aworker->appends<<endl;
    cout << "Athread erases: " << aworker->erases<<endl;
    cout << "Bthread appends: " << bworker->appends<<endl;
    cout << "Bthread erases: " << bworker->erases<<endl;
    cout << "Cthread appends: " << cworker->appends<<endl;
    cout << "Cthread erases: " << cworker->erases<<endl;
    cout << "List capacity: "<< listlockfree->capacity()<<endl;
    cout << "List size: "<< listlockfree->size()<<endl;
//     while( listlockfree->empty() == false ) {
//         Dummy d =  listlockfree->back();
//         //cout << "Left: "<< d <<endl;
//         BOOST_CHECK( listlockfree->erase( d ) );
//     }
#endif

    BOOST_CHECK( aworker->appends == aworker->erases );
    BOOST_CHECK( bworker->appends == bworker->erases );
    BOOST_CHECK( cworker->appends == cworker->erases );

    delete aworker;
    delete bworker;
    delete cworker;
    delete grower;
}

#ifdef OROPKG_OS_GNULINUX
BOOST_AUTO_TEST_CASE( testAtomicQueue )
{
    QueueType* qt = new QueueType(QS);
    AQWorker* aworker = new AQWorker( qt );
    AQWorker* bworker = new AQWorker( qt );
    AQWorker* cworker = new AQWorker( qt );
    AQGrower* grower = new AQGrower( qt );

    {
        boost::scoped_ptr<SingleThread> athread( new SingleThread(20,"ThreadA", aworker ));
        boost::scoped_ptr<SingleThread> bthread( new SingleThread(20,"ThreadB", bworker ));
        boost::scoped_ptr<SingleThread> cthread( new SingleThread(20,"ThreadC", cworker ));
        boost::scoped_ptr<SingleThread> gthread( new SingleThread(20,"ThreadG", grower ));

        // avoid system lock-ups
        athread->setScheduler(ORO_SCHED_OTHER);
        bthread->setScheduler(ORO_SCHED_OTHER);
        cthread->setScheduler(ORO_SCHED_OTHER);
        gthread->setScheduler(ORO_SCHED_OTHER);

        athread->start();
        bthread->start();
        cthread->start();
        gthread->start();
        sleep(10);
        gthread->stop();
        athread->stop();
        bthread->stop();
        cthread->stop();
    }

    //cout <<endl
    //     << "Total appends: " << aworker->appends + bworker->appends + cworker->appends+ grower->i<<endl;
    //cout << "Total erases : " << aworker->erases + bworker->erases+ cworker->erases + qt->size()<<endl;
    int i = 0; // left-over count
    Dummy* d = 0;
    while( qt->size() != 0 ) {
        BOOST_CHECK( qt->dequeue(d) == true);
        BOOST_CHECK( d );
        i++;
    }
    BOOST_CHECK( qt->dequeue(d) == false );
    //cout << "Left in Queue: "<< i <<endl;

    // assert: sum queues == sum dequeues
    BOOST_CHECK( aworker->appends + bworker->appends + cworker->appends + grower->i
                    == aworker->erases + bworker->erases + cworker->erases + i );

    delete aworker;
    delete bworker;
    delete cworker;
    delete grower;
}
#endif
BOOST_AUTO_TEST_SUITE_END()
