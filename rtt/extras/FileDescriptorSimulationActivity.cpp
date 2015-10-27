/***************************************************************************

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

#include "FileDescriptorSimulationActivity.hpp"
#include "os/MainThread.hpp"

using namespace RTT;
using namespace extras;

FileDescriptorSimulationActivity::FileDescriptorSimulationActivity(int priority,
                                                                   base::RunnableInterface* _r,
                                                                   const std::string& name) :
    base::ActivityInterface(_r),
    period(0),
    running(false),
    lastReason(base::RunnableInterface::TimeOut)
{
    // prevent compiler warnings
    (void)priority;
    (void)name;
}

FileDescriptorSimulationActivity::FileDescriptorSimulationActivity(int scheduler,
                                                                   int priority,
                                                                   base::RunnableInterface* _r,
                                                                   const std::string& name) :
    base::ActivityInterface(_r),
    period(0),
    running(false),
    lastReason(base::RunnableInterface::TimeOut)
{
    // prevent compiler warnings
    (void)priority;
    (void)name;
}

FileDescriptorSimulationActivity::FileDescriptorSimulationActivity(int scheduler,
                                                                   int priority,
                                                                   Seconds _p,
                                                                   base::RunnableInterface* _r,
                                                                   const std::string& name) :
    base::ActivityInterface(_r),
    period(_p >= 0.0 ? _p : 0.0),
    running(false),
    lastReason(base::RunnableInterface::TimeOut)
{
    // prevent compiler warnings
    (void)scheduler;
    (void)priority;
    (void)name;
}

FileDescriptorSimulationActivity::FileDescriptorSimulationActivity(int scheduler,
                                                                   int priority,
                                                                   Seconds _p,
                                                                   unsigned cpu_affinity,
                                                                   base::RunnableInterface* _r,
                                                                   const std::string& name) :
    base::ActivityInterface(_r),
    period(_p >= 0.0 ? _p : 0.0),
    running(false),
    lastReason(base::RunnableInterface::TimeOut)
{
    // prevent compiler warnings
    (void)scheduler;
    (void)priority;
    (void)cpu_affinity;
    (void)name;
}

FileDescriptorSimulationActivity::~FileDescriptorSimulationActivity()
{
    stop();
}

/***************************************************************************
 * FDAInterface functions
 ***************************************************************************/

void FileDescriptorSimulationActivity::watch(int fd)
{
    (void)fd;           // prevent compiler warning
}

void FileDescriptorSimulationActivity::unwatch(int fd)
{
    (void)fd;           // prevent compiler warning
}

void FileDescriptorSimulationActivity::clearAllWatches()
{
}

bool FileDescriptorSimulationActivity::isWatched(int fd) const
{
    (void)fd;           // prevent compiler warning
    return false;
}

bool FileDescriptorSimulationActivity::isUpdated(int fd) const
{
    return (base::RunnableInterface::IOReady == lastReason);
}

bool FileDescriptorSimulationActivity::hasTimeout() const
{
    return (base::RunnableInterface::TimeOut == lastReason);
}

bool FileDescriptorSimulationActivity::hasError() const
{
    return false;
}

void FileDescriptorSimulationActivity::setTimeout(int timeout)
{
    (void)timeout;       // prevent compiler warning
}

void FileDescriptorSimulationActivity::setTimeout_us(int timeout_us)
{
    (void)timeout_us;    // prevent compiler warning
}

int FileDescriptorSimulationActivity::getTimeout() const
{
    return 0;
}

int FileDescriptorSimulationActivity::getTimeout_us() const
{
    return 0;
}

/***************************************************************************
 * ActivityInterface functions
 ***************************************************************************/


bool FileDescriptorSimulationActivity::start()
{
    if (running) {
        return false;
    }
    running = true;
    return true;
}

bool FileDescriptorSimulationActivity::stop()
{
    if (!running) {
        return false;
    }
    running = false;
    return true;
}

bool FileDescriptorSimulationActivity::isRunning() const
{
    return running;
}

bool FileDescriptorSimulationActivity::isActive() const
{
    return running;
}

Seconds FileDescriptorSimulationActivity::getPeriod() const
{
    return period;
}

bool FileDescriptorSimulationActivity::isPeriodic() const
{
    return (0 != period);
}

bool FileDescriptorSimulationActivity::setPeriod(Seconds s)
{
	if (s < 0) {
        return false;
    }
    period = s;
    return true;
}

unsigned FileDescriptorSimulationActivity::getCpuAffinity() const
{
    return 0;
}

bool FileDescriptorSimulationActivity::setCpuAffinity(unsigned cpu)
{
    (void)cpu;      // prevent compiler warning
    return true;
}

bool FileDescriptorSimulationActivity::execute()
{
    return true;
}

bool FileDescriptorSimulationActivity::trigger()
{
    return true;
}

bool FileDescriptorSimulationActivity::timeout()
{
    return true;
}

os::ThreadInterface* FileDescriptorSimulationActivity::thread()
{
    return os::MainThread::Instance();
}

void FileDescriptorSimulationActivity::work(base::RunnableInterface::WorkReason reason)
{
    if (0 != runner) {
        runner->work(reason);
        lastReason = reason;
    }
}
