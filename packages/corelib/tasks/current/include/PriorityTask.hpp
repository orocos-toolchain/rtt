/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:54 CEST 2004  PriorityTask.hpp 

                        PriorityTask.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef PRIORITYTASK_HPP
#define PRIORITYTASK_HPP

#include "PriorityThread.hpp"
#include "RealTimeTask.hpp"

namespace ORO_CoreLib
{
    /**
     * A PriorityTask is a RealTimeTask which will never be preempted by
     * another RealTimeTask. Typical PriorityTask instances are
     * fast calculations with fast device input and output communication.
     * 
     * They can only be run in the PriorityThread<Priority>.
     */
    template< int Priority >
    class PriorityTask
        : public RealTimeTask
    {
    public:
        /**
         * Construct a PriorityTask with a periodicity <period>
         * Seconds
         */
        PriorityTask(Seconds period, RunnableInterface* r=0 )
            :RealTimeTask(period, r)
        {
            pt = PriorityThread<Priority>::Instance();
        }

        /**
         * Construct a PriorityTask with a periodicity of
         * <s> seconds and <ns> nanoseconds
         */
        PriorityTask(secs s, nsecs ns, RunnableInterface* r=0 )
            :RealTimeTask(s, ns, r)
        {
            pt = PriorityThread<Priority>::Instance();
        }

        /**
         * Destroys this instance. The Task is stopped.
         */
        virtual  ~PriorityTask()
        {
            stop();
        }
                                 
    protected:
        virtual bool taskAdd()
        {
            return pt->taskAdd(this, per_ns);
        }

        virtual void taskRemove()
        {
            pt->taskRemove(this);
        }

        /**
         * The PriorityThread<Priority> used for scheduling this task
         */
        PriorityThread<Priority>* pt;
    };

}

#endif
