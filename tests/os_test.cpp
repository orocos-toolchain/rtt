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


#include "unit.hpp"

#include <rtt-config.h>

#include <rtt/os/Condition.hpp>
#include <rtt/os/Mutex.hpp>
#include <rtt/os/MutexLock.hpp>

using namespace RTT;
using namespace RTT::os;

BOOST_AUTO_TEST_SUITE(OsTestSuite)

BOOST_AUTO_TEST_CASE( testMutex )
{
    Mutex m;

    m.lock();
    m.unlock();
    {
        MutexLock lock(m);
    }

    BOOST_CHECK(m.trylock());
    m.unlock();
    {
        MutexTryLock lock(m);
        BOOST_CHECK(lock.isSuccessful());
        MutexTryLock lock2(m);
#if !defined(OROCOS_TARGET_XENOMAI)
        BOOST_CHECK(!lock2.isSuccessful());
#else
        // a Xenomai mutex is always recursive
        BOOST_CHECK(lock2.isSuccessful());
#endif
    }

    BOOST_CHECK(m.timedlock(Seconds(1.)));
    m.unlock();
    {
        MutexTimedLock lock(m, Seconds(1.));
        BOOST_CHECK(lock.isSuccessful());
        MutexTimedLock lock2(m, Seconds(1.));
#if !defined(OROCOS_TARGET_XENOMAI)
        BOOST_CHECK(!lock2.isSuccessful());
#else
        // a Xenomai mutex is always recursive
        BOOST_CHECK(lock2.isSuccessful());
#endif
    }

    BOOST_CHECK(m.timedlock(Seconds(1.)));
#if !defined(OROCOS_TARGET_XENOMAI)
    BOOST_CHECK(!m.trylock());
    BOOST_CHECK(!m.timedlock(Seconds(1.)));
#else
    // a Xenomai mutex is always recursive
#endif
    m.unlock();

    // Test locked mutex during destruction
    m.lock();
}

BOOST_AUTO_TEST_CASE( testMutexRecursive )
{
    MutexRecursive m;

    m.lock();
    m.lock();
    m.unlock();
    m.unlock();
    {
        MutexLock lock(m);
        MutexLock lock2(m);
    }

    BOOST_CHECK(m.trylock());
    BOOST_CHECK(m.trylock());
    m.unlock();
    m.unlock();
    {
        MutexTryLock lock(m);
        BOOST_CHECK(lock.isSuccessful());
        MutexTryLock lock2(m);
        BOOST_CHECK(lock2.isSuccessful());
    }

    BOOST_CHECK(m.timedlock(Seconds(1.)));
    BOOST_CHECK(m.timedlock(Seconds(1.)));
    m.unlock();
    m.unlock();
    {
        MutexTimedLock lock(m, Seconds(1.));
        BOOST_CHECK(lock.isSuccessful());
        MutexTimedLock lock2(m, Seconds(1.));
        BOOST_CHECK(lock2.isSuccessful());
    }

    BOOST_CHECK(m.timedlock(Seconds(1.)));
    BOOST_CHECK(m.trylock());
    m.unlock();
    m.unlock();

    // Test locked mutex during destruction
    m.lock();
}

BOOST_AUTO_TEST_SUITE_END()
