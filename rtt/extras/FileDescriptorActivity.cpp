/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  FileDescriptorActivity.cpp

                        FileDescriptorActivity.cpp -  description
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


#include "FileDescriptorActivity.hpp"
#include "../ExecutionEngine.hpp"
#include "../base/TaskCore.hpp"
#include "../Logger.hpp"


#include <algorithm>

#ifdef WIN32
  #include <io.h>
  #include <fcntl.h>
  #define pipe(X) _pipe((X), 1024, _O_BINARY)
  #define close _close
  #define write _write
  #undef max

#else
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#endif

#include <boost/cstdint.hpp>

using namespace RTT;
using namespace extras;
using namespace base;
const char FileDescriptorActivity::CMD_ANY_COMMAND;

/**
 * Create a FileDescriptorActivity with a given priority and RunnableInterface
 * instance. The default scheduler for NonPeriodicActivity
 * objects is ORO_SCHED_RT.
 *
 * @param priority The priority of the underlying thread.
 * @param _r The optional runner, if none, this->loop() is called.
 */
FileDescriptorActivity::FileDescriptorActivity(int priority, RunnableInterface* _r, const std::string& name )
    : Activity(priority, 0.0, _r, name)
    , m_running(false)
    , m_timeout_us(0)
    , m_period(0)
    , m_has_error(false)
    , m_has_timeout(false)
    , m_break_loop(false)
    , m_trigger(false)
    , m_update_sets(false)
{
    FD_ZERO(&m_fd_set);
    FD_ZERO(&m_fd_work);
    m_interrupt_pipe[0] = m_interrupt_pipe[1] = -1;
}

/**
 * Create a FileDescriptorActivity with a given scheduler type, priority and
 * RunnableInterface instance.
 * @param scheduler
 *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
 *        ORO_SCHED_RT.
 * @param priority The priority of the underlying thread.
 * @param _r The optional runner, if none, this->loop() is called.
 */
FileDescriptorActivity::FileDescriptorActivity(int scheduler, int priority, RunnableInterface* _r, const std::string& name )
    : Activity(scheduler, priority, 0.0, _r, name)
    , m_running(false)
    , m_timeout_us(0)
    , m_period(0)
    , m_has_error(false)
    , m_has_timeout(false)
    , m_break_loop(false)
    , m_trigger(false)
    , m_update_sets(false)
{
    FD_ZERO(&m_fd_set);
    FD_ZERO(&m_fd_work);
    m_interrupt_pipe[0] = m_interrupt_pipe[1] = -1;
}

FileDescriptorActivity::FileDescriptorActivity(int scheduler, int priority, Seconds period, RunnableInterface* _r, const std::string& name )
    : Activity(scheduler, priority, 0.0, _r, name)	// actual period == 0.0
    , m_running(false)
    , m_timeout_us(0)
    , m_period(period >= 0.0 ? period : 0.0)        // intended period
    , m_has_error(false)
    , m_has_timeout(false)
    , m_break_loop(false)
    , m_trigger(false)
    , m_update_sets(false)
{
    FD_ZERO(&m_fd_set);
    FD_ZERO(&m_fd_work);
    m_interrupt_pipe[0] = m_interrupt_pipe[1] = -1;
}

FileDescriptorActivity::FileDescriptorActivity(int scheduler, int priority, Seconds period, unsigned cpu_affinity, RunnableInterface* _r, const std::string& name )
    : Activity(scheduler, priority, 0.0, cpu_affinity, _r, name)	// actual period == 0.0
    , m_running(false)
    , m_timeout_us(0)
    , m_period(period >= 0.0 ? period : 0.0)        // intended period
    , m_has_error(false)
    , m_has_timeout(false)
    , m_break_loop(false)
    , m_trigger(false)
    , m_update_sets(false)
{
    FD_ZERO(&m_fd_set);
    FD_ZERO(&m_fd_work);
    m_interrupt_pipe[0] = m_interrupt_pipe[1] = -1;
}

FileDescriptorActivity::~FileDescriptorActivity()
{
    stop();
}

Seconds FileDescriptorActivity::getPeriod() const
{ return m_period; }

bool FileDescriptorActivity::setPeriod(Seconds p)
{
	if (p < 0)
        return false;
	m_period = p;
	return true;
}

bool FileDescriptorActivity::isRunning() const
{ return Activity::isRunning() && m_running; }
int FileDescriptorActivity::getTimeout() const
{ return m_timeout_us / 1000; }
int FileDescriptorActivity::getTimeout_us() const
{ return m_timeout_us; }
void FileDescriptorActivity::setTimeout(int timeout)
{
	setTimeout_us(timeout * 1000);
}
void FileDescriptorActivity::setTimeout_us(int timeout_us)
{
	if (0 <= timeout_us)
	{
		m_timeout_us = timeout_us;
	}
	else
	{
        log(Error) << "Ignoring invalid timeout (" << timeout_us << ")" << endlog();
    }
}
void FileDescriptorActivity::watch(int fd)
{ RTT::os::MutexLock lock(m_lock);
    if (fd < 0)
    {
        log(Error) << "negative file descriptor given to FileDescriptorActivity::watch" << endlog();
        return;
    }

    m_watched_fds.insert(fd);
    FD_SET(fd, &m_fd_set);
    triggerUpdateSets();
}
void FileDescriptorActivity::unwatch(int fd)
{ RTT::os::MutexLock lock(m_lock);
    m_watched_fds.erase(fd);
    FD_CLR(fd, &m_fd_set);
    triggerUpdateSets();
}
void FileDescriptorActivity::clearAllWatches()
{ RTT::os::MutexLock lock(m_lock);
    m_watched_fds.clear();
    FD_ZERO(&m_fd_set);
    triggerUpdateSets();
}
void FileDescriptorActivity::triggerUpdateSets()
{
    { RTT::os::MutexLock lock(m_command_mutex);
        m_update_sets = true;
    }
    int unused; (void)unused;
    unused = write(m_interrupt_pipe[1], &CMD_ANY_COMMAND, 1);
}
bool FileDescriptorActivity::isUpdated(int fd) const
{ return FD_ISSET(fd, &m_fd_work); }
bool FileDescriptorActivity::hasError() const
{ return m_has_error; }
bool FileDescriptorActivity::hasTimeout() const
{ return m_has_timeout; }
bool FileDescriptorActivity::isWatched(int fd) const
{ RTT::os::MutexLock lock(m_lock);
    return FD_ISSET(fd, &m_fd_set); }

bool FileDescriptorActivity::start()
{
    if ( isActive() )
        return false;

    if (pipe(m_interrupt_pipe) == -1)
    {
        log(Error) << "FileDescriptorActivity: cannot create control pipe" << endlog();
        return false;
    }

#ifndef WIN32
    // set m_interrupt_pipe to non-blocking
    int flags = 0;
    if ((flags = fcntl(m_interrupt_pipe[0], F_GETFL, 0)) == -1 ||
        fcntl(m_interrupt_pipe[0], F_SETFL, flags | O_NONBLOCK) == -1 ||
        (flags = fcntl(m_interrupt_pipe[1], F_GETFL, 0)) == -1 ||
        fcntl(m_interrupt_pipe[1], F_SETFL, flags | O_NONBLOCK) == -1)
    {
        close(m_interrupt_pipe[0]);
        close(m_interrupt_pipe[1]);
        m_interrupt_pipe[0] = m_interrupt_pipe[1] = -1;
        log(Error) << "FileDescriptorActivity: could not set the control pipe to non-blocking mode" << endlog();
        return false;
    }
#endif

    // reset flags
    m_break_loop = false;
    m_trigger = false;
    m_update_sets = false;

    if (!Activity::start())
    {
        close(m_interrupt_pipe[0]);
        close(m_interrupt_pipe[1]);
        m_interrupt_pipe[0] = m_interrupt_pipe[1] = -1;
        log(Error) << "FileDescriptorActivity: Activity::start() failed" << endlog();
        return false;
    }
    return true;
}

bool FileDescriptorActivity::trigger()
{ 
    if (isActive() ) {
        { RTT::os::MutexLock lock(m_command_mutex);
            m_trigger = true;
        }
        int unused; (void)unused;
        unused = write(m_interrupt_pipe[1], &CMD_ANY_COMMAND, 1);
        return true;
    } else
        return false;
}

bool FileDescriptorActivity::timeout()
{
    return false;
}


struct fd_watch {
    int& fd;
    fd_watch(int& fd) : fd(fd) {}
    ~fd_watch()
    {
        if (fd != -1) 
            close(fd);
        fd = -1;
    }
};

void FileDescriptorActivity::loop()
{
    int pipe = m_interrupt_pipe[0];
    fd_watch watch_pipe_0(m_interrupt_pipe[0]);
    fd_watch watch_pipe_1(m_interrupt_pipe[1]);

    while(true)
    {
        int max_fd;
        { RTT::os::MutexLock lock(m_lock);
            if (m_watched_fds.empty())
                max_fd = pipe;
            else
                max_fd = std::max(pipe, *m_watched_fds.rbegin());

            m_fd_work = m_fd_set;
        }
        FD_SET(pipe, &m_fd_work);

        int ret;
        m_running = false;
        if (m_timeout_us == 0)
        {
            ret = select(max_fd + 1, &m_fd_work, NULL, NULL, NULL);
        }
        else
        {
			static const int USECS_PER_SEC = 1000000;
            timeval timeout = { m_timeout_us / USECS_PER_SEC,
                                m_timeout_us % USECS_PER_SEC};
            ret = select(max_fd + 1, &m_fd_work, NULL, NULL, &timeout);
        }

        m_has_error   = false;
        m_has_timeout = false;
        if (ret == -1)
        {
            log(Error) << "FileDescriptorActivity: error in select(), errno = " << errno << endlog();
            m_has_error = true;
        }
        else if (ret == 0)
        {
//            log(Error) << "FileDescriptorActivity: timeout in select()" << endlog();
            m_has_timeout = true;
        }

        // Empty all commands queued in the pipe
        if (ret > 0 && FD_ISSET(pipe, &m_fd_work)) // breakLoop or trigger requests
        {
            // These variables are used in order to loop with select(). See the
            // while() condition below.
            fd_set watch_pipe;
            timeval timeout;
            char dummy;
            do
            {
                int unused; (void)unused;
                unused = read(pipe, &dummy, 1);

                // Initialize the values for the next select() call
                FD_ZERO(&watch_pipe);
                FD_SET(pipe, &watch_pipe);
                timeout.tv_sec  = 0;
                timeout.tv_usec = 0;
            }
            while(select(pipe + 1, &watch_pipe, NULL, NULL, &timeout) > 0);
        }

        // We check the flags after the command queue was emptied as we could miss commands otherwise:
        bool do_trigger = true;
        bool user_trigger = false;
        { RTT::os::MutexLock lock(m_command_mutex);
            // This section should be really fast to not block threads calling trigger(), breakLoop() or watch().
            if (m_trigger) {
                do_trigger = true;
                user_trigger = true;
                m_trigger = false;
            }
            if (m_update_sets) {
                m_update_sets = false;
                do_trigger = false;
            }
            if (m_break_loop) {
                m_break_loop = false;
                break;
            }
        }

        if (do_trigger)
        {
            try
            {
                m_running = true;
                step();
                if (m_has_timeout)
                    work(RunnableInterface::TimeOut);
                else if ( user_trigger )
                    work(RunnableInterface::Trigger);
                else
                    work(RunnableInterface::IOReady);
                m_running = false;
            }
            catch(...)
            {
                m_running = false;
                throw;
            }
        }
    }
}

bool FileDescriptorActivity::breakLoop()
{
    { RTT::os::MutexLock lock(m_command_mutex);
        m_break_loop = true;
    }
    int unused; (void)unused;
    unused = write(m_interrupt_pipe[1], &CMD_ANY_COMMAND, 1);
    return true;
}

void FileDescriptorActivity::step()
{
    m_running = true;
    if (runner != 0)
        runner->step();
    m_running = false;
}

void FileDescriptorActivity::work(base::RunnableInterface::WorkReason reason) {
    m_running = true;
    if (runner != 0)
        runner->work(reason);
    m_running = false;

}

bool FileDescriptorActivity::stop()
{
    // If fatal() is called from the updateHook(), stop() will be called from
    // within the context and loop() will still run after this command has quit.
    //
    // This is bad and will have to be fixed in RTT 2.0 by having delayed stops
    // (i.e. setting the task context's state to FATAL only when loop() has
    // quit)
    if ( Activity::stop() == true )
    {
        fd_watch watch_pipe_0(m_interrupt_pipe[0]);
        fd_watch watch_pipe_1(m_interrupt_pipe[1]);
        return true;
    }
    return false;
}

