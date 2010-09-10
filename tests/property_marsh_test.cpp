/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  property_marsh_test.cpp

                        property_marsh_test.cpp -  description
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

//! Test writing some properties and reading the same file back in.
BOOST_AUTO_TEST_CASE( testPropMarsh )
{
    std::string filename = "testPropMarsh.tst";

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

//! Test writing a vector to file and back in.
BOOST_AUTO_TEST_CASE( testPropMarshVect )
{
    std::string filename = "testPropMarshVect.tst";

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
    BOOST_CHECK_EQUAL( bag.rvalue().size(), 7 );

    // update bag -> array.
    BOOST_CHECK( updateProperties( source, target) );

    //p1 = source.getProperty("p1");
    BOOST_REQUIRE( p1->ready() );
    BOOST_CHECK_EQUAL( p1->rvalue().size() , 7 );
    BOOST_CHECK_EQUAL( p1->rvalue()[0] , 1.234 );

    deletePropertyBag( target );
    deletePropertyBag( source );
}

//! Test reading a legacy vector back in.
BOOST_AUTO_TEST_CASE( testPropMarshVectLegacy )
{
    PropertyBag target; // to file
    PropertyBag source; // from file

    Property<std::vector<double> >* p1 =  new Property<std::vector<double> >("driveLimits","p1d", std::vector<double>(7, 1.234) );

    // setup target tree
    target.addProperty( *p1 );
    {
        // scope required such that file is closed
        PropertyDemarshaller pd( "testPropMarshVectLegacy.cpf" );
        BOOST_REQUIRE( pd.deserialize( source ) );
    }
    // Check if the bag was read from file:
    Property<PropertyBag> bag = source.getProperty("driveLimits");
    BOOST_REQUIRE( bag.ready() );
    // check if legacy bag contains 7 elements:
    BOOST_CHECK_EQUAL( bag.value().size() , 7 );

    // update bag -> array.
    cout <<"target: "<< target <<endl;
    cout <<"source: "<< source <<endl;
    BOOST_CHECK( refreshProperties( target, source) );

    //p1 = target.getProperty("p1");
    BOOST_REQUIRE( p1->ready() );
    //cout << p1 << endl;
    // check updated vector size is 6, contents is 1:
    BOOST_CHECK_EQUAL( p1->rvalue().size() , 6 );
    BOOST_CHECK_EQUAL( p1->rvalue()[0] , 1 );

    deletePropertyBag( source );
}

BOOST_AUTO_TEST_SUITE_END()
