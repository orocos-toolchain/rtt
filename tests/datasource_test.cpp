/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  datasource_test.cpp

                        datasource_test.cpp -  description
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


#include "tests/unit.hpp"

#include <boost/array.hpp>

#include "rtt/rtt-fwd.hpp"

#include "rtt/internal/DataSources.hpp"
#include "rtt/internal/PartDataSource.hpp"
#include "rtt/internal/ArrayPartDataSource.hpp"
//#include <internal/OffsetPartDataSource.hpp>
#include "rtt/internal/carray.hpp"

#include <boost/serialization/array.hpp>

#include "rtt/os/fosi.h"
#include "tests/datasource_fixture.hpp"

class DataSourceTest
{
public:
    AType atype;
    AType atype_orig;

    BType btype;
    BType btype_orig;
    DataSourceTest() {
        atype.init();
        atype_orig.init();
        btype.init();
        btype_orig.init();
    }
    ~DataSourceTest() {  }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  DataSourceTestSuite,  DataSourceTest )

// Test semantics of ValueDataSource and test fixture.
BOOST_AUTO_TEST_CASE( testValueDataSource )
{
    AssignableDataSource<AType>::shared_ptr d = new ValueDataSource<AType>( atype );

    // Test fixture:
    BOOST_CHECK_EQUAL( atype, atype );
    BOOST_CHECK_EQUAL( atype, atype_orig );

    // Do not take by reference:
    BOOST_CHECK_EQUAL( d->set(), atype );
    atype.a = -atype.a;
    BOOST_CHECK_EQUAL( d->set(), atype_orig );
}

// Test constant DataSource.
BOOST_AUTO_TEST_CASE( testConstantDataSource )
{
    DataSource<AType>::shared_ptr d = new ConstantDataSource<AType>( atype );

    // Do not take by reference:
    BOOST_CHECK_EQUAL( d->rvalue(), atype );
    atype.a = -atype.a;
    BOOST_CHECK_EQUAL( d->rvalue(), atype_orig );
}

// Test reference to C++ data
BOOST_AUTO_TEST_CASE( testReferenceDataSource )
{
    AssignableDataSource<AType>::shared_ptr d = new ReferenceDataSource<AType>( atype );

    // Take by reference:
    BOOST_CHECK_EQUAL( d->set(), atype );
    atype.a = -atype.a;
    BOOST_CHECK_EQUAL( d->set(), atype );

    d->set().a = 55;
    BOOST_CHECK_EQUAL( d->set(), atype );
}

// Test const reference to C++ data
BOOST_AUTO_TEST_CASE( testConstReferenceDataSource )
{
    DataSource<AType>::shared_ptr d = new ConstReferenceDataSource<AType>( atype );

    // Take by reference:
    BOOST_CHECK_EQUAL( d->rvalue(), atype );
    atype.a = -atype.a;
    BOOST_CHECK_EQUAL( d->rvalue(), atype );
}

// Test part reference to C++ data
BOOST_AUTO_TEST_CASE( testPartDataSource )
{
    DataSource<AType>::shared_ptr abase = new UpdatedReferenceDataSource<AType>( atype );
    AssignableDataSource<string>::shared_ptr d = new PartDataSource<string>( atype.c, abase );

    // Take string by reference:
    BOOST_CHECK_EQUAL( d->set(), atype.c );
    atype.c = "world";
    BOOST_CHECK_EQUAL( d->set(), atype.c );

    // Test both versions of set(). This also checks updated():
    d->set( "Long gone!" );
    BOOST_CHECK_EQUAL( d->set(), abase->rvalue().c );
    BOOST_CHECK_EQUAL( d->set(), atype.c );

    d->set() = "And back!";
    d->updated();
    BOOST_CHECK_EQUAL( d->set(), abase->rvalue().c );
    BOOST_CHECK_EQUAL( d->set(), atype.c );
}

//! Test C-style array as DataSource
//! Only applies to BType objects and uses carray for this.
BOOST_AUTO_TEST_CASE( testArrayDataSource )
{
    DataSource<BType>::shared_ptr abase = new UpdatedReferenceDataSource<BType>( btype );
    AssignableDataSource<carray<char> >::shared_ptr d =
            new ValueDataSource<carray<char> >( carray<char>(btype.c, 10) );

    // Take string by reference (!):
    BOOST_CHECK_EQUAL( d->set().address(), btype.c );
    strcpy( btype.c, "world");
    BOOST_CHECK_EQUAL( "world", btype.c );
    BOOST_CHECK_EQUAL( d->set().address(), btype.c );

    // Test both versions of set(). This also checks updated():
    *d->set().address() = 'L';
    d->updated();
    BOOST_CHECK_EQUAL( "Lorld", btype.c );
    BOOST_CHECK_EQUAL( "Lorld", d->rvalue().address() );
    BOOST_CHECK_EQUAL( d->set().address(), btype.c );
}

//! Test reference to part of C-style array
//! Only applies to BType objects.
BOOST_AUTO_TEST_CASE( testArrayPartDataSource )
{
    AssignableDataSource<unsigned int>::shared_ptr index = new ValueDataSource<unsigned int>(0);
    DataSource<BType>::shared_ptr abase = new UpdatedReferenceDataSource<BType>( btype );
    AssignableDataSource<char>::shared_ptr d = new ArrayPartDataSource<char>( *btype.c, index, abase, sizeof(btype.c) );

    // Take string by reference:
    BOOST_CHECK_EQUAL( &d->set(), btype.c );
    strcpy( btype.c, "world");
    BOOST_CHECK_EQUAL( "world", btype.c );
    BOOST_CHECK_EQUAL( &d->set(), btype.c );

    // Test both versions of set(). This also checks updated():
    d->set( 'L' );
    BOOST_CHECK_EQUAL( "Lorld", btype.c );
    BOOST_CHECK( strcmp( &d->set(), abase->rvalue().c) == 0 );
    BOOST_CHECK_EQUAL( &d->set(), btype.c );

    d->set() = 'W';
    d->updated();
    BOOST_CHECK_EQUAL( "World", btype.c );
    BOOST_CHECK( strcmp( &d->set(), abase->rvalue().c) == 0 );
    BOOST_CHECK_EQUAL( &d->set(), btype.c );

    index->set( 3 );
    d->set( 'L' );
    BOOST_CHECK_EQUAL( "WorLd", btype.c );
    BOOST_CHECK( strcmp( &d->set(), &abase->rvalue().c[3]) == 0 );
    BOOST_CHECK_EQUAL( &d->set(), &btype.c[3] );
}


BOOST_AUTO_TEST_SUITE_END()

