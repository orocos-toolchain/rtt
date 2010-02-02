
#include "operations_fixture.hpp"
#include <marsh/Marshalling.hpp>
#include <marsh/MarshallingAccess.hpp>

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
    //MarshallingAccess* sa = new MarshallingAccess( tc ); // done by TC or plugin

    boost::shared_ptr<Marshalling> sc = tc->getProvider<Marshalling>("marshalling");
    BOOST_REQUIRE( sc );
    BOOST_CHECK( sc->ready() );
}


BOOST_AUTO_TEST_SUITE_END()
