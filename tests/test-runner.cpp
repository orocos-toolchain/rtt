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
#include <Logger.hpp>

#include <boost/test/included/unit_test.hpp>

using boost::unit_test::test_suite;

using namespace RTT;

struct InitOrocos {
public:
	InitOrocos(){  }
	~InitOrocos(){ 
#ifndef OROCOS_TARGET_XENOMAI
        __os_exit(); 
#endif
}

};

BOOST_GLOBAL_FIXTURE( InitOrocos )

boost::unit_test::test_suite* init_unit_test_suite(int argc, char** argv)
{
	__os_init(argc, argv);

    // disable logging of errors or warnings if no ORO_LOGLEVEL was set.
    if ( log().getLogLevel() == Logger::Warning ) {
        log(Info) << "Lowering LogLevel to Critical." << endlog();
        log().setLogLevel(Logger::Critical);
    } else {
        log(Info) << "LogLevel unaltered by test-runner." << endlog();
    }

    return 0;
}

