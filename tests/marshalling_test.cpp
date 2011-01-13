/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  marshalling_test.cpp

                        marshalling_test.cpp -  demarshription
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

#include "operations_fixture.hpp"
#include <marsh/Marshalling.hpp>
#include <marsh/MarshallingService.hpp>
#include "plugin/PluginLoader.hpp"
#include "datasource_fixture.hpp"

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

#include <boost/shared_ptr.hpp>

struct MarshallingFixture : public OperationsFixture
{
    boost::shared_ptr<Marshalling> marsh;
    MarshallingFixture() {
        PluginLoader::shared_ptr pl = PluginLoader::Instance();
        pl->loadTypekits("testtypes");
        marsh = tc->getProvider<Marshalling>("marshalling");
    }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MarshallingTestSuite,  MarshallingFixture )

// Tests getProvider for marshalling service
BOOST_AUTO_TEST_CASE(TestGetProvider)
{
    //MarshallingService* sa = new MarshallingService( tc ); // done by TC or plugin

    boost::shared_ptr<Marshalling> marsh2 = tc->getProvider<Marshalling>("marshalling");
    BOOST_REQUIRE( marsh );
    BOOST_REQUIRE( marsh2 );
    BOOST_CHECK( marsh->ready() );
    BOOST_CHECK( marsh2->ready() );
}

// Tests marshalling a matrix type (sequence of sequence)
BOOST_AUTO_TEST_CASE(TestMarshallMatrix)
{
    typedef std::vector<std::vector<double> > MatrixType;
    typedef std::vector<double> RowType;
    MatrixType mx( 5, RowType(5,5.0)); // matrix 5x5 with values '5.0'
    MatrixType mxz( 5, RowType(5,0.0)); // matrix 5x5 with values '0.0'
    MatrixType mxrz; // real empty matrix

    // check initial values:
    BOOST_CHECK_EQUAL( mx[0][0], 5.0);
    BOOST_CHECK_EQUAL( mx[4][4], 5.0);
    BOOST_CHECK_EQUAL( mx[0][2], 5.0);
    BOOST_CHECK_EQUAL( mx[2][0], 5.0);
    BOOST_CHECK_EQUAL( mx[3][3], 5.0);

    tc->addProperty("mx", mx);
    // write a non-existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallMatrix.cpf") );
    // zero out our copy:
    mx = mxrz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallMatrix.cpf") );
    BOOST_REQUIRE_EQUAL( mx.size(), 5);
    BOOST_REQUIRE_EQUAL( mx[0].size(), 5);
    // check restored result:
    BOOST_CHECK_EQUAL( mx[0][0], 5.0);
    BOOST_CHECK_EQUAL( mx[4][4], 5.0);
    BOOST_CHECK_EQUAL( mx[0][2], 5.0);
    BOOST_CHECK_EQUAL( mx[2][0], 5.0);
    BOOST_CHECK_EQUAL( mx[3][3], 5.0);

    // write it again to an existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallMatrix.cpf") );
    // zero out our copy:
    mx = mxz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallMatrix.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( mx[0][0], 5.0);
    BOOST_CHECK_EQUAL( mx[4][4], 5.0);
    BOOST_CHECK_EQUAL( mx[0][2], 5.0);
    BOOST_CHECK_EQUAL( mx[2][0], 5.0);
    BOOST_CHECK_EQUAL( mx[3][3], 5.0);
}

// Tests marshalling a struct type
BOOST_AUTO_TEST_CASE(TestMarshallStructAType)
{
    // filled in type and zero type:
    AType at, atref;
    AType atz; atz.clear();

    tc->addProperty("at", at);
    // write a non-existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallAType.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallAType.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);

    // write it again to an existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallAType.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallAType.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);
}

// Tests marshalling a struct type
BOOST_AUTO_TEST_CASE(TestMarshallStructBType)
{
    // filled in type and zero type:
    BType at, atref;
    BType atz; atz.clear();

    tc->addProperty("at", at);
    // write a non-existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallBType.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallBType.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);

    // write it again to an existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallBType.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallBType.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);
}

// Tests marshalling a struct type
BOOST_AUTO_TEST_CASE(TestMarshallStructCType)
{
    // filled in type and zero type:
    CType at, atref;
    CType atz; atz.clear();

    tc->addProperty("at", at);
    // write a non-existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallCType.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallCType.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);

    // write it again to an existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallCType.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallCType.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);
}

// Tests marshalling a struct type
BOOST_AUTO_TEST_CASE(TestMarshallStructATypes)
{
    // filled in type and zero type:
    ATypes at, atref;
    at.resize(5); atref.resize(5);
    ATypes atz;

    tc->addProperty("at", at);
    // write a non-existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallATypes.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallATypes.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);

    // write it again to an existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallATypes.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallATypes.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);
}

// Tests marshalling a struct type
BOOST_AUTO_TEST_CASE(TestMarshallStructBTypes)
{
    // filled in type and zero type:
    BTypes at, atref;
    at.resize(5); atref.resize(5);
    BTypes atz; atz.clear();

    tc->addProperty("at", at);
    // write a non-existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallBTypes.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallBTypes.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);

    // write it again to an existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallBTypes.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallBTypes.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);
}

// Tests marshalling a struct type
BOOST_AUTO_TEST_CASE(TestMarshallStructCTypes)
{
    // filled in type and zero type:
    CTypes at, atref;
    at.resize(5); atref.resize(5);
    CTypes atz; atz.clear();

    tc->addProperty("at", at);
    // write a non-existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallCTypes.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallCTypes.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);

    // write it again to an existing file:
    BOOST_CHECK( marsh->writeProperties("TestMarshallCTypes.cpf") );
    // zero out our copy:
    at = atz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallCTypes.cpf") );
    // check restored result:
    BOOST_CHECK_EQUAL( at, atref);
}

BOOST_AUTO_TEST_SUITE_END()
