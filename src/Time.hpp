/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  Time.hpp

                        Time.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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

#ifndef ORO_CORELIB_TIME_HPP
#define ORO_CORELIB_TIME_HPP

#include <cmath>

/****************************************
*	 Implementation of rint for MSVC	*
****************************************/
#include "os/rint.h"

/**
 * @defgroup CoreLibTime Time measurement classes.
 * Time measurement and time keeping.
 */

namespace RTT
{
	/**
     * Seconds are stored as a double precision float.
     */
    typedef double Seconds;
    /**
     * seconds as a signed long.
     */
    typedef long secs;
    /**
     * milliseconds as a signed long.
     */
    typedef long msecs;
    /**
     * microseconds as a signed long.
     */
    typedef long usecs;
    /**
     * nanoseconds as a signed long long.
     */
    typedef long long nsecs;
    /**
     * picoseconds as a signed long long
     */
    typedef long long psecs;

    const long MSECS_IN_SECS = 1000;
    const long USECS_IN_SECS = 1000 * MSECS_IN_SECS;
    const long NSECS_IN_SECS = 1000 * USECS_IN_SECS;
    const long long PSECS_IN_SECS = 1000LL * NSECS_IN_SECS;

    const long USECS_IN_MSECS = 1000;
    const long NSECS_IN_MSECS = 1000 * USECS_IN_MSECS;
    const long PSECS_IN_MSECS = 1000 * NSECS_IN_MSECS;

    const long NSECS_IN_USECS = 1000;
    const long PSECS_IN_USECS = 1000 * NSECS_IN_USECS;

    const long PSECS_IN_NSECS = 1000;

    inline msecs secs_to_msecs(const secs s) { return s * MSECS_IN_SECS; }
    inline usecs secs_to_usecs(const secs s) { return s * USECS_IN_SECS; }
    inline nsecs secs_to_nsecs(const secs s) { return s * NSECS_IN_SECS; }
    inline psecs secs_to_psecs(const secs s) { return s * PSECS_IN_SECS; }

    inline usecs msecs_to_usecs(const msecs ms) { return ms * USECS_IN_MSECS; }
    inline nsecs msecs_to_nsecs(const msecs ms) { return ms * NSECS_IN_MSECS; }
    inline psecs msecs_to_psecs(const msecs ms) { return ms * PSECS_IN_MSECS; }

    inline nsecs usecs_to_nsecs(const usecs us) { return us * NSECS_IN_USECS; }
    inline psecs usecs_to_psecs(const usecs us) { return us * PSECS_IN_USECS; }

    inline psecs nsecs_to_psecs(const nsecs ns) { return ns * PSECS_IN_NSECS; }

    inline nsecs Seconds_to_nsecs(const Seconds s) { return nsecs( rint( s * secs_to_nsecs(1) ) ); }
    inline Seconds nsecs_to_Seconds(const nsecs ns) { return Seconds( ns ) / Seconds(NSECS_IN_SECS); }
    inline psecs Seconds_to_psecs(const Seconds s) { return psecs( rint( s * secs_to_psecs(1) ) ); }
    inline Seconds psecs_to_Seconds(const psecs ps) { return Seconds( ps ) / Seconds(PSECS_IN_SECS); }

}

#endif
