/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  enum_type_test.cpp

                        enum_type_test.cpp -  description
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
#include "rtt/rtt-fwd.hpp"
#include "rtt/internal/DataSources.hpp"
#include "rtt/os/fosi.h"

#include "tests/datasource_fixture.hpp"
#include "rtt/marsh/PropertyBagIntrospector.hpp"
#include "rtt/types/EnumTypeInfo.hpp"
#include "rtt/marsh/PropertyLoader.hpp"
#include "rtt/TaskContext.hpp"

typedef enum
{
    A, B, C
} TheEnum;

class EnumTypeTest
{
public:
    TheEnum enum1;
    TheEnum enum2;

    AssignableDataSource<TheEnum>::shared_ptr a; //! Contains 'A'
    AssignableDataSource<TheEnum>::shared_ptr b; //! Contains 'B'
    TypeInfo* ti; //! Type info for enum
    TypeInfo* ts; //! Type info for 'string'

    EnumTypeTest()
    {

        a = new ValueDataSource<TheEnum>( A );
        b = new ValueDataSource<TheEnum>( B );

        Types()->addType( new EnumTypeInfo<TheEnum>("TheEnum") );
    }
    ~EnumTypeTest()
    {
    }
};


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( EnumTypeTestSuite, EnumTypeTest )

// Tests enum to int conversions
BOOST_AUTO_TEST_CASE( testEnumIntConversion )
{

    BOOST_REQUIRE( Types()->type("TheEnum") );
    BOOST_REQUIRE( Types()->type("int") );

    // Test enum to int
    BOOST_CHECK( Types()->type("int")->convert( a ) );
    BOOST_CHECK( Types()->type("int")->convert( b ) );

    // Test enum to int to enum
    BOOST_CHECK( Types()->type("TheEnum")->convert( Types()->type("int")->convert(a) ) );
    BOOST_CHECK( Types()->type("TheEnum")->convert( Types()->type("int")->convert(b) ) );

    // Test composition of enum from int.
    PropertyBag result;
    Property<TheEnum> pa("a","", a );
    Property<TheEnum> pb("a","", b );
    PropertyBagIntrospector pbi(result);
    pbi.introspect( &pa);
    BOOST_REQUIRE_EQUAL( result.size(), 1);
    BOOST_CHECK( result.getItem(0)->getTypeInfo() == Types()->type("int") );
    DataSource<int>::shared_ptr dint = DataSource<int>::narrow( result.getItem(0)->getDataSource().get() );
    BOOST_CHECK_EQUAL(dint->get(), (int)A );

    pbi.introspect( &pb);
    BOOST_REQUIRE_EQUAL( result.size(), 2);
    BOOST_CHECK( result.getItem(1)->getTypeInfo() == Types()->type("int") );
    dint = DataSource<int>::narrow( result.getItem(1)->getDataSource().get() );
    BOOST_REQUIRE( dint );
    BOOST_CHECK_EQUAL(dint->get(), (int)B );
}


// Tests enum to file and back
BOOST_AUTO_TEST_CASE( testEnumSaveIntProperties )
{
    TaskContext tc("TC");
    PropertyLoader pl(&tc);
    enum1 = A;
    enum2 = B;
    tc.addProperty("enum1", enum1 );
    tc.addProperty("enum2", enum2 );

    // saves A and B
    BOOST_CHECK( pl.save("enum_type_test_int_save.cpf", true) );

    enum1 = B;
    enum2 = A;

    // restores A and B
    BOOST_CHECK( pl.load("enum_type_test_int_save.cpf") );

    BOOST_CHECK_EQUAL( enum1, A);
    BOOST_CHECK_EQUAL( enum2, B);
}

BOOST_AUTO_TEST_SUITE_END()

