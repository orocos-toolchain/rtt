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



#include <iostream>
#include <boost/scoped_ptr.hpp>

#include <internal/AtomicQueue.hpp>
#include <internal/AtomicMWSRQueue.hpp>

#include <Activity.hpp>

#include <boost/test/unit_test.hpp>

#include <RTT.hpp>
#include <base/Buffer.hpp>
#include <internal/ListLockFree.hpp>
#include <base/DataObject.hpp>
#include <internal/TsPool.hpp>
//#include <internal/SortedList.hpp>

#include <os/Thread.hpp>
#include <rtt-config.h>

using namespace std;
using namespace RTT;
using namespace RTT::detail;

class Dummy {
public:
    Dummy(double a = 0.0, double b =1.0, double c=2.0)
        :d1(a), d2(b), d3(c) {}
    double d1;
    double d2;
    double d3;
    bool operator==(const Dummy& d) const
    {
        return d.d1 == d1 && d.d2 == d2 && d.d3 == d3;
    }

    bool operator!=(const Dummy& d) const
    {
        return d.d1 != d1 || d.d2 != d2 || d.d3 != d3;
    }

    bool operator<(const Dummy& d) const
    {
        return d1+d2+d3 < d.d1 + d.d2 + d.d3;
    }
/*
     volatile Dummy& operator=(const Dummy& d) volatile
     {
         d1 = d.d1;
         d2 = d.d2;
         d3 = d.d3;
         return *this;
     }
*/
};


typedef AtomicQueue<Dummy*> QueueType;
typedef AtomicMWSRQueue<Dummy*> MWSRQueueType;

// Don't make queue size too large, we want to catch
// overrun issues too.
#define QS 10

class BuffersAQueueTest
{
public:
    AtomicQueue<Dummy*>* aqueue;
    ThreadInterface* athread;
    ThreadInterface* bthread;
    ListLockFree<Dummy>* listlockfree;

    BuffersAQueueTest()
    {
        aqueue = new AtomicQueue<Dummy*>(QS);
        listlockfree = new ListLockFree<Dummy>(10, 4);
    }
    ~BuffersAQueueTest(){
        aqueue->clear();
        delete aqueue;
        delete listlockfree;
    }
};

class BuffersAtomicMWSRQueueTest
{
public:
    AtomicMWSRQueue<Dummy*>* aqueue;

    BuffersAtomicMWSRQueueTest()
    {
        aqueue = new AtomicMWSRQueue<Dummy*>(QS);
    }
    ~BuffersAtomicMWSRQueueTest(){
        aqueue->clear();
        delete aqueue;
    }
};

class BuffersDataFlowTest
{
public:
    BufferLockFree<Dummy>* lockfree;
    DataObjectLockFree<Dummy>* dataobj;

    ThreadInterface* athread;
    ThreadInterface* bthread;

    BuffersDataFlowTest()
    {
        lockfree = new BufferLockFree<Dummy>(QS);

        dataobj  = new DataObjectLockFree<Dummy>();
    }

    ~BuffersDataFlowTest(){
        delete lockfree;
        delete dataobj;
    }
};

class BuffersMPoolTest
{
public:

    ThreadInterface* athread;
    ThreadInterface* bthread;

    TsPool<Dummy>* mpool;
    TsPool<std::vector<Dummy> >* vpool;

    BuffersMPoolTest()
    {
        mpool = new TsPool<Dummy>(QS);
        vpool = new TsPool<std::vector<Dummy> >(QS, std::vector<Dummy>(QS) );
    }

    ~BuffersMPoolTest(){
        delete mpool;
        delete vpool;
    }
};


std::ostream& operator<<( std::ostream& os, const Dummy& d )  {
	os << "(" << d.d1 <<","<<d.d2<<","<<d.d3<<")";
	return os;
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


struct LLFWorker : public RunnableInterface
{
    volatile bool stop;
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

struct LLFGrower : public RunnableInterface
{
    volatile bool stop;
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

/**
 * A Worker Reads and writes the queue.
 */
template<class T>
struct AQWorker : public RunnableInterface
{
    static os::Mutex m;
    bool stop;
    T* mlst;
    int appends;
    int erases;
    Dummy* orig;
    AQWorker(T* l ) : stop(false), mlst(l),appends(0), erases(0) {
        orig = new Dummy( 1,2,3);
    }
    ~AQWorker() {
        delete orig;
    }
    bool initialize() {
        stop = false;
        return true;
    }
    void step() {
        Dummy* d = orig;
        while (stop == false ) {
            //cout << "Appending, i="<<i<<endl;
            if ( mlst->enqueue( d ) ) { ++appends; }
            //cout << "Erasing, i="<<i<<endl;
            if ( mlst->dequeue( d ) ) {
                if( *d != *orig) {
                    os::MutexLock lock(m);
                    assert(*d == *orig); // exercise reading returned memory.
                }
                ++erases;
            }
        }
        //cout << "Stopping, i="<<i<<endl;
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

template<class T>
os::Mutex AQWorker<T>::m;

/**
 * A grower stresses the 'overrun' case by flooding
 * the queue.
 */
template<class T>
struct AQGrower : public RunnableInterface
{
    volatile bool stop;
    T* mlst;
    int appends;
    Dummy* orig;
    AQGrower(T* l ) : stop(false), mlst(l), appends(0) {
        orig = new Dummy( 1,2,3);
    }
    ~AQGrower() {
        delete orig;
    }
    bool initialize() {
        stop = false;
        return true;
    }
    void step() {
        // stress full queue
        Dummy* d = orig;
        while (stop == false ) {
            if ( mlst->enqueue(d) ) {
                ++appends;
            }
        }
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

/**
 * An Eater stresses the 'underrun' case by emptying
 * the queue.
 */
template<class T>
struct AQEater : public RunnableInterface
{
    volatile bool stop;
    T* mlst;
    int erases;
    AQEater(T* l ) : stop(false), mlst(l), erases(0) {}
    bool initialize() {
        stop = false;
        return true;
    }
    void step() {
        // stress full queue
        Dummy* d;
        while (stop == false ) {
            if ( mlst->dequeue(d) ) {
                //if( *d != *orig)
                //    BOOST_CHECK_EQUAL(*d, *orig); // exercise reading returned memory.
                ++erases;
            }
        }
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};


BOOST_FIXTURE_TEST_SUITE( BuffersAtomicTestSuite, BuffersAQueueTest )

BOOST_AUTO_TEST_CASE( testAtomicQueue )
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
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( BuffersMWSRQueueTestSuite, BuffersAtomicMWSRQueueTest )

BOOST_AUTO_TEST_CASE( testAtomicMWSRQueue )
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
        BOOST_CHECK( d );
    }
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->capacity() );
    BOOST_CHECK( aqueue->isFull() == true );
    BOOST_CHECK( aqueue->isEmpty() == false );
    BOOST_CHECK( aqueue->enqueue( d ) == false );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->size() );
    d = 0;
    aqueue->dequeue( d );
    BOOST_CHECK( d ); // not null
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS-1), aqueue->size() );

    for ( int i = 0; i < QS - 1 ; ++i) {
        BOOST_CHECK( aqueue->dequeue( d ) == true);
        BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS - 2 - i), aqueue->size() );
        BOOST_CHECK( d );
    }
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_CHECK( aqueue->isEmpty() == true );
    BOOST_CHECK( aqueue->dequeue(d) == false );
    BOOST_CHECK( d ); // not null

    delete d;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( BuffersDataFlowTestSuite, BuffersDataFlowTest )

BOOST_AUTO_TEST_CASE( testBufLockFree )
{
    /**
     * Single Threaded test for BufferLockFree.
     * This is a mixed white/black box test.
     */

    Dummy* d = new Dummy;
    Dummy* c = new Dummy(2.0, 1.0, 0.0);
    Dummy r;

    BOOST_CHECK( lockfree->Pop(r) == false );

    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );

    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *c );

    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Push( *c ) == false );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );

    // start writing again half-way
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );

    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( lockfree->Pop(r) );
    BOOST_CHECK( r == *d );

    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );
    BOOST_CHECK( lockfree->Pop(r) == false );

    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( *c ) );

    std::vector<Dummy> v;
    BOOST_CHECK( 5 == lockfree->Pop(v) );
    BOOST_CHECK( v[0] == *c );
    BOOST_CHECK( v[1] == *d );
    BOOST_CHECK( v[2] == *c );
    BOOST_CHECK( v[3] == *d );
    BOOST_CHECK( v[4] == *c );

    BufferBase::size_type sz = 10;
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( v ) == (int)v.size() );
    BOOST_CHECK( lockfree->Push( *c ) );
    BOOST_CHECK( lockfree->Push( *d ) );
    BOOST_CHECK( lockfree->Push( v ) == 1 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_CHECK( lockfree->Push( v ) == 0 );
    BOOST_REQUIRE_EQUAL( sz, lockfree->Pop(v) );
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
    BOOST_CHECK( 0 == lockfree->Pop(v) );
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
    while ( i != 3.5*dataobj->MAX_THREADS ) {
        dataobj->Set( *c );
        dataobj->Set( d );
        ++i;
    }
    BOOST_REQUIRE_EQUAL( d , dataobj->Get() );
    BOOST_REQUIRE_EQUAL( d , dataobj->Get() );

    delete c;
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE( BuffersMPoolTestSuite, BuffersMPoolTest )

BOOST_AUTO_TEST_CASE( testMemoryPool )
{
    // Test initial conditions.
    TsPool<Dummy>::size_type sz = QS;
    // Capacity test
    BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz, vpool->capacity() );
    BOOST_CHECK_EQUAL( sz, mpool->size());
    BOOST_CHECK_EQUAL( sz, vpool->size());

    // test default initialiser:
    for (TsPool<Dummy>::size_type i = 0; i <3*sz; ++i ) {
        // MemoryPool:
        std::vector<Dummy>* v = vpool->allocate();
        BOOST_CHECK_EQUAL( sz - 1, vpool->size());
        std::vector<Dummy>::size_type szv = QS;
        BOOST_REQUIRE_EQUAL( szv, v->size() );
        BOOST_REQUIRE_EQUAL( szv, v->capacity() );
        BOOST_CHECK(vpool->deallocate( v ));
        BOOST_CHECK_EQUAL( sz, vpool->size());
    }
    BOOST_CHECK_EQUAL( vpool->size(), QS);

    // test Allocation.
    std::vector<Dummy*> mpv;
    // MemoryPool:
    for (TsPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        mpv.push_back( mpool->allocate() );
        BOOST_CHECK_EQUAL( sz - i - 1, mpool->size());
        BOOST_CHECK( mpv.back() );
        BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    }
    BOOST_CHECK_EQUAL( mpool->size(), 0);
    BOOST_CHECK_EQUAL( mpool->allocate(), (Dummy*)0 );
    for (TsPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        BOOST_CHECK_EQUAL( i , mpool->size());
        BOOST_CHECK(mpool->deallocate( mpv.front() ));
        BOOST_CHECK_EQUAL( i + 1, mpool->size());
        mpv.erase( mpv.begin() );
        BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    }
    BOOST_CHECK_EQUAL( mpv.size(), 0 );
    BOOST_CHECK_EQUAL( mpool->size(), QS);
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

#ifdef OROPKG_OS_GNULINUX

BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE( BuffersStressLockFreeTestSuite, BuffersAQueueTest )

BOOST_AUTO_TEST_CASE( testListLockFree )
{
    LLFWorker* aworker = new LLFWorker( listlockfree );
    LLFWorker* bworker = new LLFWorker( listlockfree );
    LLFWorker* cworker = new LLFWorker( listlockfree );
    LLFGrower* grower = new LLFGrower( listlockfree );

    {
        boost::scoped_ptr<Activity> athread( new Activity(ORO_SCHED_OTHER, 0, 0, aworker, "ActivityA" ));
        boost::scoped_ptr<Activity> bthread( new Activity(ORO_SCHED_OTHER, 0, 0, bworker, "ActivityB" ));
        boost::scoped_ptr<Activity> cthread( new Activity(ORO_SCHED_OTHER, 0, 0, cworker, "ActivityC" ));
        boost::scoped_ptr<Activity> gthread( new Activity(ORO_SCHED_OTHER, 0, 0, grower, "ActivityG" ));

        athread->start();
        bthread->start();
        cthread->start();

        sleep(5);
        gthread->start();
        sleep(10);
        gthread->stop();
        sleep(5);

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
#endif

#ifdef OROPKG_OS_GNULINUX
BOOST_AUTO_TEST_CASE( testAtomicQueue )
{
    QueueType* qt = new QueueType(QS);
    AQWorker<QueueType>* aworker = new AQWorker<QueueType>( qt );
    AQWorker<QueueType>* bworker = new AQWorker<QueueType>( qt );
    AQWorker<QueueType>* cworker = new AQWorker<QueueType>( qt );
    AQGrower<QueueType>* grower = new AQGrower<QueueType>( qt );
    AQEater<QueueType>* eater = new AQEater<QueueType>( qt );

    {
        boost::scoped_ptr<Activity> athread( new Activity(20, aworker, "ActivityA" ));
        boost::scoped_ptr<Activity> bthread( new Activity(20, bworker, "ActivityB" ));
        boost::scoped_ptr<Activity> cthread( new Activity(20, cworker, "ActivityC" ));
        boost::scoped_ptr<Activity> gthread( new Activity(20, grower, "ActivityG"));
        boost::scoped_ptr<Activity> ethread( new Activity(20, eater, "ActivityE"));

        // avoid system lock-ups
        athread->thread()->setScheduler(ORO_SCHED_OTHER);
        bthread->thread()->setScheduler(ORO_SCHED_OTHER);
        cthread->thread()->setScheduler(ORO_SCHED_OTHER);
        gthread->thread()->setScheduler(ORO_SCHED_OTHER);

        log(Info) <<"Stressing multi-read/multi-write..." <<endlog();
        athread->start();
        bthread->start();
        cthread->start();
        sleep(5);
        log(Info) <<"Stressing multi-read/multi-write...on full buffer" <<endlog();
        gthread->start(); // stress full bufs
        sleep(5);
        gthread->stop();
        log(Info) <<"Stressing multi-read/multi-write...on empty buffer" <<endlog();
        ethread->start(); // stress empty bufs
        sleep(5);
        athread->stop();
        bthread->stop();
        cthread->stop();
        gthread->start(); // stress single-reader single-writer
        log(Info) <<"Stressing read&write..." <<endlog();
        sleep(5);
        gthread->stop();
        ethread->stop();
    }

    cout <<endl
         << "Total appends: " << aworker->appends + bworker->appends + cworker->appends+ grower->appends<<endl;
    cout << "Total erases : " << aworker->erases + bworker->erases+ cworker->erases + qt->size() + eater->erases <<endl;
    if (aworker->appends + bworker->appends + cworker->appends+ grower->appends != aworker->erases + bworker->erases+ cworker->erases + int(qt->size()) + eater->erases) {
        cout << "Mismatch detected !" <<endl;
    }
    int i = 0; // left-over count
    Dummy* d = 0;
    BOOST_CHECK( qt->size() <= QS );
    while( qt->size() != 0 ) {
        BOOST_CHECK( qt->dequeue(d) == true);
        BOOST_CHECK( d );
        i++;
        if ( i > QS ) {
            BOOST_CHECK( i <= QS); // avoid infinite loop.
            break;
        }
    }
    cout << "Left in Queue: "<< i <<endl;
    BOOST_CHECK( qt->dequeue(d) == false );
    BOOST_CHECK( qt->dequeue(d) == false );
    BOOST_CHECK( qt->isEmpty() );
    BOOST_CHECK_EQUAL( qt->size(), 0 );

    // assert: sum queues == sum dequeues
    BOOST_CHECK_EQUAL( aworker->appends + bworker->appends + cworker->appends + grower->appends,
                       aworker->erases + bworker->erases + cworker->erases + i + eater->erases );
    delete aworker;
    delete bworker;
    delete cworker;
    delete grower;
    delete eater;
}

BOOST_AUTO_TEST_CASE( testAtomicMWSRQueue )
{
    MWSRQueueType* qt = new MWSRQueueType(QS);
    AQGrower<MWSRQueueType>* aworker = new AQGrower<MWSRQueueType>( qt );
    AQGrower<MWSRQueueType>* bworker = new AQGrower<MWSRQueueType>( qt );
    AQGrower<MWSRQueueType>* cworker = new AQGrower<MWSRQueueType>( qt );
    AQGrower<MWSRQueueType>* grower = new AQGrower<MWSRQueueType>( qt );
    AQEater<MWSRQueueType>* eater = new AQEater<MWSRQueueType>( qt );

    {
        boost::scoped_ptr<Activity> athread( new Activity(20, aworker, "ActivityA" ));
        boost::scoped_ptr<Activity> bthread( new Activity(20, bworker, "ActivityB" ));
        boost::scoped_ptr<Activity> cthread( new Activity(20, cworker, "ActivityC" ));
        boost::scoped_ptr<Activity> gthread( new Activity(20, grower, "ActivityG"));
        boost::scoped_ptr<Activity> ethread( new Activity(20, eater, "ActivityE"));

        // avoid system lock-ups
        athread->thread()->setScheduler(ORO_SCHED_OTHER);
        bthread->thread()->setScheduler(ORO_SCHED_OTHER);
        cthread->thread()->setScheduler(ORO_SCHED_OTHER);
        gthread->thread()->setScheduler(ORO_SCHED_OTHER);
        ethread->thread()->setScheduler(ORO_SCHED_OTHER);

        log(Info) <<"Stressing multi-write/single-read..." <<endlog();
        athread->start();
        bthread->start();
        cthread->start();
        gthread->start();
        ethread->start();
        sleep(5);
        athread->stop();
        bthread->stop();
        cthread->stop();
        log(Info) <<"Stressing single-write/single-read..." <<endlog();
        sleep(5);
        gthread->stop();
        ethread->stop();
    }

    cout <<endl
         << "Total appends: " << aworker->appends + bworker->appends + cworker->appends+ grower->appends<<endl;
    cout << "Total erases : " << eater->erases <<endl;
    if (aworker->appends + bworker->appends + cworker->appends+ grower->appends != int(qt->size()) + eater->erases) {
        cout << "Mismatch detected !" <<endl;
    }
    int i = 0; // left-over count
    Dummy* d = 0;
    BOOST_CHECK( qt->size() <= QS );
    while( qt->size() != 0 ) {
        BOOST_CHECK( qt->dequeue(d) == true);
        BOOST_CHECK( d );
        i++;
        if ( i > QS ) {
            BOOST_CHECK( i <= QS); // avoid infinite loop.
            break;
        }
    }
    cout << "Left in Queue: "<< i <<endl;
    BOOST_CHECK( qt->dequeue(d) == false );
    BOOST_CHECK( qt->dequeue(d) == false );
    BOOST_CHECK( qt->isEmpty() );
    BOOST_CHECK_EQUAL( qt->size(), 0 );

    // assert: sum queues == sum dequeues
    BOOST_CHECK_EQUAL( aworker->appends + bworker->appends + cworker->appends + grower->appends,
                       i + eater->erases );
    delete aworker;
    delete bworker;
    delete cworker;
    delete grower;
    delete eater;
}
#endif
BOOST_AUTO_TEST_SUITE_END()
