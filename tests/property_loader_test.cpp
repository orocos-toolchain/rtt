#include "unit.hpp"
#include "marsh/PropertyLoader.hpp"
#include "TaskContext.hpp"

struct LoaderTest {
    LoaderTest() : tc("tc") {}
    PropertyLoader pl;
    TaskContext tc;
};


BOOST_FIXTURE_TEST_SUITE( PropertyLoader, LoaderTest )

BOOST_AUTO_TEST_CASE( testPropLoading )
{
    std::string filename = "property_loading.cpf";
    BOOST_CHECK( pl.load(filename, &tc) );

    BOOST_CHECK( tc.hasProperty("load1") );
    BOOST_CHECK( tc.hasProperty("load2") );
    BOOST_CHECK( tc.hasProperty("bag1"));
    Property<PropertyBag> bag1 = tc.properties()->getProperty<PropertyBag>("bag1");
    BOOST_CHECK( bag1.ready() );
    BOOST_CHECK( bag1.getDescription() == "Bag1");
    PropertyBag& bag = bag1.value();
    BOOST_CHECK_EQUAL( bag.size(), 2);
    BOOST_CHECK( bag.find("bagload1"));
    BOOST_CHECK( bag.find("bagload2"));
    Property<int> bagload1 = bag.getProperty<int>("bagload1");
    Property<string> bagload2 = bag.getProperty<string>("bagload2");

    BOOST_CHECK(bagload1.ready());
    BOOST_CHECK_EQUAL(bagload1.get(), 3);
    BOOST_CHECK_EQUAL(bagload2.get(), "3 3 3");
}

BOOST_AUTO_TEST_SUITE_END()
