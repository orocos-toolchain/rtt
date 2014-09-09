/***************************************************************************
  tag: Intermodalics BVBA  Mon Sep 8 2014  tlsf_test.cpp

                        tlsf_test.cpp -  description
                           -------------------
    begin                : Mon September 08 2014
    copyright            : (C) 2014 Intermodalics BVBA
    email                : ruben@intermodalics.eu

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define ORO_MEMORY_POOL

#include "unit.hpp"
#include <rtt/os/tlsf/tlsf.h>
#include <signal.h>

void signal_handler(int sig_num){
    if(sig_num == SIGABRT){
        exit(0);
    }
}

class TLSFTest
{
public:
    void* rtMem;
    void setUp(){
        rtMem = malloc(10000);
        BOOST_CHECK(rtMem);
        BOOST_CHECK_LE(0,(int)(init_memory_pool(10000,rtMem)));
    }
    void tearDown(){
        destroy_memory_pool(rtMem);
        free(rtMem);
    }
    TLSFTest(){ setUp(); };
    ~TLSFTest(){ tearDown(); };
};

BOOST_FIXTURE_TEST_SUITE(TLSFTestSuite, TLSFTest)

BOOST_AUTO_TEST_CASE(testCreateAndDestroy)
{
    //Do nothing, see if memory pool is created and destroyed without error
}

BOOST_AUTO_TEST_CASE(testInitAllocFree){
    void* a = 0;
    a = oro_rt_malloc(100);//Allocate
    BOOST_CHECK(a);
    oro_rt_free(a);//Free
}

BOOST_AUTO_TEST_CASE(testAllocToBig){
    void* a = 0;
    a = oro_rt_malloc(20000);//Allocation bigger than memorypool should fail
    BOOST_CHECK(!a);
}


BOOST_AUTO_TEST_CASE(testInitTwiceSameMemPool){
    BOOST_CHECK_LE(0,(int)(init_memory_pool(10000,rtMem)));//Allocation of the same memory pool should succeed
    BOOST_CHECK_LE(0,(int)(init_memory_pool(9000,rtMem)));//Allocation of the same memory pool but smaller should succeed
    BOOST_CHECK_LE(0,(int)(init_memory_pool(15000,rtMem)));//Allocation of the same memory pool but bigger should fail
}

BOOST_AUTO_TEST_CASE(testUseSecondaryMemPool){
    void* a = 0;
    a = oro_rt_malloc(500);
    BOOST_CHECK(a);
    void* rtMem_p = malloc(15000);
    BOOST_CHECK_LE(0,(int)init_memory_pool(15000,rtMem_p));
    void* b = 0;
    b = malloc_ex(500,rtMem_p);
    BOOST_CHECK(b);
    free_ex(b,rtMem_p);
    destroy_memory_pool(rtMem_p);
    oro_rt_free(a);
    a = 0;
    a = oro_rt_malloc(500);
    BOOST_CHECK(a);
    oro_rt_free(a);
}

BOOST_AUTO_TEST_CASE(testDoubleFree)
{
    signal(SIGABRT,&signal_handler);
    void* a = oro_rt_malloc(500);
    oro_rt_free(a);
    oro_rt_free(a);
    BOOST_FAIL("Double free undetected");
}

BOOST_AUTO_TEST_SUITE_END()
