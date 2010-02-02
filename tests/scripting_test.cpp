
#include "operations_fixture.hpp"
#include <scripting/Scripting.hpp>
#include <scripting/ScriptingAccess.hpp>

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  ScriptingTestSuite,  OperationsFixture )

BOOST_AUTO_TEST_CASE(TestGetService)
{
    ScriptingAccess* sa = new ScriptingAccess( tc ); // done by plugin.

    boost::shared_ptr<Scripting> sc = tc->getService<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    bool r;
    r = sc->loadProgramText("program Foo { do methods.assert(true); set ret = 10.0; }");
    BOOST_REQUIRE( r );
    BOOST_CHECK( sc->hasProgram("Foo") );
    BOOST_REQUIRE( sc->startProgram("Foo") );
    BOOST_CHECK( sc->isProgramRunning("Foo") );
    tc->getActivity()->trigger();
    usleep(100);
    BOOST_CHECK( sc->isProgramRunning("Foo") == false );
    BOOST_CHECK( sc->inProgramError("Foo") == false );
    BOOST_CHECK( ret == 10.0 );

}


BOOST_AUTO_TEST_SUITE_END()
