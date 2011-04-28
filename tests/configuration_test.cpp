#include "unit.hpp"
#include <Attribute.hpp>
#include <Property.hpp>
#include <TaskContext.hpp>

/**
 * Fixture.
 */
class ConfigurationTestFixture
{
public:
    TaskContext tc;
    int i;
    Property<int> p;
    Attribute<int> a;
    Constant<int> c;

public:
    ConfigurationTestFixture()
    : tc("root"), i(0)
    {
    }

    ~ConfigurationTestFixture()
    {
    }
};


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  ConfigurationTestSuite,  ConfigurationTestFixture )

// Tests properties() interface:
BOOST_AUTO_TEST_CASE( testProperties)
{
    Property<double> d1("d1", "desc1", 1.234);
    tc.properties()->addProperty( d1 );

    BOOST_CHECK_EQUAL( double(1.234), d1.get() );
    BOOST_CHECK_EQUAL( double(1.234), tc.properties()->getPropertyType<double>("d1")->get() );

    // test setup of mirror:
    Property<string> s1;
    BOOST_CHECK( !s1.ready() );
    Property<string> s2("hello","description", "world");
    BOOST_CHECK( s2.ready() );

    BOOST_CHECK(tc.properties()->addProperty( s1 ) == false);
    BOOST_CHECK(tc.properties()->addProperty( s2 ) );
    s1 = tc.properties()->getProperty("hello");
    BOOST_CHECK( s1.ready() );

    BOOST_CHECK_EQUAL(std::string("hello"), s1.getName() );
    BOOST_CHECK_EQUAL(std::string("description"), s1.getDescription() );
    BOOST_CHECK_EQUAL(std::string("world"), s1.get() );

    // Test mirroring of 'set' and 'get':
    s1.set("universe");
    BOOST_CHECK_EQUAL(std::string("universe"), s2.get() );

}

// Tests attributes() interface:
BOOST_AUTO_TEST_CASE( testAttributes)
{
    // test attribute repository:
    Attribute<int> i1("i1");
    Attribute<double> d1("d1", 1.234);
    BOOST_CHECK( i1.ready() );
    BOOST_CHECK( d1.ready() );
    BOOST_CHECK(tc.addAttribute( d1 ));
    BOOST_CHECK(tc.addAttribute( i1 ));

    i1.set( 3 );
    BOOST_CHECK_EQUAL( double(1.234), d1.get() );
    BOOST_CHECK_EQUAL( int(3), i1.get() );

    d1 = tc.getAttribute("d1");
    BOOST_CHECK_EQUAL( double(1.234), d1.get() );
    Attribute<int> i2 = tc.getAttribute("i1");
    BOOST_CHECK_EQUAL( int(3),        i2.get() );

    // test setup of mirror:
    Attribute<string> s1;
    BOOST_CHECK( !s1.ready() );
    Attribute<string> s2("hello","world");
    BOOST_CHECK( s2.ready() );

    BOOST_CHECK(tc.addAttribute( s1 ) == false);
    BOOST_CHECK(tc.addAttribute( s2 ) );
    s1 = tc.getAttribute("hello");
    BOOST_CHECK( s1.ready() );

    BOOST_CHECK_EQUAL(std::string("hello"), s1.getName() );
    BOOST_CHECK_EQUAL(std::string("world"), s1.get() );

    // Test mirroring of 'set' and 'get':
    s1.set("universe");
    BOOST_CHECK_EQUAL(std::string("universe"), s2.get() );

}

// Tests add + double adds of the objects themselves:
BOOST_AUTO_TEST_CASE( testAddPropertyObjects )
{
    BOOST_CHECK( tc.provides()->addProperty("p",p).doc("foo").ready() );
    BOOST_CHECK( tc.provides()->addAttribute("a",a).ready() );
    BOOST_CHECK( tc.provides()->addConstant("c",c).ready() );

    BOOST_CHECK( p.ready() ); BOOST_CHECK_EQUAL( p.getName(), "p" ); BOOST_CHECK_EQUAL( p.getDescription(), "foo" );
    BOOST_CHECK( a.ready() ); BOOST_CHECK_EQUAL( a.getName(), "a" );
    BOOST_CHECK( c.ready() ); BOOST_CHECK_EQUAL( c.getName(), "c" );

    BOOST_CHECK( tc.provides()->addProperty("p2",p).doc("bar").ready() );
    BOOST_CHECK( tc.provides()->addAttribute("a2",a).ready() );
    BOOST_CHECK( tc.provides()->addConstant("c2",c).ready() );

    BOOST_CHECK( p.ready() ); BOOST_CHECK_EQUAL( p.getName(), "p2" ); BOOST_CHECK_EQUAL( p.getDescription(), "bar" );
    BOOST_CHECK( a.ready() ); BOOST_CHECK_EQUAL( a.getName(), "a2" );
    BOOST_CHECK( c.ready() ); BOOST_CHECK_EQUAL( c.getName(), "c2" );
}

// Tests add + double adds of a property/attribute/constant reference to a member
BOOST_AUTO_TEST_CASE( testAddProperties )
{
    // Tests add + double adds of the objects themselves:
    i = 3;
    BOOST_CHECK( tc.provides()->addProperty("p",i).doc("foo").ready() );
    BOOST_CHECK( tc.provides()->addAttribute("a",i) );
    BOOST_CHECK( tc.provides()->addConstant("c",i) );

    p = tc.provides()->getProperty("p");
    a = tc.provides()->getAttribute("a");
    c = tc.provides()->getConstant("c");

    BOOST_CHECK( p.ready() ); BOOST_CHECK_EQUAL( p.getName(), "p" ); BOOST_CHECK_EQUAL( p.getDescription(), "foo" ); BOOST_CHECK_EQUAL( p.get(), 3);
    BOOST_CHECK( a.ready() ); BOOST_CHECK_EQUAL( a.getName(), "a" ); BOOST_CHECK_EQUAL( a.get(), 3);
    BOOST_CHECK( c.ready() ); BOOST_CHECK_EQUAL( c.getName(), "c" ); BOOST_CHECK_EQUAL( c.get(), 3);
    BOOST_CHECK_EQUAL(i , 3); // not modified 'i'

    // check reference to i:
    i = 4;
    BOOST_CHECK_EQUAL( p.get(), 4);
    BOOST_CHECK_EQUAL( p.get(), 4);
    BOOST_CHECK_EQUAL( p.get(), 4);

    BOOST_CHECK( tc.provides()->addProperty("p2",i).doc("bar").ready() );
    BOOST_CHECK( tc.provides()->addAttribute("a2",i) );
    BOOST_CHECK( tc.provides()->addConstant("c2",i) );

    p = tc.provides()->getProperty("p2");
    a = tc.provides()->getAttribute("a2");
    c = tc.provides()->getConstant("c2");

    BOOST_CHECK( p.ready() ); BOOST_CHECK_EQUAL( p.getName(), "p2" ); BOOST_CHECK_EQUAL( p.getDescription(), "bar" ); BOOST_CHECK_EQUAL( p.get(), 4);
    BOOST_CHECK( a.ready() ); BOOST_CHECK_EQUAL( a.getName(), "a2" ); BOOST_CHECK_EQUAL( a.get(), 4);
    BOOST_CHECK( c.ready() ); BOOST_CHECK_EQUAL( c.getName(), "c2" ); BOOST_CHECK_EQUAL( c.get(), 4);
}


BOOST_AUTO_TEST_SUITE_END()

