/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:28 CEST 2004  Thread.cxx

                        Thread.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "fosi_internal_interface.hpp"
#include "Thread.hpp"
#include "../Time.hpp"
#include "threads.hpp"
#include "../Logger.hpp"
#include "MutexLock.hpp"

#include "../rtt-config.h"
#include "../internal/CatchConfig.hpp"

#ifdef OROPKG_OS_THREAD_SCOPE
# include "../extras/dev/DigitalOutInterface.hpp"
#define SCOPE_INIT(name) 
#define SCOPE_ON   if ( task->d ) task->d->switchOn( bit );
#define SCOPE_OFF  if ( task->d ) task->d->switchOff( bit );
#elif defined(HAVE_LTTNG_UST) && defined(OROPKG_OS_GNULINUX)
#include "gnulinux/traces/lttng_ust.h"
#define SCOPE_INIT(name) tracepoint(orocos_rtt, thread_init , name);
#define SCOPE_ON         tracepoint(orocos_rtt, thread_scope, 1);
#define SCOPE_OFF        tracepoint(orocos_rtt, thread_scope, 0);
#else
#define SCOPE_INIT(name) 
#define SCOPE_ON
#define SCOPE_OFF
#endif

namespace RTT {
    namespace os
    {
        using RTT::Logger;

        unsigned int Thread::default_stack_size = 0;

        double Thread::lock_timeout_no_period_in_s = 1.0;

        double Thread::lock_timeout_period_factor = 10.0;

        void Thread::setStackSize(unsigned int ssize) { default_stack_size = ssize; }

        void Thread::setLockTimeoutNoPeriod(double timeout_in_s) { lock_timeout_no_period_in_s = timeout_in_s; }
       
        void Thread::setLockTimeoutPeriodFactor(double factor) { lock_timeout_period_factor = factor; }

        void *thread_function(void* t)
        {
            /**
             * This is one time initialisation
             */
            Thread* task = static_cast<os::Thread*> (t);
            Logger::In in(task->getName());

            SCOPE_INIT(task->getName())

            task->configure();

            // signal to setup() that we're created.
            rtos_sem_signal(&(task->sem));

            // This lock forces setup(), which holds the lock, to continue.
            { MutexLock lock(task->breaker); }
#ifdef OROPKG_OS_THREAD_SCOPE
            // order thread scope toggle bit on thread number
            unsigned int bit = task->threadNumber();
#endif
            SCOPE_OFF

            int overruns = 0, cur_sched = task->msched_type;
            NANO_TIME cur_period = task->period;

            while (!task->prepareForExit)
            {
                TRY(
                    /**
                     * The real task starts here.
                     */
                    while (1)
                    {
                        if (!task->active || (task->active && task->period == 0) || !task->running )
                        {
                            // consider this the 'configuration or waiting for command state'
                            if (task->period != 0) {
                                overruns = 0;
                                // drop out of periodic mode:
                                rtos_task_set_period(task->getTask(), 0);
                            }
                            rtos_sem_wait(&(task->sem)); // wait for command.
                            task->configure();           // check for reconfigure
                            if (task->prepareForExit)    // check for exit
                            {
                                break; // break while(1) {}
                            }
                            // end of configuration
                        }
                        // This is the running state. Only enter if a task is running
                        if ( task->running )
                        {
                            if (task->period != 0) // periodic
                            {
                                MutexLock lock(task->breaker);
                                while(task->running && !task->prepareForExit )
                                {
                                    TRY
                                    (
                                        SCOPE_ON
                                        task->step(); // one cycle
                                        SCOPE_OFF
                                    )
                                    CATCH_ALL
                                    (
                                        SCOPE_OFF
                                        throw;
                                    )

                                    // Check changes in period
                                    if ( cur_period != task->period) {
                                        // reconfigure period before going to sleep
                                        rtos_task_set_period(task->getTask(), task->period);
                                        cur_period = task->period;
                                        if (cur_period == 0)
                                            break; // break while(task->running) if no longer periodic
                                    }

                                    // Check changes in scheduler
                                    if ( cur_sched != task->msched_type) {
                                        rtos_task_set_scheduler(task->getTask(), task->msched_type);
                                        cur_sched = task->msched_type;
                                    }
                                    // rtos_task_wait_period will return immediately if
                                    // the task is not periodic (ie period == 0)
                                    // return non-zero to indicate overrun.
                                    if (rtos_task_wait_period(task->getTask()) != 0)
                                    {
                                        ++overruns;
                                        if (overruns == task->maxOverRun)
                                            break; // break while(task->running)
                                    }
                                    else if (overruns != 0)
                                        --overruns;
                                } // while(task->running)
                                if (overruns == task->maxOverRun || task->prepareForExit)
                                    break; // break while(1) {}
                            }
                            else // non periodic:
                            TRY
                            (
                                // this mutex guarantees that stop() waits
                                // until loop() returns.
                                MutexLock lock(task->breaker);

                                task->inloop = true;
                                SCOPE_ON
                                task->loop();
                                SCOPE_OFF
                                task->inloop = false;
                            ) CATCH_ALL
                            (
                                SCOPE_OFF
                                task->inloop = false;
                                throw;
                            )
                        }
                    } // while(1)
                    if (overruns == task->maxOverRun)
                    {
                        task->emergencyStop();
                        Logger::In in(rtos_task_get_name(task->getTask()));
                        log(Critical) << rtos_task_get_name(task->getTask())
                                << " got too many periodic overruns in step() ("
                                << overruns << " times), stopped Thread !"
                                << endlog();
                        log()   << " See Thread::setMaxOverrun() for info."
                                << endlog();
                    }
                )CATCH(std::exception const& e,
                    SCOPE_OFF
                    task->emergencyStop();
                    Logger::In in(rtos_task_get_name(task->getTask()));
                    log(Critical) << rtos_task_get_name(task->getTask())
                            << " caught a C++ exception, stopped thread !"
                            << endlog();
                    log(Critical) << "exception was: "
                               << e.what() << endlog();
                ) CATCH_ALL
                (
                    SCOPE_OFF
                    task->emergencyStop();
                    Logger::In in(rtos_task_get_name(task->getTask()));
                    log(Critical) << rtos_task_get_name(task->getTask())
                            << " caught an unknown C++ exception, stopped thread !"
                            << endlog();
                )
            } // while (!prepareForExit)

            return 0;
        }

        void Thread::emergencyStop()
        {
            // set state to not running
            this->running = false;
            this->inloop  = false;
            this->active  = false;
            // execute finalize in current mode, even if hard.
            this->finalize();
        }

        Thread::Thread(int scheduler, int _priority,
                Seconds periods, unsigned cpu_affinity, const std::string & name) :
                    msched_type(scheduler), active(false), prepareForExit(false),
                    inloop(false),running(false),
                    maxOverRun(OROSEM_OS_PERIODIC_THREADS_MAX_OVERRUN),
                    period(Seconds_to_nsecs(periods)) // Do not call setPeriod(), since the semaphores are not yet used !
#ifdef OROPKG_OS_THREAD_SCOPE
        ,d(NULL)
#endif
                    , stopTimeout(0)
        {
            this->setup(_priority, cpu_affinity, name);
        }

        void Thread::setup(int _priority, unsigned cpu_affinity, const std::string& name)
        {
            Logger::In in("Thread");
            int ret;

            // we do this under lock in order to force the thread to wait until we're done.
            MutexLock lock(breaker);

            log(Info) << "Creating Thread for scheduler=" << (msched_type == ORO_SCHED_OTHER ? "ORO_SCHED_OTHER" : "ORO_SCHED_RT")
                      << ", priority=" << _priority
                      << ", CPU affinity=" << cpu_affinity
                      << ", with name='" << name << "'"
                      << endlog();
            ret = rtos_sem_init(&sem, 0);
            if (ret != 0)
            {
                log(Critical)
                        << "Could not allocate configuration semaphore 'sem' for "
                        << name
                        << ". Throwing std::bad_alloc." << endlog();
                rtos_sem_destroy(&sem);
#ifndef ORO_EMBEDDED
                throw std::bad_alloc();
#else
                return;
#endif
            }

#ifdef OROPKG_OS_THREAD_SCOPE
            // Check if threadscope device already exists

            {
                if ( DigitalOutInterface::nameserver.getObject("ThreadScope") )
                {
                    d = DigitalOutInterface::nameserver.getObject("ThreadScope");
                }
                else
                {
                    log(Warning) << "Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << endlog();
                }
            }
#endif
            int rv = rtos_task_create(&rtos_task, _priority, cpu_affinity, name.c_str(),
                    msched_type, default_stack_size, thread_function, this);
            if (rv != 0)
            {
                log(Critical) << "Could not create thread "
                        << name << "."
                        << endlog();
                rtos_sem_destroy(&sem);
#ifndef ORO_EMBEDDED
                throw std::bad_alloc();
#else
                return;
#endif
            }

            // Wait for creation of thread.
            rtos_sem_wait( &sem );

            const char* modname = getName();
            Logger::In in2(modname);
            log(Info) << "Thread created with scheduler type '"
                    << (getScheduler() == ORO_SCHED_OTHER ? "ORO_SCHED_OTHER" : "ORO_SCHED_RT") << "', priority " << getPriority()
                    << ", cpu affinity " << getCpuAffinity()
                    << " and period " << getPeriod() << " (PID= " << getPid() << " )." << endlog();
#ifdef OROPKG_OS_THREAD_SCOPE
            if (d)
            {
                unsigned int bit = threadNumber();
                log(Info) << "ThreadScope :"<< modname <<" toggles bit "<< bit << endlog();
            }
#endif
        }

        Thread::~Thread()
        {
            Logger::In in("~Thread");
            if (this->isRunning())
                this->stop();

            log(Debug) << "Terminating " << this->getName() << endlog();
            terminate();
            log(Debug) << " done" << endlog();
            rtos_sem_destroy(&sem);

        }

        bool Thread::start()
        {
            if ( period == 0)
            {
                // just signal if already active.
                if ( isActive() ) {
#ifndef OROPKG_OS_MACOSX
                    // This is a 'weak' race condition.
                    // it could be that sem becomes zero
                    // after this check. Technically, this means
                    // loop is being executed (preemption) during start().
                    // For most user code, this is sufficient though, as it
                    // can not know the difference between executing loop()
                    // *in* start or *right after* start (the latter is
                    // guaranteed by the API).
                    // @see ActivityInterface::trigger for how trigger uses this
                    // assumption.
                    if ( rtos_sem_value(&sem) > 0 )
                        return true;
#endif
                    rtos_sem_signal(&sem);
                    return true;
                }

                active=true;
                if ( this->initialize() == false || active == false ) {
                    active = false;
                    return false;
                }

                running = true;
                rtos_sem_signal(&sem);

                return true;
            }
            else {

                if ( active )
                    return false;
                active = true;

                bool result;
                result = this->initialize();

                if (result == false || active == false) // detect call to stop() within initialize()
                {
                    active = false;
                    return false;
                }

                running = true;

                // signal start :
                rtos_task_make_periodic(&rtos_task, period);
                int ret = rtos_sem_signal(&sem);
                if (ret != 0)
                    log(Critical)
                    << "Thread::start(): sem_signal returns " << ret
                    << endlog();
                // do not wait, we did our job.

                return true;
            }
        }

        void Thread::setStopTimeout(Seconds value)
        {
            stopTimeout = value;
        }

        Seconds Thread::getStopTimeout() const
        {
            if (stopTimeout != 0)
                return stopTimeout;
            else if (period == 0)
                return Thread::lock_timeout_no_period_in_s;
            else return Thread::lock_timeout_period_factor * getPeriod();
        }

        bool Thread::stop()
        {
            if (!active)
                return false;

            running = false;

            if ( period == 0)
            {
                if ( inloop ) {
                    if ( !this->breakLoop() ) {
                        log(Warning) << "Failed to stop thread " << this->getName() << ": breakLoop() returned false."<<endlog();
                        running = true;
                        return false;
                    }
                    // breakLoop was ok, wait for loop() to return.
                }
                // always take this lock, but after breakLoop was called !
                MutexTimedLock lock(breaker, getStopTimeout()); 
                if ( !lock.isSuccessful() ) {
                    log(Error) << "Failed to stop thread " << this->getName() << ": breakLoop() returned true, but loop() function did not return after " << getStopTimeout() <<" seconds."<<endlog();
                    running = true;
                    return false;
                }
            } else {
                //
                MutexTimedLock lock(breaker, getStopTimeout() ); 
                if ( lock.isSuccessful() ) {
                    // drop out of periodic mode.
                    rtos_task_make_periodic(&rtos_task, 0);
                } else {
                    log(Error) << "Failed to stop thread " << this->getName() << ": step() function did not return after "<< getStopTimeout() <<" seconds."<<endlog();
                    running = true;
                    return false;
                }
            }

            this->finalize();
            active = false;
            return true;
        }

        bool Thread::isRunning() const
        {
            return period == 0 ? inloop : running;
        }

        bool Thread::isActive() const
        {
            return active;
        }

        bool Thread::setScheduler(int sched_type)
        {
            Logger::In in("Thread::setScheduler");
            if (os::CheckScheduler(sched_type) == false)
                return false;
            if (this->getScheduler() == sched_type)
            {
                return true;
            }

            log(Info) << "Setting scheduler type for Thread '"
                      << rtos_task_get_name(&rtos_task) << "' to "
                      << sched_type << endlog();
            rtos_task_set_scheduler(&rtos_task, sched_type); // this may be a no-op, in that case, configure() will pick the change up.
            msched_type = sched_type;
            rtos_sem_signal(&sem);
            return true; // we assume all will go well.
        }

        int Thread::getScheduler() const
        {
            return rtos_task_get_scheduler(&rtos_task);
        }

        void Thread::configure()
        {
            // this function is called from within the thread
            // when we wake up after start()
            // It is intended to check our scheduler, priority,..., and do the in-thread
            // stuff that may be required by the RTOS. For example: RTAI requires that
            // we set the scheduler within the thread itself.

            // reconfigure period
            rtos_task_set_period(&rtos_task, period);

            // reconfigure scheduler.
            if (msched_type != rtos_task_get_scheduler(&rtos_task))
            {
                rtos_task_set_scheduler(&rtos_task, msched_type);
                msched_type = rtos_task_get_scheduler(&rtos_task);
            }
        }

        void Thread::step()
        {
        }

        void Thread::loop()
        {
            this->step();
        }

        bool Thread::breakLoop()
        {
            return false;
        }


        bool Thread::initialize()
        {
            return true;
        }

        void Thread::finalize()
        {
        }

        bool Thread::setPeriod(double s)
        {
            nsecs nsperiod = Seconds_to_nsecs(s);
            return setPeriod(0, nsperiod);
        }

        bool Thread::setPeriod(secs s, nsecs ns)
        {
            nsecs nsperiod = ns + 1000* 1000* 1000* s ;
            if (nsperiod < 0)
                return false;
            // logic to switch from per->nper || nper->per
            if ( (nsperiod == 0 && period != 0) || (nsperiod != 0 && period == 0)) {
                // switch between periodic/non-periodic
                // note for RTAI: the fosi_internal layer must detect if this is called from
                // within rtos_task or outside the thread.
                rtos_task_make_periodic(&rtos_task, nsperiod);
                // jump from non periodic into periodic: first sample.
                if ( period == 0) {
                    period = nsperiod; // avoid race with sem in thread func.
                    rtos_sem_signal(&sem);
                }
            }
            // update rate:
            period = nsperiod;

            return true;
        }

        bool Thread::setPeriod( TIME_SPEC p)
        {
            return this->setPeriod( p.tv_sec, p.tv_nsec );
        }

        void Thread::getPeriod(secs& s, nsecs& ns) const
        {
            s = secs(period/(1000*1000*1000));
            ns = period - s*1000*1000*1000;
        }

        bool Thread::setPriority(int p)
        {
            return rtos_task_set_priority(&rtos_task, p) == 0;
        }

        bool Thread::isPeriodic() const
        {
            return period != 0;
        }

        int Thread::getPriority() const
        {
            return rtos_task_get_priority(&rtos_task);
        }

        double Thread::getPeriod() const
        {
            return nsecs_to_Seconds(period);
        }

        nsecs Thread::getPeriodNS() const
        {
            return period;
        }

        bool Thread::setCpuAffinity(unsigned cpu_affinity)
        {
            return rtos_task_set_cpu_affinity(&rtos_task, cpu_affinity) == 0;
        }

        unsigned Thread::getCpuAffinity() const
        {
            return rtos_task_get_cpu_affinity(&rtos_task);
        }

        unsigned int Thread::getPid() const
        {
        	return rtos_task_get_pid(&rtos_task);
        }

        void Thread::yield()
        {
            rtos_task_yield( &rtos_task );
        }

        void Thread::terminate()
        {
            // avoid callling twice.
            if (prepareForExit) return;

            prepareForExit = true;
            rtos_sem_signal(&sem);

            rtos_task_delete(&rtos_task); // this must join the thread.
        }

        const char* Thread::getName() const
        {
            return rtos_task_get_name(&rtos_task);
        }

        void Thread::setMaxOverrun( int m )
        {
            maxOverRun = m;
        }

        int Thread::getMaxOverrun() const
        {
            return maxOverRun;
        }

        void Thread::setWaitPeriodPolicy(int p)
        {
            rtos_task_set_wait_period_policy(&rtos_task, p);  
        }

    }
}

