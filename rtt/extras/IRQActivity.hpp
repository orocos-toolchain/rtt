/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  IRQActivity.hpp

                        IRQActivity.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#ifndef IRQ_ACTIVITY_HPP
#define IRQ_ACTIVITY_HPP

#include "../Activity.hpp"
#include "../rtt-config.h"

#ifdef OROPKG_OS_XENOMAI
#include <native/intr.h>
#else
// Provide a dump RT_INTR type. start() always returns false on non-Xenomai
// OSes.
struct RT_INTR { };
#ifdef _MSC_VER
	#pragma message( "IRQActivity is available only on Xenomai. It will fail on startup")
#else
#warning "IRQActivity is available only on Xenomai. It will fail on startup"
#endif
#endif

namespace RTT { namespace extras {

    /** An activity which is triggered by an IRQ edge. This activity is only
     * available on xenomai
     *
     * The IRQ line can be given by two means:
     * <ul>
     *   <li> setIRQ is called
     *   <li> one of the underlying task contexts (if any) provides the
     *   Provider interface. In that case, the getIRQ() method of
     *   that task context is called to get the descriptor
     * </ul>
     */
    class RTT_API IRQActivity : public Activity
    {
        int      m_irq;
        RT_INTR  m_handle;
        base::RunnableInterface* m_runner;

    public:
        /** If this interface is derived from one of the TaskContext objects
         * which are executed by this activity, then the activity can
         * auto-configure itself (i.e. it can get the file descriptor from
         * that task context). Otherwise, setIRQ must be called
         * explicitely
         */
        struct Provider {
            virtual int getIRQ() const = 0;
        };

        /**
         * Create a IRQActivity with a given priority and base::RunnableInterface
         * instance. The default scheduler for IRQActivity
         * objects is ORO_SCHED_RT.
         *
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        IRQActivity(int priority, base::RunnableInterface* _r = 0, const std::string& name ="IRQActivity" );

        /**
         * Create a IRQActivity with a given scheduler type, priority and
         * base::RunnableInterface instance.
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        IRQActivity(int scheduler, int priority, base::RunnableInterface* _r = 0, const std::string& name ="IRQActivity" );

        virtual ~IRQActivity();

        int  getIRQ() const;
        void setIRQ(int irq);

        virtual bool start();
#ifdef OROPKG_OS_XENOMAI
        virtual void loop();
        virtual bool breakLoop();
        virtual void step();
#endif
    };
}}

#endif
