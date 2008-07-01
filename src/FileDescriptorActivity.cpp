#include "FileDescriptorActivity.hpp"
#include "ExecutionEngine.hpp"
#include "TaskCore.hpp"

#include <sys/select.h>
#include <unistd.h>
#include <iostream>

using namespace RTT;

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
    , m_fd(-1), m_close_on_stop(false)
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
    , m_fd(-1), m_close_on_stop(false)
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
    , m_fd(-1), m_close_on_stop(false)
    , runner(_r) {}

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
                m_fd = fd_provider->getFileDescriptor();
                m_close_on_stop = false;
            }
            
        }
    }

    if (m_fd == -1)
        return false;
    if (pipe(m_interrupt_pipe) == -1)
        return false;

    return NonPeriodicActivity::start();
}

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

        int ret = select(max + 1, &set, NULL, NULL, NULL);
        if (ret > 0)
        {
            if (FD_ISSET(fd, &set)) // data is available
                step();
            if (FD_ISSET(pipe, &set)) // breakLoop request
            {
                sleep(1);
                std::cerr << "BREAKING" << std::endl;
                break;
            }
        }
        else if (ret == -1)
        {
            const int error = errno;
            if (error != EINTR)
            {
                stop();
                break;
            }
        }
    }
    close(pipe);
}

bool FileDescriptorActivity::breakLoop()
{
    if (write(m_interrupt_pipe[1], "1", 1) != 1)
        return false;

    while(true)
    {
        // wait for the read side to be closed
        int pipe = m_interrupt_pipe[0];
        fd_set set;
        FD_ZERO(&set);
        FD_SET(pipe, &set);
        if (select(pipe + 1, &set, NULL, NULL, NULL) == EBADF)
            return true;
    }
    // never reached
}

void FileDescriptorActivity::step()
{
    if (runner != 0)
        runner->step();
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

        close(m_interrupt_pipe[1]); // read side closed by breakLoop 
        return true;
    }
    else
        return false;
}

