/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  RunnableInterface.hpp 

                        RunnableInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
 



#ifndef ORO_OS_RUNNABLEINTERFACE_HPP
#define ORO_OS_RUNNABLEINTERFACE_HPP

namespace ORO_OS
{

    /**
     * @brief A class for running a certain piece of code in a thread.
     *
     * It defines three methods for executing functionality. It allows
     * to execute code in a PeriodicThreadInterface or SingleThreadInterface, which define the
     * control methods as starting and stopping (periodic) threads.
     * 
     * In a start-run-stop cycle, before step() is called the first time,
     * initialize() will be called.
     * When step() is ran the last time in this cycle, finalize() will be 
     * called, after it finishes.
     */

    class RunnableInterface
    {
        public:
            virtual ~RunnableInterface()
            {}

            /**
             * The method that will be called once each time before the periodical
             * execution of step() is started.
             */
            virtual bool initialize() = 0;

            /**
             * The method that will be periodically executed when this
             * class is run.
             */
            virtual void step() = 0;

            /**
             * The method that will be called once each time after the periodical
             * execution of step() is stopped.
             */
            virtual void finalize() = 0;
    };

}

#endif
