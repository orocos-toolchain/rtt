#include "FileDescriptorActivity.hpp"
#include "ExecutionEngine.hpp"
#include "TaskCore.hpp"

#include <sys/select.h>
#include <unistd.h>
#include <boost/cstdint.hpp>

using namespace RTT;
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
FileDescriptorActivity::FileDescriptorActivity(int priority, RunnableInterface* _r )
    : NonPeriodicActivity(priority, _r)
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
FileDescriptorActivity::FileDescriptorActivity(int scheduler, int priority, RunnableInterface* _r )
    : NonPeriodicActivity(scheduler, priority, _r)
    , m_running(false)
    , m_timeout(0)
    , runner(_r)
{
    FD_ZERO(&m_fd_set);
}

/**
 * Create a FileDescriptorActivity with a given priority, name and
 * RunnableInterface instance.
 * @param priority The priority of the underlying thread.
 * @param name The name of the underlying thread.
 * @param _r The optional runner, if none, this->loop() is called.
 */
FileDescriptorActivity::FileDescriptorActivity(int priority, const std::string& name, RunnableInterface* _r )
    : NonPeriodicActivity(priority, name, _r)
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
{ return NonPeriodicActivity::isRunning() && m_running; }
int FileDescriptorActivity::getTimeout() const
{ return m_timeout; }
void FileDescriptorActivity::setTimeout(int timeout)
{ m_timeout = timeout; }
void FileDescriptorActivity::watch(int fd)
{
    if (fd < 0)
    {
        log(Error) << "negative file descriptor given to FileDescriptorActivity::watch" << endlog();
        return;
    }

    m_watched_fds.insert(fd);
    FD_SET(fd, &m_fd_set);
}
void FileDescriptorActivity::unwatch(int fd)
{
    m_watched_fds.erase(fd);
    FD_CLR(fd, &m_fd_set);
}
bool FileDescriptorActivity::isUpdated(int fd) const
{ return FD_ISSET(fd, &m_fd_work); }
bool FileDescriptorActivity::hasError() const
{ return m_has_error; }
bool FileDescriptorActivity::hasTimeout() const
{ return m_has_timeout; }
bool FileDescriptorActivity::isWatched(int fd) const
{ return FD_ISSET(fd, &m_fd_set); }

bool FileDescriptorActivity::start()
{
    // Check that there is FDs set ...
    if (m_watched_fds.empty())
        return false;

    if (pipe(m_interrupt_pipe) == -1)
        return false;

    if (!NonPeriodicActivity::start())
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

        m_has_error   = false;
        m_has_timeout = false;
        if (ret == -1)
        {
            log(Error) << "FileDescriptorActivity: error in select(), errno = " << errno << endlog();
            m_has_error = true;
        }
        else if (ret == 0)
            m_has_timeout = true;

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

    // OS::SingleThread properly waits for loop() to return
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
    // NonPeriodicActivity::stop will call breakLoop and act upon its return
    // value.
    if (NonPeriodicActivity::stop())
    {
        close(m_interrupt_pipe[0]);
        close(m_interrupt_pipe[1]);
        return true;
    }
    else
        return false;
}

