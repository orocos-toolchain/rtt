#include "IRQActivity.hpp"
#include "ExecutionEngine.hpp"
#include "TaskCore.hpp"

#include <sys/select.h>
#include <unistd.h>
#include <iostream>

using namespace RTT;

IRQActivity::IRQActivity(int priority, RunnableInterface* _r )
    : NonPeriodicActivity(priority, _r)
    , m_irq(-1)
    , m_runner(_r) {}

IRQActivity::IRQActivity(int scheduler, int priority, RunnableInterface* _r )
    : NonPeriodicActivity(scheduler, priority, _r)
    , m_irq(-1)
    , m_runner(_r) {}

IRQActivity::IRQActivity(int priority, const std::string& name, RunnableInterface* _r )
    : NonPeriodicActivity(priority, name, _r)
    , m_irq(-1)
    , m_runner(_r) {}

IRQActivity::~IRQActivity()
{
    stop();
}

int  IRQActivity::getIRQ() const { return m_irq; }
void IRQActivity::setIRQ(int irq) { m_irq = irq; }

#ifndef OROPKG_OS_XENOMAI
bool IRQActivity::start() {
    Logger::log() << Logger::Error << "IRQActivity is only usable on Xenomai" << Logger::endl;
    return false; 
}
#else
bool IRQActivity::start()
{
    if (m_irq == -1)
    { // no FD explicitely set. Try to find one.
        ExecutionEngine* engine = dynamic_cast<ExecutionEngine*>(m_runner);
        if (engine)
        {
            Provider* provider = dynamic_cast<Provider*>(engine->getParent());
            if (provider)
                m_irq = provider->getIRQ();
        }
    }

    if (m_irq == -1)
    {
        Logger::log() << Logger::Error << "no IRQ set for IRQActivity" << Logger::endl;
        return false;
    }

    char name[20];
    if (snprintf(name, 20, "IRQActivity%d", m_irq) >= 20)
    {
        Logger::log() << Logger::Error << "something is wrong with m_irq. Are you trying to do a buffer overflow ?" << strerror(errno) << Logger::endl;
        return false;
    }

    int ret = rt_intr_create(&m_handle, name, m_irq, 0);
    if (ret != 0)
    {
        Logger::log() << Logger::Error << "cannot create interrupt object for IRQ " << m_irq << ": " << strerror(-ret) << Logger::endl;
        return false;
    }

    if (! NonPeriodicActivity::start())
    {
        rt_intr_delete(&m_handle);
        return false;
    }

    return true;
}

void IRQActivity::loop()
{
    if (m_irq == -1)
        return;

    while(true)
    {
        if (rt_intr_wait(&m_handle, TM_INFINITE) > 0)
            step();
        else
            break;
        // {
        //     sleep(1);
        //     if (errno == -EIDRM) // interrupt handler has been deleted
        //         break;
        //     else
        //     {
        //         std::cout << "ERROR " << errno << std::endl;
        //         break;
        //     }
        // }
        // step();
    }
}

bool IRQActivity::breakLoop()
{
    rt_intr_delete(&m_handle);
    return true;
}

void IRQActivity::step()
{
    if (m_runner != 0)
        m_runner->step();
}

#endif // OS is xenomai

