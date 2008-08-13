/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  buffers_test.hpp

                        buffers_test.hpp -  description
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


#ifndef BUFFERSTEST_H
#define BUFFERSTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <RTT.hpp>
#include <AtomicQueue.hpp>
#include <BufferLockFree.hpp>
#include <ListLockFree.hpp>
#include <DataObjectInterfaces.hpp>
#include <MemoryPool.hpp>
//#include <SortedList.hpp>

#include <os/SingleThread.hpp>
#include <rtt-config.h>

using namespace RTT;
using namespace RTT::OS;

class Dummy;

typedef AtomicQueue<Dummy*> QueueType;

class BuffersTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( BuffersTest );
    CPPUNIT_TEST( testAtomic );
    CPPUNIT_TEST( testAtomicCounted );
#ifdef OROPKG_OS_GNULINUX
    // this test assumes a not real-time OS.
    //CPPUNIT_TEST( testListLockFree );
    CPPUNIT_TEST( testAtomicQueue );
#endif
    CPPUNIT_TEST( testBufLockFree );
    CPPUNIT_TEST( testDObjLockFree );
    //CPPUNIT_TEST( testSortedList );
    CPPUNIT_TEST( testMemoryPool );
    CPPUNIT_TEST_SUITE_END();

    AtomicQueue<Dummy*>* aqueue;

    BufferLockFree<Dummy>* lockfree;
    DataObjectLockFree<Dummy>* dataobj;
    //SortedList<Dummy>* mslist;

    ThreadInterface* athread;
    ThreadInterface* bthread;

    ListLockFree<Dummy>* listlockfree;
    MemoryPool<Dummy>* mpool;
    MemoryPool<std::vector<Dummy> >* vpool;
    FixedSizeMemoryPool<Dummy>* fmpool;
    FixedSizeMemoryPool<std::vector<Dummy> >* fvpool;
public:

    void setUp();
    void tearDown();

    void testAtomic();
    void testAtomicCounted();
    void testAtomicQueue();
    void testBufLockFree();
    void testDObjLockFree();
    void testMemoryPool();

    void testSortedList();

    void testListLockFree();
};

#endif
