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

#include <RTT.hpp>
#include <AtomicQueue.hpp>
#include <BufferLockFree.hpp>
#include <ListLockFree.hpp>
#include <DataObjectInterfaces.hpp>
#include <MemoryPool.hpp>
//#include <SortedList.hpp>

#include <os/SingleThread.hpp>
#include <os/Thread.hpp>
#include <rtt-config.h>

using namespace RTT;
using namespace RTT::OS;

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

class BuffersTest
{
public:

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

    BuffersTest(){setUp();};
    ~BuffersTest(){tearDown();};
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
