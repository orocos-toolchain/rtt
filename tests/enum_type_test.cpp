#include "unit.hpp"
#include <rtt-fwd.hpp>
#include <internal/DataSources.hpp>
#include <os/fosi.h>
#include <boost/lambda/lambda.hpp>

#include "datasource_fixture.hpp"
#include "marsh/PropertyBagIntrospector.hpp"
#include "types/EnumTypeInfo.hpp"

using namespace boost::lambda;

class EnumTypeTest
{
public:
    EnumTypeTest()
    {
    }
    ~EnumTypeTest()
    {
    }
};

typedef enum
{
    A, B, C
} TheEnum;

struct EnumStringTypeInfo: public EnumTypeInfo<TheEnum>
{
    EnumStringTypeInfo() :
        EnumTypeInfo<TheEnum> ("TheEnum")
    {
        to_string[A] = "A";
        to_string[B] = "B";
        to_string[C] = "C";
    }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( EnumTypeTestSuite, EnumTypeTest )

// Tests enum to int conversions
BOOST_AUTO_TEST_CASE( testEnumIntConversion )
{
    Types()->addType( new EnumTypeInfo<TheEnum>("TheEnum") );

    AssignableDataSource<TheEnum>::shared_ptr a = new ValueDataSource<TheEnum>( A );
    AssignableDataSource<TheEnum>::shared_ptr b = new ValueDataSource<TheEnum>( B );

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

// Tests enum to string conversions
BOOST_AUTO_TEST_CASE( testEnumStringConversion )
{
    Types()->addType( new EnumStringTypeInfo() );
    TypeInfo* ti = Types()->type("TheEnum");
    TypeInfo* ts = Types()->type("string");

    AssignableDataSource<TheEnum>::shared_ptr a = new ValueDataSource<TheEnum>( A );
    AssignableDataSource<TheEnum>::shared_ptr b = new ValueDataSource<TheEnum>( B );

    BOOST_REQUIRE( ti );
    BOOST_REQUIRE( ts );

    // Test enum convertType
    BOOST_REQUIRE( ti->convertType( a ) );
    BOOST_CHECK_EQUAL( ti->convertType( a )->getTypeInfo(), ts );

    // converts a to "A" and then back to enum and stores it in B.
    BOOST_CHECK( ti->composeType(ti->convertType(a), b) );

    BOOST_CHECK_EQUAL( a->get(), A);
    BOOST_CHECK_EQUAL( b->get(), A);

    b->set(B);

    // Test composition of enum from string.
    PropertyBag result;
    Property<TheEnum> pa("a","", a );
    Property<TheEnum> pb("a","", b );
    PropertyBagIntrospector pbi(result);
    pbi.introspect( &pa);
    BOOST_REQUIRE_EQUAL( result.size(), 1);
    BOOST_CHECK( result.getItem(0)->getTypeInfo() == ts );
    DataSource<string>::shared_ptr dstring = DataSource<string>::narrow( result.getItem(0)->getDataSource().get() );
    BOOST_CHECK_EQUAL(dstring->get(), "A" );

    pbi.introspect( &pb);
    BOOST_REQUIRE_EQUAL( result.size(), 2);
    BOOST_CHECK( result.getItem(1)->getTypeInfo() == ts );
    dstring = DataSource<string>::narrow( result.getItem(1)->getDataSource().get() );

    BOOST_CHECK_EQUAL(dstring->get(), "B" );
}

BOOST_AUTO_TEST_SUITE_END()

