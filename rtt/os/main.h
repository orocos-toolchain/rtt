/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  main.h 

                        main.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

/**
 * @file main.h
 * The prototype of the application's ORO_main
 *
 */

#include "rtt/os/startstop.h"
#include "rtt/Logger.hpp"

/**
 * Forward declare this wrapper around the user code.
 * @see ORO_main
 */
int ORO_main_impl(int,char**);

/**
 * Declare the function 'int ORO_main(int argc, char **argv)' instead
 * of 'int main(int argc, char **argv)' such that the Real-Time
 * Toolkit can setup the OS environment, prior to executing user
 * code. Using ORO_main is optional, but in some RTOS'es, you'll be
 * required to call __os_init() and __os_exit() instead when not using
 * ORO_main.
 *
 * @see __os_init(), __os_exit()
 */
#define ORO_main                                                \
    main( int argc, char **argv) {                              \
        int res = -1;                                           \
        std::string location(argv[0]); location += "::main()";  \
        __os_init(argc, argv); {                                \
            RTT::Logger::In in(location.c_str());               \
            if ( __os_checkException(argc,argv) ) {             \
                try {                                           \
                    res = ORO_main_impl(argc, argv);            \
                } catch( std::exception& arg )                  \
                {                                               \
                    __os_printException(argv[0], arg);          \
                } catch( ... )                                  \
                {                                               \
                    __os_printFailure(argv[0]);                 \
                }                                               \
            } else {                                            \
                res = ORO_main_impl(argc, argv);                \
            }                                                   \
        } __os_exit();                                          \
        return res;                                             \
    }                                                           \
int ORO_main_impl

