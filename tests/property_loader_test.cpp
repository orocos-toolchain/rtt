#include "unit.hpp"
#include "marsh/PropertyLoader.hpp"
#include "TaskContext.hpp"

struct LoaderTest {
    LoaderTest() : tc("tc"),
            pstring("pstring","pstringd","Hello World"),
            pchar("pchar","pchard",'H'),
            pdouble("pdouble", "pdoubled", 1.23456)
    {}
    PropertyLoader pl;
    TaskContext tc;
    Property<string> pstring;
    Property<char> pchar;
    Property<double> pdouble;
};


BOOST_FIXTURE_TEST_SUITE( PropertyLoaderTest, LoaderTest )

BOOST_AUTO_TEST_CASE( testPropWriting )
{
    std::string filename = "property_writing.cpf";
    tc.addProperty(pstring);
    tc.addProperty(pchar);
    tc.addProperty(pdouble);

    BOOST_CHECK( pl.save(filename, &tc) );
}

BOOST_AUTO_TEST_CASE( testPropLoading )
{
    std::string filename = "property_loading.cpf";
    BOOST_REQUIRE( pl.load(filename, &tc) );

    BOOST_CHECK( tc.provides()->hasProperty("load1") );
    BOOST_CHECK( tc.provides()->hasProperty("load2") );
    BOOST_CHECK( tc.provides()->hasProperty("bag1"));
    Property<PropertyBag> bag1 = tc.properties()->getProperty("bag1");
    BOOST_REQUIRE( bag1.ready() );
    BOOST_CHECK( bag1.getDescription() == "Bag1");
    PropertyBag& bag = bag1.value();
    BOOST_CHECK_EQUAL( bag.size(), 2);
    BOOST_CHECK( bag.find("bagload1"));
    BOOST_CHECK( bag.find("bagload2"));
    Property<int> bagload1 = bag.getProperty("bagload1");
    Property<string> bagload2 = bag.getProperty("bagload2");

    BOOST_REQUIRE(bagload1.ready());
    BOOST_CHECK_EQUAL(bagload1.get(), 3);
    BOOST_CHECK_EQUAL(bagload2.get(), "3 3 3");
}

BOOST_AUTO_TEST_SUITE_END()
