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
    EnumTypeTest() {  }
    ~EnumTypeTest() {  }
};

typedef enum { A, B, C } TheEnum;

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  EnumTypeTestSuite,  EnumTypeTest )

BOOST_AUTO_TEST_CASE( testEnumTypeInfo )
{
    Types()->addType( new EnumTypeInfo<TheEnum>("TheEnum") );

    AssignableDataSource<TheEnum>::shared_ptr a = new ValueDataSource<TheEnum>( A );
    AssignableDataSource<TheEnum>::shared_ptr b = new ValueDataSource<TheEnum>( B );
    AssignableDataSource<TheEnum>::shared_ptr c = new ValueDataSource<TheEnum>( C );

    BOOST_REQUIRE( Types()->type("TheEnum") );
    BOOST_REQUIRE( Types()->type("int") );

    // Test enum to int
    BOOST_CHECK( Types()->type("int")->convert( a ) );
    BOOST_CHECK( Types()->type("int")->convert( b ) );
    BOOST_CHECK( Types()->type("int")->convert( c ) );

    // Test enum to int to enum
    BOOST_CHECK( Types()->type("TheEnum")->convert( Types()->type("int")->convert(a) ) );
    BOOST_CHECK( Types()->type("TheEnum")->convert( Types()->type("int")->convert(b) ) );
    BOOST_CHECK( Types()->type("TheEnum")->convert( Types()->type("int")->convert(c) ) );

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
    BOOST_CHECK_EQUAL(dint->get(), (int)B );
}

BOOST_AUTO_TEST_SUITE_END()

