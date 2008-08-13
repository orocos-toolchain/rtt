/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:50 CET 2005  test-runner.cpp

                        test-runner.cpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include <os/main.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <Logger.hpp>

using namespace RTT;

int ORO_main(int argc, char** argv)
{
    // disable logging of errors or warnings if no ORO_LOGLEVEL was set.
    if ( log().getLogLevel() == Logger::Warning ) {
        log(Info) << "Lowering LogLevel to Critical." << endlog();
        log().setLogLevel(Logger::Critical);
    } else {
        log(Info) << "LogLevel unaltered by test-runner." << endlog();
    }

    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( suite );

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
                                                         std::cerr ) );

    // Run the tests.
    bool wasSucessful = runner.run();

    // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;
}

