
#include <marsh/PropertyMarshaller.hpp>
#include <marsh/PropertyDemarshaller.hpp>
#include <Property.hpp>
#include <PropertyBag.hpp>

#include "unit.hpp"

class PropertyMarshTest
{
public:
    PropertyMarshTest()
    {
    }
    ~PropertyMarshTest()
    {
    }
};


BOOST_FIXTURE_TEST_SUITE( PropertyMarshTestSuite, PropertyMarshTest )

// This test does not yet test all types !
BOOST_AUTO_TEST_CASE( testPropMarsh )
{
    std::string filename = ".property_test.cpf";

    PropertyBag source; // to file
    PropertyBag target; // from file

    Property<PropertyBag> b1("b1","b1d");
    Property<PropertyBag> b2("b2","b2d");
    Property<int> p1("p1","p1d",-1);

    // setup source tree
    source.addProperty( b1 );
    b1.value().addProperty( b2 );
    b2.value().addProperty( p1 );

    {
        // scope required such that file is closed
        PropertyMarshaller pm( filename );
        pm.serialize( source );
    }

    {
        // scope required such that file is closed
        PropertyDemarshaller pd( filename );
        BOOST_REQUIRE( pd.deserialize( target ) );
    }

    Property<PropertyBag> bag = target.getProperty("b1");
    BOOST_REQUIRE( bag.ready() );
    BOOST_CHECK( bag.getDescription() == "b1d" );

    bag = bag.rvalue().getProperty("b2");
    BOOST_REQUIRE( bag.ready() );
    BOOST_CHECK( bag.getDescription() == "b2d" );

    Property<int> pi = bag.rvalue().getProperty("p1");
    BOOST_REQUIRE( pi.ready() );
    BOOST_CHECK( pi.get() == -1 );
    BOOST_CHECK( pi.getDescription() == "p1d" );
    deletePropertyBag( target );
}

BOOST_AUTO_TEST_CASE( testPropMarshVect )
{
    std::string filename = ".property_test_vect.cpf";

    PropertyBag source; // to file
    PropertyBag target; // from file

    Property<std::vector<double> >* p1 =  new Property<std::vector<double> >("p1","p1d", std::vector<double>(7, 1.234) );

    // setup source tree
    source.addProperty( *p1 );

    {
        // scope required such that file is closed
        PropertyMarshaller pm( filename );
        pm.serialize( source );
    }

    p1->set() = std::vector<double>(3, 0.234);
    {
        // scope required such that file is closed
        PropertyDemarshaller pd( filename );
        BOOST_REQUIRE( pd.deserialize( target ) );
    }

    // check bag:
    Property<PropertyBag> bag = target.getProperty("p1");
    BOOST_REQUIRE( bag.ready() );
    BOOST_CHECK( bag.getDescription() == "p1d" );
    BOOST_CHECK( bag.rvalue().size() == 7 );

    // update bag -> array.
    BOOST_CHECK( updateProperties( source, target) );

    //p1 = source.getProperty("p1");
    BOOST_REQUIRE( p1->ready() );
    BOOST_CHECK( p1->rvalue().size() == 7 );
    BOOST_CHECK( p1->rvalue()[0] == 1.234 );

    // Test legacy:
    deletePropertyBag( target );
    p1->setName("driveLimits");
    {
        // scope required such that file is closed
        PropertyDemarshaller pd( "property_test_vect.cpf" );
        BOOST_REQUIRE( pd.deserialize( target ) );
    }
    bag = target.getProperty("driveLimits");
    BOOST_REQUIRE( bag.ready() );
    BOOST_CHECK( bag.rvalue().size() == 7 );

    // update bag -> array.
    BOOST_CHECK( updateProperties( source, target) );

    //p1 = source.getProperty("p1");
    BOOST_REQUIRE( p1->ready() );
    //cout << p1 << endl;
    BOOST_CHECK( p1->rvalue().size() == 6 );
    BOOST_CHECK( p1->rvalue()[0] == 1 );

    deletePropertyBag( target );
    deletePropertyBag( source );
}

BOOST_AUTO_TEST_SUITE_END()
