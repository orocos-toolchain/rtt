/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  property_loader_test.cpp

                        property_loader_test.cpp -  description
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


#include "unit.hpp"
#include "marsh/PropertyLoader.hpp"
#include "TaskContext.hpp"

struct LoaderTest {
    LoaderTest() : tc("tc"), pl(&tc),
            pstring("pstring","pstringd","Hello World"),
            pchar("pchar","pchard",'H'),
            pint("pint", "pintd", -2),
            puint("puint", "puintd", 3),
            pllong("pllong", "pllongd", 2147483648),
            pullong("pullong", "pullongd", 18446744073709551615ull),
            pdouble("pdouble", "pdoubled", 1.23456),
            pbag("pbag","pbagd"),
            pints("pints", "pintsd", vector<int>(3,4)),
            pdoubles("pdoubles", "pdoublesd", vector<double>(3,4.123))
    {

    }
    TaskContext tc;
    PropertyLoader pl;
    Property<string> pstring;
    Property<char> pchar;
    Property<int> pint;
    Property<unsigned int> puint;
    Property<long long> pllong;
    Property<unsigned long long> pullong;
    Property<double> pdouble;
    Property<PropertyBag> pbag;
    Property<std::vector<int> > pints;
    Property<std::vector<double> > pdoubles;
    PropertyBag bag;
};


BOOST_FIXTURE_TEST_SUITE( PropertyLoaderTest, LoaderTest )

/**
 * Test saving and loading the properties using primitive
 * types and sub-sub-bags.
 */
BOOST_AUTO_TEST_CASE( testPropSaveLoad )
{
    std::string filename = "property_writing.tst";
    tc.addProperty(pstring);
    tc.addProperty(pchar);
    tc.addProperty(pint);
    tc.addProperty(puint);
    tc.addProperty(pllong);
    tc.addProperty(pullong);
    tc.addProperty(pdouble);
    tc.addProperty(pdoubles);
    tc.addProperty("newbag", bag).doc("newbag doc"); // takes reference to bag (PropertyBag&)
    bag.addProperty( pdouble );
    bag.addProperty( pdoubles );
    bag.addProperty( pbag ); // takes reference to bpag (PropertyBase&)
    pbag.value().addProperty( pchar );

    // deep copy original property values
    RTT::PropertyBag orig;
    RTT::copyProperties(orig, *tc.properties());

    // save all to fresh file
    BOOST_CHECK( pl.save(filename, true) );
    // configure all from file.
    BOOST_CHECK( pl.configure(filename, true) );

    // compare property values
    BOOST_CHECK_EQUAL( pstring.value(), orig.getPropertyType<std::string>("pstring")->value() );
    BOOST_CHECK_EQUAL( pchar.value(), orig.getPropertyType<char>("pchar")->value() );
    BOOST_CHECK_EQUAL( pint.value(), orig.getPropertyType<int>("pint")->value() );
    BOOST_CHECK_EQUAL( puint.value(), orig.getPropertyType<unsigned int>("puint")->value() );
    BOOST_CHECK_EQUAL( pllong.value(), orig.getPropertyType<long long>("pllong")->value() );
    BOOST_CHECK_EQUAL( pullong.value(), orig.getPropertyType<unsigned long long>("pullong")->value() );
    BOOST_CHECK_EQUAL( pdouble.value(), orig.getPropertyType<double>("pdouble")->value() );
    BOOST_CHECK_EQUAL_COLLECTIONS(
        pdoubles.value().begin(),
        pdoubles.value().end(),
        orig.getPropertyType< vector<double> >("pdoubles")->value().begin(),
        orig.getPropertyType< vector<double> >("pdoubles")->value().end()
    );
    BOOST_CHECK_EQUAL( bag.getPropertyType<double>("pdouble")->value(), orig.getPropertyType<RTT::PropertyBag>("newbag")->value().getPropertyType<double>("pdouble")->value() );
    BOOST_CHECK_EQUAL_COLLECTIONS(
        bag.getPropertyType< vector<double> >("pdoubles")->value().begin(),
        bag.getPropertyType< vector<double> >("pdoubles")->value().end(),
        orig.getPropertyType<RTT::PropertyBag>("newbag")->value().getPropertyType< vector<double> >("pdoubles")->value().begin(),
        orig.getPropertyType<RTT::PropertyBag>("newbag")->value().getPropertyType< vector<double> >("pdoubles")->value().end()
    );
    BOOST_CHECK_EQUAL( pchar.value(), orig.getPropertyType<RTT::PropertyBag>("newbag")->value().getPropertyType<RTT::PropertyBag>("pbag")->value().getPropertyType<char>("pchar")->value() );

    // configure all fails with one missing element.
    bag.addProperty( pstring );

    // TEST FAILS : TO BE FIXED in updateProperties()
    //BOOST_REQUIRE( !pl.configure(filename, true) );

    // configure some does not fail with one missing element.
    BOOST_CHECK( pl.configure(filename, false) );

    // save some (does not add new props to file:
    BOOST_CHECK( pl.save(filename, false) );

    // TEST FAILS : TO BE FIXED in updateProperties()
    //BOOST_REQUIRE( !pl.configure(filename, true) ); // must still fail, not all were saved.

    // save all:
    BOOST_CHECK( pl.save(filename, true) );
    BOOST_CHECK( pl.configure(filename, true) ); // all were saved.
}

/**
 * Test saving and loading the properties using unkown types
 */
BOOST_AUTO_TEST_CASE( testPropUnknown )
{
    std::string filename = "property_unknown.tst";
    tc.addProperty(pints);

    BOOST_CHECK( pl.save(filename, true) ); // produces empty file.
    BOOST_CHECK( !pl.configure(filename, true) ); // must fail, was not serialized !
    BOOST_CHECK( pl.configure(filename, false) );

    // test unknown in bag:
    tc.properties()->removeProperty( &pints );
    tc.addProperty( "bag", bag ).doc( "bag doc" );
    bag.addProperty(pints);

    BOOST_CHECK( pl.save(filename, true) ); // produces file with bag.

    // TEST FAILS : TO BE FIXED in updateProperties()
    //BOOST_CHECK( !pl.configure(filename, true) ); // must fail, was not serialized !

    BOOST_CHECK( pl.configure(filename, false) );
}

BOOST_AUTO_TEST_CASE( testPropLoading )
{
    std::string filename = "property_loading.cpf";
    BOOST_REQUIRE( pl.load(filename) );

    BOOST_CHECK( tc.provides()->hasProperty("load1") );
    BOOST_CHECK( tc.provides()->hasProperty("load2") );
    BOOST_CHECK( tc.provides()->hasProperty("bag1"));
    BOOST_CHECK( tc.provides()->hasProperty("pint2") );
    BOOST_CHECK( tc.provides()->hasProperty("puint2") );
    BOOST_CHECK( tc.provides()->hasProperty("pllong2") );
    BOOST_CHECK( tc.provides()->hasProperty("pullong2") );

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

    BOOST_CHECK_EQUAL( tc.properties()->getPropertyType<int>("pint2")->value(), -2 );
    BOOST_CHECK_EQUAL( tc.properties()->getPropertyType<unsigned int>("puint2")->value(), 3 );
    BOOST_CHECK_EQUAL( tc.properties()->getPropertyType<long long>("pllong2")->value(), 9223372036854775807ll );
    BOOST_CHECK_EQUAL( tc.properties()->getPropertyType<unsigned long long>("pullong2")->value(), 18446744073709551615ull );
}

BOOST_AUTO_TEST_CASE( testPropStoring )
{
    tc.addProperty(pstring);
    tc.addProperty(pchar);
    tc.addProperty(pdouble);
    tc.addProperty(pdoubles);
    tc.addProperty("newbag", bag).doc("newbag doc"); // takes reference to bag (PropertyBag&)
    bag.addProperty( pdouble );
    bag.addProperty( pdoubles );
    bag.addProperty( pbag ); // takes reference to bpag (PropertyBase&)
    pbag.value().addProperty( pchar );

    std::string filename = "property_storing.tst";
    BOOST_REQUIRE( pl.store(filename) );

    tc.properties()->clear();

    // check by using load:
    BOOST_REQUIRE( pl.load(filename) );

    BOOST_CHECK( tc.provides()->hasProperty("pstring") );
    BOOST_CHECK( tc.provides()->hasProperty("pchar") );
    BOOST_CHECK( tc.provides()->hasProperty("pdouble") );
    BOOST_CHECK( tc.provides()->hasProperty("pdoubles") );
    BOOST_CHECK( tc.provides()->hasProperty("newbag"));
    Property<PropertyBag> bag1 = tc.properties()->getProperty("newbag");
    BOOST_REQUIRE( bag1.ready() );
    BOOST_CHECK( bag1.getDescription() == "newbag doc");
    PropertyBag& bag = bag1.value();
    BOOST_CHECK_EQUAL( bag.size(), 3);
    BOOST_CHECK( bag.find("pdouble"));
    BOOST_CHECK( bag.find("pdoubles"));
    BOOST_CHECK( bag.find("pbag"));
    Property<double> bagdouble = bag.getProperty("pdouble");
    Property<vector<double> > bagvector = bag.getProperty("pdoubles");

    BOOST_REQUIRE(bagdouble.ready());
    BOOST_CHECK_EQUAL( bagdouble.value(), 1.23456);
    BOOST_REQUIRE(bagvector.ready());
    BOOST_REQUIRE_EQUAL(bagvector.value().size(), 3);
    BOOST_CHECK_EQUAL(bagvector.value()[0], 4.123);
    BOOST_CHECK_EQUAL(bagvector.value()[1], 4.123);
    BOOST_CHECK_EQUAL(bagvector.value()[2], 4.123);
}

BOOST_AUTO_TEST_SUITE_END()
