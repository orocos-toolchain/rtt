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

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

#include <boost/shared_ptr.hpp>

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MarshallingTestSuite,  OperationsFixture )

// Tests getProvider for marshalling service
BOOST_AUTO_TEST_CASE(TestGetProvider)
{
    //MarshallingService* sa = new MarshallingService( tc ); // done by TC or plugin

    boost::shared_ptr<Marshalling> marsh = tc->getProvider<Marshalling>("marshalling");
    BOOST_REQUIRE( marsh );
    BOOST_CHECK( marsh->ready() );
}

// Tests marshalling a matrix type (sequence of sequence)
BOOST_AUTO_TEST_CASE(TestMarshallMatrix)
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();
    pl->loadTypekits("testtypes");

    boost::shared_ptr<Marshalling> marsh = tc->getProvider<Marshalling>("marshalling");

    typedef std::vector<std::vector<double> > MatrixType;
    typedef std::vector<double> RowType;
    MatrixType mx( 5, RowType(5,5.0)); // matrix 5x5 with values '5.0'
    MatrixType mxz( 5, RowType(5,0.0)); // matrix 5x5 with values '0.0'

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
    mx = mxz;
    BOOST_REQUIRE( marsh->readProperties("TestMarshallMatrix.cpf") );
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

BOOST_AUTO_TEST_SUITE_END()
