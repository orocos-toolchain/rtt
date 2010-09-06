/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  marshalling_test.cpp

                        marshalling_test.cpp -  description
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



#include "operations_fixture.hpp"
#include <marsh/Marshalling.hpp>
#include <marsh/MarshallingService.hpp>

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MarshallingTestSuite,  OperationsFixture )

BOOST_AUTO_TEST_CASE(TestGetProvider)
{
    //MarshallingService* sa = new MarshallingService( tc ); // done by TC or plugin

    boost::shared_ptr<Marshalling> sc = tc->getProvider<Marshalling>("marshalling");
    BOOST_REQUIRE( sc );
    BOOST_CHECK( sc->ready() );
}


BOOST_AUTO_TEST_SUITE_END()
