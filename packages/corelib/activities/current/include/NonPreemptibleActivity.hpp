/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  NonPreemptibleActivity.hpp 

                        NonPreemptibleActivity.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef TASKNONPREEMPTIBLE_HPP
#define TASKNONPREEMPTIBLE_HPP

#include "PeriodicActivity.hpp"

namespace RTT
{
    /**
     * @brief A NonPreemptibleActivity is a PeriodicActivity which will never be preempted by
     * another PeriodicActivity.
     *
     * Typical NonPreemptibleActivity instances are
     * fast calculations with fast device input and output communication.
     * 
     * They will be run in the ZeroTimeThread.
     */
    class NonPreemptibleActivity
        : public PeriodicActivity
    {
    public:
        /**
         * Construct a NonPreemptibleActivity with a periodicity \a period
         * Seconds
         */
        NonPreemptibleActivity(Seconds period, RunnableInterface* r=0 );

        /**
         * Construct a NonPreemptibleActivity with a periodicity of
         * \a s seconds and \a ns nanoseconds
         */
        NonPreemptibleActivity(secs s, nsecs ns, RunnableInterface* r=0 );

        /**
         * Destroys this instance. The Activity is stopped.
         */
        virtual  ~NonPreemptibleActivity();
                                 
    };

}

#endif
