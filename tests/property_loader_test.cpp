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
    LoaderTest() : tc("tc"),
            pstring("pstring","pstringd","Hello World"),
            pchar("pchar","pchard",'H'),
            pdouble("pdouble", "pdoubled", 1.23456),
            pbag("pbag","pbagd"),
            pints("pints", "pintsd", vector<int>(3,4)),
            pdoubles("pdoubles", "pdoublesd", vector<double>(3,4.123))
    {

    }
    PropertyLoader pl;
    TaskContext tc;
    Property<string> pstring;
    Property<char> pchar;
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
    tc.addProperty(pdouble);
    tc.addProperty(pdoubles);
    tc.addProperty("newbag", bag).doc("newbag doc"); // takes reference to bag (PropertyBag&)
    bag.addProperty( pdouble );
    bag.addProperty( pdoubles );
    bag.addProperty( pbag ); // takes reference to bpag (PropertyBase&)
    pbag.value().addProperty( pchar );

    // save all to fresh file
    BOOST_CHECK( pl.save(filename, &tc, true) );
    // configure all from file.
    BOOST_CHECK( pl.configure(filename, &tc, true) );

    // configure all fails with one missing element.
    bag.addProperty( pstring );

    // TEST FAILS : TO BE FIXED in updateProperties()
    //BOOST_REQUIRE( !pl.configure(filename, &tc, true) );

    // configure some does not fail with one missing element.
    BOOST_CHECK( pl.configure(filename, &tc, false) );

    // save some (does not add new props to file:
    BOOST_CHECK( pl.save(filename, &tc, false) );

    // TEST FAILS : TO BE FIXED in updateProperties()
    //BOOST_REQUIRE( !pl.configure(filename, &tc, true) ); // must still fail, not all were saved.

    // save all:
    BOOST_CHECK( pl.save(filename, &tc, true) );
    BOOST_CHECK( pl.configure(filename, &tc, true) ); // all were saved.
}

/**
 * Test saving and loading the properties using unkown types
 */
BOOST_AUTO_TEST_CASE( testPropUnknown )
{
    std::string filename = "property_unknown.tst";
    tc.addProperty(pints);

    BOOST_CHECK( pl.save(filename, &tc, true) ); // produces empty file.
    BOOST_CHECK( !pl.configure(filename, &tc, true) ); // must fail, was not serialized !
    BOOST_CHECK( pl.configure(filename, &tc, false) );

    // test unknown in bag:
    tc.properties()->removeProperty( &pints );
    tc.addProperty( "bag", bag ).doc( "bag doc" );
    bag.addProperty(pints);

    BOOST_CHECK( pl.save(filename, &tc, true) ); // produces file with bag.

    // TEST FAILS : TO BE FIXED in updateProperties()
    //BOOST_CHECK( !pl.configure(filename, &tc, true) ); // must fail, was not serialized !

    BOOST_CHECK( pl.configure(filename, &tc, false) );
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
    BOOST_REQUIRE( pl.store(filename, &tc) );

    tc.properties()->clear();

    // check by using load:
    BOOST_REQUIRE( pl.load(filename, &tc) );

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
