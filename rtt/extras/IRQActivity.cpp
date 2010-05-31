/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  IRQActivity.cpp

                        IRQActivity.cpp -  description
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


#include "IRQActivity.hpp"
#include "../ExecutionEngine.hpp"
#include "../base/TaskCore.hpp"
#include "../Logger.hpp"

#include <iostream>

using namespace RTT;
using namespace extras;
using namespace base;

IRQActivity::IRQActivity(int priority, RunnableInterface* _r, const std::string& name )
    : Activity(priority, 0.0, _r, name)
    , m_irq(-1)
    , m_runner(_r) {}

IRQActivity::IRQActivity(int scheduler, int priority, RunnableInterface* _r, const std::string& name )
    : Activity(scheduler, priority, 0.0, _r, name)
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

#include <sys/select.h>
#include <unistd.h>

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

    if (! Activity::start())
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

