/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  startstop.h 

                        startstop.h -  description
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
 * @file startstop.h
 * Prototype definitions of common os init/exit calls.
 */

#include "rtt/rtt-config.h"
#include <exception>

/**
 * Call this function from within main() before
 * using the RTT services. You do not need this
 * when you use ORO_main().
 */
RTT_API int __os_init(int argc, char** argv );

/**
 * Call this function from within main() after
 * using the RTT services, just before returning. 
 * You do not need this when you use ORO_main().
 */
RTT_API void __os_exit(void);

/**
 * Internal: Inform the user that a failure occured.
 * @see ORO_main
 */
RTT_API void __os_printFailure(const char* prog);

/**
 * Internal: Inform the user that a failure occured.
 * @see ORO_main
 */
RTT_API void __os_printException(const char* prog, std::exception& arg);

/**
 * Internal: Check if the user requests disabling
 * exception catching at run-time.
 * @see ORO_main
 */
RTT_API int __os_checkException(int&, char**);

/**
 * Call this function to find out how many arguments were
 * passed to the process' main() function.
 */
RTT_API int __os_main_argc(void);

/**
 * Call this function to get the list of arguments of this
 * process' main() function.
 */
RTT_API char** __os_main_argv(void);
