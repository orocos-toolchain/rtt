/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  mqueue_archive_test.cpp

                        mqueue_archive_test.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "unit.hpp"

#include <boost/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#if BOOST_VERSION >= 106400
// The class name has been changed from boost::serialization::array<T> to array_wrapper<T> in Boost 1.61,
// but the header has only be renamed in Boost 1.64. Starting from Boost 1.65 array.hpp includes array_wrapper.hpp,
// but with 1.64 compilation fails if array_wrapper.hpp is not included.
# include <boost/serialization/array_wrapper.hpp>
#else
# include <boost/serialization/array.hpp>
#endif
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <rtt-fwd.hpp>
#include <transports/mqueue/binary_data_archive.hpp>
#include <os/fosi.h>

using namespace std;
using namespace boost::archive;
using namespace RTT::detail;
using namespace RTT::mqueue;
namespace io = boost::iostreams;

class MQueueArchiveTest
{
public:
    MQueueArchiveTest() { }
    ~MQueueArchiveTest() { }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MQueueArchiveTestSuite,  MQueueArchiveTest )

using namespace boost::serialization;

// Test writing a data sample + a vector into a binary data archive
// this must be a complete real-time operation. Use valgrind to check
// allocs.
BOOST_AUTO_TEST_CASE( testBinaryDataArchive )
{
    char sink[1000];
    memset( sink, 0, 1000);
    double d = 3.0;
    vector<double> c(10, 9.99);

    rtos_enable_rt_warning();
    io::stream<io::array_sink>  outbuf(sink,1000);
    binary_data_oarchive out( outbuf ); // +0 alloc
    out << d; // +0 alloc
    out << c; // +0 alloc
    rtos_disable_rt_warning();

    unsigned int stored = out.getArchiveSize();
    BOOST_CHECK( stored > 10*sizeof(double) );


    d = 0.0;
    c.clear();
    c.resize(20,0.0);

    rtos_enable_rt_warning();
    io::stream<io::array_source>  inbuf(sink,1000);
    binary_data_iarchive in( inbuf ); // +0 alloc
    in >> d; // +0 alloc
    in >> c; // +0 alloc
    rtos_disable_rt_warning();

    BOOST_CHECK_CLOSE( d, 3.0, 0.01);
    BOOST_CHECK_EQUAL( c.size(), 10);
    for(int i=0; i != 10; ++i) {
        BOOST_CHECK_CLOSE( c[i], 9.99, 0.01);
    }
    BOOST_CHECK_EQUAL( stored, in.getArchiveSize() );
}

/**
 * For serializing C-Style arrays, we need to use the make_array
 * or array<T> helper functions (aka 'wrappers') from boost::serialization.
 */
BOOST_AUTO_TEST_CASE( testFixedStringBinaryDataArchive )
{
    char sink[1000];
    memset( sink, 0, 1000);
    char c[10] = "123456789";

    rtos_enable_rt_warning();
    io::stream<io::array_sink>  outbuf(sink,1000);
    binary_data_oarchive out( outbuf ); // +0 alloc
    out << make_array(c, 10); // +0 alloc
    rtos_disable_rt_warning();

    unsigned int stored = out.getArchiveSize();
    BOOST_CHECK( stored >= 10*sizeof(char) );

    rtos_enable_rt_warning();
    io::stream<io::array_source>  inbuf(sink,1000);
    binary_data_iarchive in( inbuf ); // +0 alloc
#if BOOST_VERSION >= 106100
    boost::serialization::array_wrapper<char> ma = boost::serialization::make_array(c, 10);
#else
    boost::serialization::array<char> ma = boost::serialization::make_array(c, 10);
#endif
    in >> ma; // +0 alloc
    rtos_disable_rt_warning();

    BOOST_CHECK_EQUAL(c, "123456789");
    BOOST_CHECK_EQUAL( stored, in.getArchiveSize() );
}

/**
 * For serializing C-Style arrays created with make_nvp("array", make_array() )
 */
BOOST_AUTO_TEST_CASE( testMakeArrayBinaryDataArchive )
{
    char sink[1000];
    memset( sink, 0, 1000);
    double c[10] = {-1,1,2,3,4,5,6,7,8,9};
    double r[10] = {0,0,0,0,0,0,0,0,0,0};

    rtos_enable_rt_warning();
    io::stream<io::array_sink>  outbuf(sink,1000);
    binary_data_oarchive out( outbuf );
    out & make_nvp("array", make_array(c, 10) );
    rtos_disable_rt_warning();

    unsigned int stored = out.getArchiveSize();
    BOOST_CHECK( stored >= 10*sizeof(double) );

    rtos_enable_rt_warning();
    io::stream<io::array_source>  inbuf(sink,1000);
    binary_data_iarchive in( inbuf );
    in & make_nvp("array", make_array(r, 10) );
    rtos_disable_rt_warning();

    BOOST_CHECK_EQUAL(r[0], c[0]);
    BOOST_CHECK_EQUAL(r[4], c[4]);
    BOOST_CHECK_EQUAL(r[9], c[9]);
    BOOST_CHECK_EQUAL( stored, in.getArchiveSize() );
}

BOOST_AUTO_TEST_SUITE_END()

