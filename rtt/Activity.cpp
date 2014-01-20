/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  Activity.cpp

                        Activity.cpp -  description
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



#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "Time.hpp"
#include "Activity.hpp"
#include "os/MutexLock.hpp"
#include "os/traces.h"
#include "Logger.hpp"
#include "rtt-fwd.hpp"

#include <cmath>

namespace RTT
{
    using namespace detail;

    Activity::Activity(RunnableInterface* _r, const std::string& name )
        : ActivityInterface(_r), os::Thread(ORO_SCHED_OTHER, RTT::os::LowestPriority, 0.0, 0, name )
    {
    }

    Activity::Activity(int priority, RunnableInterface* r, const std::string& name )
        : ActivityInterface(r), os::Thread(ORO_SCHED_RT, priority, 0.0, 0, name )
    {
    }

    Activity::Activity(int priority, Seconds period, RunnableInterface* r, const std::string& name )
        : ActivityInterface(r), os::Thread(ORO_SCHED_RT, priority, period, 0, name )
    {
    }

    Activity::Activity(int scheduler, int priority, RunnableInterface* r, const std::string& name )
        : ActivityInterface(r), os::Thread(scheduler, priority, 0.0, 0, name )
    {
    }

    Activity::Activity(int scheduler, int priority, Seconds period, RunnableInterface* r, const std::string& name )
        : ActivityInterface(r), os::Thread(scheduler, priority, period, 0, name )
    {
    }

    Activity::Activity(int scheduler, int priority, Seconds period, unsigned cpu_affinity, RunnableInterface* r, const std::string& name )
    : ActivityInterface(r), os::Thread(scheduler, priority, period, cpu_affinity, name )
    {
    }

    Activity::~Activity()
    {
        stop();
    }

    os::ThreadInterface* Activity::thread() {
        return this;
    }

    bool Activity::initialize() {
        if ( runner )
            return runner->initialize();
        return true;
    }

    void Activity::step() {
        if ( runner )
            runner->step();
    }

    void Activity::loop() {
        if ( runner )
            runner->loop();
        else
            this->step();
    }

    bool Activity::breakLoop() {
        if ( runner )
            return runner->breakLoop();
        return false;
    }

    void Activity::finalize() {
        if ( runner )
            runner->finalize();
    }


    bool Activity::start() {
        return Thread::start();
    }

    bool Activity::stop() {
        return Thread::stop();
    }

    bool Activity::trigger() {
        tracepoint(orocos_rtt, Activity_trigger, getName());
        return Thread::isActive() ? Thread::start() : false;
    }

    bool Activity::execute() {
        return false;
    }

    bool Activity::isRunning() const {
        return Thread::isRunning();
    }

    bool Activity::isActive() const {
        return Thread::isActive();
    }

    Seconds Activity::getPeriod() const
    {
        return Thread::getPeriod();
    }

    bool Activity::setPeriod(Seconds period)
    {
        return Thread::setPeriod(period);
    }


    bool Activity::isPeriodic() const {
        return Thread::isPeriodic();
    }

    unsigned Activity::getCpuAffinity() const
    {
        return Thread::getCpuAffinity();
    }

    bool Activity::setCpuAffinity(unsigned cpu)
    {
        return Thread::setCpuAffinity(cpu);
    }

}
