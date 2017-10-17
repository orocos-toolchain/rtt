/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  plugins_test.cpp

                        plugins_test.cpp -  description
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

#include <boost/filesystem.hpp>
#include "TaskContext.hpp"
#include "plugin/Plugin.hpp"
#include "plugin/PluginLoader.hpp"
#include "internal/GlobalService.hpp"

/* For internal use only - check if extension contains a version. */
RTT_API bool isExtensionVersion(const std::string& ext);
/* For internal use only - check if library should be loaded */
RTT_API bool isLoadableLibrary(const boost::filesystem::path& filename);

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

BOOST_AUTO_TEST_CASE( testExtensionVersion )
{
    BOOST_CHECK( true  == isExtensionVersion(".1") );
    BOOST_CHECK( true  == isExtensionVersion(".12") );
    BOOST_CHECK( true  == isExtensionVersion(".123") );

    BOOST_CHECK( false == isExtensionVersion("1") );
    BOOST_CHECK( false == isExtensionVersion("12") );
    BOOST_CHECK( false == isExtensionVersion("123") );
    BOOST_CHECK( false == isExtensionVersion(".a") );
    BOOST_CHECK( false == isExtensionVersion("a") );
    BOOST_CHECK( false == isExtensionVersion(".123 ") );
    BOOST_CHECK( false == isExtensionVersion(". 123") );

    // pathological
    BOOST_CHECK( false == isExtensionVersion("") );
    BOOST_CHECK( false == isExtensionVersion(".") );
    BOOST_CHECK( false == isExtensionVersion(".1.a") );
    BOOST_CHECK( false == isExtensionVersion("..1") );
    BOOST_CHECK( false == isExtensionVersion(".1a2") );
    BOOST_CHECK( false == isExtensionVersion(".1.2") );
    BOOST_CHECK( false == isExtensionVersion(" .1") );
}

BOOST_AUTO_TEST_CASE( testLoadableLibrary )
{
#if     defined(__APPLE__)

    BOOST_CHECK( true  == isLoadableLibrary("libfoo.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.2.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.2.3.dylib") );

    BOOST_CHECK( true  == isLoadableLibrary("libfoo.bar.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.bar.1.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.bar.1.2.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.bar.1.2.3.dylib") );
    BOOST_CHECK( true  == isLoadableLibrary("libfoo.bar.1..dylib") );

    BOOST_CHECK( true  == isLoadableLibrary("libfoo.1.bar.dylib") );
    BOOST_CHECK( true  == isLoadableLibrary("libfoo.1.2.bar.dylib") );
    BOOST_CHECK( true  == isLoadableLibrary("libfoo.1.2.3.bar.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.bar.1.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.bar.1.2.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.bar.1.2.3.dylib") );

    BOOST_CHECK( true  == isLoadableLibrary("libfoo-bar.dylib") );
    BOOST_CHECK( true  == isLoadableLibrary("libfoo-2.3.bar.dylib") );

    // acceptable to RTT as libraries
    BOOST_CHECK( true  == isLoadableLibrary("foo.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("foo.1.dylib") );
    BOOST_CHECK( true  == isLoadableLibrary("foo.1.b.dylib") );
    BOOST_CHECK( true  == isLoadableLibrary("1.dylib") );

    // not libraries
    BOOST_CHECK( false == isLoadableLibrary("libfoo.a") );
    BOOST_CHECK( false == isLoadableLibrary("foo") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.so") );
    BOOST_CHECK( false == isLoadableLibrary("foo.so") );
    BOOST_CHECK( false == isLoadableLibrary("foo.dylib.x") );
    BOOST_CHECK( false == isLoadableLibrary("foo.dll") );

    // pathological
    BOOST_CHECK( false == isLoadableLibrary(".dylib") );

#elif   defined(WIN32)

    BOOST_CHECK( true  == isLoadableLibrary("libfoo.dll") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.dll.1") );

    // not libraries
    BOOST_CHECK( false == isLoadableLibrary("foo") );
    BOOST_CHECK( false == isLoadableLibrary("foo.so") );
    BOOST_CHECK( false == isLoadableLibrary("foo.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("foo.1.dll") );

    // pathological
    BOOST_CHECK( false == isLoadableLibrary(".dll") );
    BOOST_CHECK( false == isLoadableLibrary("1.dll") );

#else
    // assume Linux

    BOOST_CHECK( true  == isLoadableLibrary("libfoo.so") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.so.1") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.so.1.2") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.so.1.2.3") );

    BOOST_CHECK( true  == isLoadableLibrary("libfoo.bar.so") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.bar.so.1") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.bar.so.1.2") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.bar.so.1.2.3") );

    BOOST_CHECK( true  == isLoadableLibrary("libfoo.1.bar.so") );
    BOOST_CHECK( true  == isLoadableLibrary("libfoo.1.2.bar.so") );
    BOOST_CHECK( true  == isLoadableLibrary("libfoo.1.2.3.bar.so") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.bar.so.1") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.bar.so.1.2") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.1.bar.so.1.2.3") );

    BOOST_CHECK( true  == isLoadableLibrary("libfoo-bar.so") );
    BOOST_CHECK( true  == isLoadableLibrary("libfoo-2.3.bar.so") );

    // acceptable to RTT as libraries
    BOOST_CHECK( true  == isLoadableLibrary("foo.so") );
    BOOST_CHECK( true  == isLoadableLibrary("foo.1.so") );

    // not libraries
    BOOST_CHECK( false == isLoadableLibrary("libfoo.a") );
    BOOST_CHECK( false == isLoadableLibrary("foo") );
    BOOST_CHECK( false == isLoadableLibrary("libfoo.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("foo.dylib") );
    BOOST_CHECK( false == isLoadableLibrary("foo.dll") );
    BOOST_CHECK( false == isLoadableLibrary("foo.so.x") );

    // pathological
    BOOST_CHECK( false == isLoadableLibrary(".so") );
    BOOST_CHECK( false == isLoadableLibrary(".so.1") );

#endif

    // pathological
    BOOST_CHECK( false == isLoadableLibrary("") );
    BOOST_CHECK( false == isLoadableLibrary(".") );
    BOOST_CHECK( false == isLoadableLibrary("..") );
    BOOST_CHECK( false == isLoadableLibrary("1.") );
    BOOST_CHECK( false == isLoadableLibrary("1") );
    BOOST_CHECK( false == isLoadableLibrary(".1") );
    BOOST_CHECK( false == isLoadableLibrary("a.") );
    BOOST_CHECK( false == isLoadableLibrary("a") );
    BOOST_CHECK( false == isLoadableLibrary(".a") );
}

/** loads a complete project with a given path
 */
BOOST_AUTO_TEST_CASE( testTypePlugins )
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();

    BOOST_CHECK( pl->isLoaded("typekit_plugin") == false);
    BOOST_CHECK( pl->isLoaded("TypesPluginTest") == false);
    pl->loadTypekit("testproject",".;..");
    BOOST_CHECK( pl->isLoaded("typekit_plugin") );
    BOOST_CHECK( pl->isLoaded("TypesPluginTest") );

}

/** loads a single library/plugin with a given path.
 */
BOOST_AUTO_TEST_CASE( testTypePlugin )
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();

    BOOST_REQUIRE( pl->loadTypekit("typekit_plugin", "testproject;../testproject") );
    BOOST_CHECK( pl->isLoaded("typekit_plugin") );
    BOOST_CHECK( pl->isLoaded("TypesPluginTest") );
}

BOOST_AUTO_TEST_CASE( testServicePlugins )
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();

    BOOST_CHECK( pl->isLoaded("service_plugin") == false);
    BOOST_CHECK( pl->isLoaded("serviceplugin") == false);
    BOOST_CHECK( pl->isLoaded("global_service_plugin") == false);
    BOOST_CHECK( pl->isLoaded("globalserviceplugin") == false);
    pl->loadPlugin("testproject",".;..");
    BOOST_CHECK( pl->isLoaded("service_plugin") );
    BOOST_CHECK( pl->isLoaded("serviceplugin") );
    BOOST_CHECK( pl->isLoaded("global_service_plugin") );
    BOOST_CHECK( pl->isLoaded("globalserviceplugin") );

    BOOST_CHECK( RTT::internal::GlobalService::Instance()->hasService("the_global_service") );
}

BOOST_AUTO_TEST_CASE( testServicePlugin )
{
    PluginLoader::shared_ptr pl = PluginLoader::Instance();

    BOOST_REQUIRE( pl->loadService("serviceplugin", tc) );
    BOOST_CHECK( tc->provides()->hasService("the_service") );

}

BOOST_AUTO_TEST_SUITE_END()

