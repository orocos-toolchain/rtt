
#include "unit.hpp"

#include "TaskContext.hpp"
#include "plugin/PluginLoader.hpp"


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


/**
 * Fixture.
 */
class PluginsFixture
{
public:
    TaskContext* tc;
public:
    PluginsFixture()
    {
        tc =  new TaskContext( "root", TaskContext::Stopped );
    }

    ~PluginsFixture()
    {
        delete tc;
    }
};

using namespace plugin;

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  PluginsSuite,  PluginsFixture )


BOOST_AUTO_TEST_CASE( testTypePlugins )
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();

    pl->loadTypekits(".");
    BOOST_CHECK( pl->isLoaded("typekit_plugin") );
    BOOST_CHECK( pl->isLoaded("TypesPluginTest") );

}

BOOST_AUTO_TEST_CASE( testTypePlugin )
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();

    BOOST_REQUIRE( pl->loadTypekit("typekit_plugin", ".") );
    BOOST_CHECK( pl->isLoaded("typekit_plugin") );
    BOOST_CHECK( pl->isLoaded("TypesPluginTest") );
}

BOOST_AUTO_TEST_CASE( testServicePlugins )
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();

    pl->loadPlugins(".");
    BOOST_CHECK( pl->isLoaded("service_plugin") );
    BOOST_CHECK( pl->isLoaded("serviceplugin") );

}

BOOST_AUTO_TEST_CASE( testServicePlugin )
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();

    BOOST_REQUIRE( pl->loadService("serviceplugin", tc) );
    BOOST_CHECK( tc->provides()->hasService("the_service") );

}

BOOST_AUTO_TEST_SUITE_END()

