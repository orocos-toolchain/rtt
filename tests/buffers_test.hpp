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

#include <corelib/AtomicQueue.hpp>
#include <corelib/BufferLockFree.hpp>
#include <corelib/DataObjectInterfaces.hpp>

using namespace ORO_CoreLib;

class Dummy;

class BuffersTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( BuffersTest );
    CPPUNIT_TEST( testAtomic );
    CPPUNIT_TEST( testAtomicCounted );
    CPPUNIT_TEST( testBufLockFree );
    CPPUNIT_TEST( testDObjLockFree );
    CPPUNIT_TEST_SUITE_END();

    AtomicQueue<Dummy*>* aqueue;
    BufferLockFree<Dummy>* lockfree;
    DataObjectLockFree<Dummy>* dataobj;
public:

    void setUp();
    void tearDown();

    void testAtomic();
    void testAtomicCounted();
    void testBufLockFree();
    void testDObjLockFree();
};

#endif
