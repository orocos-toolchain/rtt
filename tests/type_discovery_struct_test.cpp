/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  type_discovery_struct_test.cpp

                        type_discovery_struct_test.cpp -  description
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

#include <boost/serialization/vector.hpp>
#include <boost/array.hpp>

#include "rtt/rtt-fwd.hpp"
#include "rtt/internal/DataSources.hpp"
#include "rtt/types/type_discovery.hpp"
#include "rtt/os/fosi.h"

#include "tests/datasource_fixture.hpp"
#include "rtt/types/StructTypeInfo.hpp"
#include "rtt/types/CArrayTypeInfo.hpp"
#include "rtt/types/SequenceTypeInfo.hpp"
#include "rtt/types/BoostArrayTypeInfo.hpp"

using namespace boost::archive;
using namespace boost::serialization;

class StructTypeTest
{
public:
    StructTypeTest() {  }
    ~StructTypeTest() {  }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  TypeArchiveTestSuite,  StructTypeTest )

// Test the StructTypeInfo for AType
// Similar as the above tests, but now through the TypeInfo system.
BOOST_AUTO_TEST_CASE( testATypeStruct )
{
    Types()->addType( new StructTypeInfo<AType>("AType") );

    AssignableDataSource<AType>::shared_ptr atype = new ValueDataSource<AType>( AType(true) );

    BOOST_REQUIRE( Types()->type("AType") );

    // check the part names lookup:
    vector<string> names = atype->getMemberNames();
    BOOST_CHECK_EQUAL( atype->getMemberNames().size(), 5 );

    BOOST_REQUIRE_EQUAL( names.size(), 5);
    BOOST_REQUIRE( atype->getMember("a") );

    // Check individual part lookup by name:
    AssignableDataSource<int>::shared_ptr a = AssignableDataSource<int>::narrow( atype->getMember("a").get() );
    AssignableDataSource<double>::shared_ptr b = AssignableDataSource<double>::narrow( atype->getMember("b").get() );
    AssignableDataSource<string>::shared_ptr c = AssignableDataSource<string>::narrow( atype->getMember("c").get());
    AssignableDataSource<carray<int> >::shared_ptr ai = AssignableDataSource<carray<int> >::narrow( atype->getMember("ai").get());
    AssignableDataSource<vector<double> >::shared_ptr vd = AssignableDataSource<vector<double> >::narrow( atype->getMember("vd").get());

    BOOST_REQUIRE( a );
    BOOST_REQUIRE( b );
    BOOST_REQUIRE( c );
    BOOST_REQUIRE( ai );
    BOOST_REQUIRE( vd );

    BOOST_CHECK( !atype->getMember("zort") );

    // Check reading parts (must equal parent)
    BOOST_CHECK_EQUAL( a->get(), atype->get().a );
    BOOST_CHECK_EQUAL( b->get(), atype->get().b );
    BOOST_CHECK_EQUAL( c->get(), atype->get().c );
    BOOST_CHECK_EQUAL( ai->get().address()[3], atype->get().ai[3] );
    BOOST_CHECK_EQUAL( vd->get()[3], atype->get().vd[3] );

    // Check writing a part (must change in parent too).
    a->set(10);
    BOOST_CHECK_EQUAL( a->get(), 10 );
    BOOST_CHECK_EQUAL( a->get(), atype->get().a );
}

//! Tests complex type introspection (sequence of structs)
BOOST_AUTO_TEST_CASE( testCTypeStruct )
{
    Types()->addType( new StructTypeInfo< AType >("AType") );
    Types()->addType( new StructTypeInfo< BType >("BType") );
    Types()->addType( new StructTypeInfo< CType >("CType") );
    Types()->addType( new SequenceTypeInfo< vector<AType> >("as") );
    Types()->addType( new SequenceTypeInfo< vector<BType> >("bs") );
    Types()->addType( new CArrayTypeInfo< carray<int> >("cints") );
    Types()->addType( new BoostArrayTypeInfo< boost::array<int,5> >("int5") );
    Types()->addType( new SequenceTypeInfo< vector<int> >("ints") );

    AssignableDataSource<CType>::shared_ptr atype = new ValueDataSource<CType>( CType(true) );

    // decompose a complex type
    AssignableDataSource<AType>::shared_ptr a = AssignableDataSource<AType>::narrow( atype->getMember("a").get() );
    AssignableDataSource<BType>::shared_ptr b = AssignableDataSource<BType>::narrow( atype->getMember("b").get() );
    AssignableDataSource< vector<AType> >::shared_ptr av = AssignableDataSource< vector<AType> >::narrow( atype->getMember("av").get());
    AssignableDataSource< vector<BType> >::shared_ptr bv = AssignableDataSource< vector<BType> >::narrow( atype->getMember("bv").get());

    BOOST_REQUIRE( a );
    BOOST_REQUIRE( b );
    BOOST_REQUIRE( av );
    BOOST_REQUIRE( bv );

    // Access top level elements
    BOOST_REQUIRE( a->getMember("ai") );
    AssignableDataSource<int>::shared_ptr ai3 = dynamic_pointer_cast< AssignableDataSource<int> >( a->getMember("ai")->getMember("3") );
    BOOST_REQUIRE( b->getMember("ai") );
    AssignableDataSource<int>::shared_ptr bi3 = dynamic_pointer_cast< AssignableDataSource<int> >( b->getMember("ai")->getMember("3") );

    // Access elements in sequences:
    AssignableDataSource<int>::shared_ptr avi3 = dynamic_pointer_cast< AssignableDataSource<int> >( av->getMember("3")->getMember("ai")->getMember("3") );
    AssignableDataSource<int>::shared_ptr bvi3 = dynamic_pointer_cast< AssignableDataSource<int> >( bv->getMember("3")->getMember("ai")->getMember("3") );

    BOOST_REQUIRE( ai3 );
    BOOST_REQUIRE( bi3 );
    BOOST_REQUIRE( avi3 );
    BOOST_REQUIRE( bvi3 );

    // Check reading parts (must equal parent)
    BOOST_CHECK_EQUAL( a->get(), atype->get().a );
    BOOST_CHECK_EQUAL( b->get(), atype->get().b );
    BOOST_CHECK( std::equal(av->set().begin(), av->set().end(), atype->set().av.begin() ) );
    BOOST_CHECK( std::equal(bv->set().begin(), bv->set().end(), atype->set().bv.begin() ) );
    BOOST_CHECK_EQUAL( avi3->get(), atype->get().av[3].ai[3] );
    BOOST_CHECK_EQUAL( bvi3->get(), atype->get().bv[3].ai[3] );

    // Check writing a part (must change in parent too).
    avi3->set(10);
    bvi3->set(20);
    BOOST_CHECK_EQUAL( avi3->get(), 10 );
    BOOST_CHECK_EQUAL( avi3->get(), atype->get().av[3].ai[3] );
    BOOST_CHECK_EQUAL( bvi3->get(), 20 );
    BOOST_CHECK_EQUAL( bvi3->get(), atype->get().bv[3].ai[3] );
}

BOOST_AUTO_TEST_SUITE_END()

