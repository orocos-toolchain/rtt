/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:36 CEST 2002  ZeroTimeThread.h 

                       ZeroLatencyThread.h -  description
                          -------------------
   begin                : Wed April 17 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ZEROLATENCYTHREAD_HPP
#define ZEROLATENCYTHREAD_HPP

#include "TimerThread.hpp"


namespace ORO_CoreLib
{

    class TaskPreemptible;
    /**
     * This thread is the preemptible realtime thread in the
     * Orocos system. It is assumed to perform its work over a 
     * low (zero) latency timespan.
     * It Uses the Singleton pattern, since there will be only one.
     *
     * @see PeriodicThread, ZeroTimeThread
     */
    class ZeroLatencyThread
        : public TimerThread
    {
    public:
        static ZeroLatencyThread* Instance();

        /**
         * Releases the ZeroLatencyThread
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release();

        /**
         * Destructor
         */
        virtual ~ZeroLatencyThread();

    protected:

        /**
         * Constructor
         */
        ZeroLatencyThread();

    private:

        /**
         * Our only instance of the ZeroLatencyThread
         */
        static ZeroLatencyThread* _instance;

    };
} // namespace ORO_CoreLib

#endif
