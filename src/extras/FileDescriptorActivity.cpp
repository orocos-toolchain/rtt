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

#include <sys/select.h>
#include <unistd.h>
#include <boost/cstdint.hpp>

using namespace RTT;
using namespace extras;
using namespace base;
const int FileDescriptorActivity::CMD_BREAK_LOOP;
const int FileDescriptorActivity::CMD_TRIGGER;


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
    , m_timeout(0)
    , runner(_r)
{
    FD_ZERO(&m_fd_set);
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
    , m_timeout(0)
    , runner(_r)
{
    FD_ZERO(&m_fd_set);
}

FileDescriptorActivity::~FileDescriptorActivity()
{
    stop();
}

bool FileDescriptorActivity::isRunning() const
{ return Activity::isRunning() && m_running; }
int FileDescriptorActivity::getTimeout() const
{ return m_timeout; }
void FileDescriptorActivity::setTimeout(int timeout)
{ m_timeout = timeout; }
void FileDescriptorActivity::watch(int fd)
{
    m_watched_fds.insert(fd);
    FD_SET(fd, &m_fd_set);
}
void FileDescriptorActivity::unwatch(int fd)
{
    m_watched_fds.erase(fd);
    FD_CLR(fd, &m_fd_set);
}
bool FileDescriptorActivity::isUpdated(int fd)
{ return FD_ISSET(fd, &m_fd_work); }
bool FileDescriptorActivity::hasError() const
{ return m_error; }
bool FileDescriptorActivity::isWatched(int fd)
{ return FD_ISSET(fd, &m_fd_set); }

bool FileDescriptorActivity::start()
{
    // Check that there is FDs set ...
    if (m_watched_fds.empty())
        return false;

    if (pipe(m_interrupt_pipe) == -1)
        return false;

    if (!Activity::start())
    {
        close(m_interrupt_pipe[0]);
        close(m_interrupt_pipe[1]);
        return false;
    }
    return true;
}

bool FileDescriptorActivity::trigger()
{ return write(m_interrupt_pipe[1], &CMD_TRIGGER, 1) == 1; }

void FileDescriptorActivity::loop()
{
    int pipe = m_interrupt_pipe[0];
    int max_fd = std::max(pipe, *m_watched_fds.rbegin());

    while(true)
    {
        m_fd_work = m_fd_set;
        FD_SET(pipe, &m_fd_work);
        
        int ret;
        m_running = false;
        if (m_timeout == 0)
            ret = select(max_fd + 1, &m_fd_work, NULL, NULL, NULL);
        else
        {
            timeval timeout = { m_timeout / 1000, (m_timeout % 1000) * 1000 };
            ret = select(max_fd + 1, &m_fd_work, NULL, NULL, &timeout);
        }

        if (ret == -1)
        {
            if (errno != EBADF)
            {
                log(Error) << "internal error in FileDescriptorActivity: got errno = " << errno << endlog();
                break;
            }
            else
                m_error = true;
        }
        else
            m_error = false;

        if (ret > 0 && FD_ISSET(pipe, &m_fd_work)) // breakLoop or trigger requests
        { // Empty all commands queued in the pipe

            // These variables are used in order to loop with select(). See the
            // while() condition below.
            fd_set watch_pipe;
            timeval timeout;

            bool do_break = false, do_trigger = false;
            do
            {
                boost::uint8_t code;
                read(pipe, &code, 1);
                if (code == CMD_BREAK_LOOP)
                    do_break = true;
                else
                    do_trigger = true;

                // Initialize the values for the next select() call
                FD_ZERO(&watch_pipe);
                FD_SET(pipe, &watch_pipe);
                timeout.tv_sec  = 0;
                timeout.tv_usec = 0;
            }
            while(select(pipe + 1, &watch_pipe, NULL, NULL, &timeout) > 0);

            if (do_break)
                break;
        }

        try
        {
            m_running = true;
            step();
            m_running = false;
        }
        catch(...)
        {
            m_running = false;
            throw;
        }
    }
}

bool FileDescriptorActivity::breakLoop()
{
    if (write(m_interrupt_pipe[1], &CMD_BREAK_LOOP, 1) != 1)
        return false;

    // os::SingleThread properly waits for loop() to return
    return true;
}

void FileDescriptorActivity::step()
{
    m_running = true;
    if (runner != 0)
        runner->step();
    m_running = false;
}

bool FileDescriptorActivity::stop()
{
    // Activity::stop will call breakLoop and act upon its return
    // value.
    if (Activity::stop())
    {
        close(m_interrupt_pipe[0]);
        close(m_interrupt_pipe[1]);
        return true;
    }
    else
        return false;
}

