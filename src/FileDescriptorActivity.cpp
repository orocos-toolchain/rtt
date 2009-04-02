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
    , m_fd(-1), m_timeout(0), m_close_on_stop(false)
    , runner(_r) {}

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
    , m_fd(-1), m_timeout(0), m_close_on_stop(false)
    , runner(_r) {}

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
    , m_fd(-1), m_timeout(0), m_close_on_stop(false)
    , runner(_r) {}

FileDescriptorActivity::~FileDescriptorActivity()
{
    stop();
}

bool FileDescriptorActivity::isRunning() const
{ return NonPeriodicActivity::isRunning() && m_running; }

int FileDescriptorActivity::getTimeout() const { return m_timeout; }
void FileDescriptorActivity::setTimeout(int timeout)
{
    m_timeout = timeout;
}
int FileDescriptorActivity::getFileDescriptor() const { return m_fd; }
void FileDescriptorActivity::setFileDescriptor(int fd, bool close_on_stop)
{
    m_fd = fd;
    m_close_on_stop = close_on_stop;
}

bool FileDescriptorActivity::start()
{
    if (m_fd == -1)
    { // no FD explicitely set. Try to find one.
        ExecutionEngine* engine = dynamic_cast<ExecutionEngine*>(runner);
        if (engine)
        {
            Provider* fd_provider = dynamic_cast<Provider*>(engine->getParent());
            if (fd_provider)
            {
                m_fd      = fd_provider->getFileDescriptor();
                m_timeout = fd_provider->getTimeout();
                m_close_on_stop = false;
            }
            
        }
    }

    if (m_fd == -1)
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
    if (m_fd == -1)
        return;

    int fd   = m_fd;
    int pipe = m_interrupt_pipe[0];
    int max  = fd > pipe ? fd : pipe;

    while(true)
    {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        FD_SET(pipe, &set);

        
        int ret;
        m_running = false;
        if (m_timeout == 0)
            ret = select(max + 1, &set, NULL, NULL, NULL);
        else
        {
            timeval timeout = { m_timeout / 1000, (m_timeout % 1000) * 1000 };
            ret = select(max + 1, &set, NULL, NULL, &timeout);
        }

        // If true, step() will be called at the end of the loop. The other
        // option would be to put the management of the m_running flag in our
        // implementation of step(), but that would make the subclassing of
        // FileDescriptorActivity harder.
        bool do_step = false;
        if (ret > 0 && FD_ISSET(pipe, &set)) // breakLoop request
        {
            boost::uint8_t code;
            read(pipe, &code, 1);
            if (code == CMD_BREAK_LOOP)
                break;
            else if (code == CMD_TRIGGER)
                do_step = true;
        }
        if (ret == -1 || ret == 0 || FD_ISSET(fd, &set)) // data is available
            do_step = true;

        if (do_step)
        {
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
        if (m_fd != -1 && m_close_on_stop)
        {
            close(m_fd);
            m_fd = -1;
        }

        close(m_interrupt_pipe[0]);
        close(m_interrupt_pipe[1]);
        return true;
    }
    else
        return false;
}

