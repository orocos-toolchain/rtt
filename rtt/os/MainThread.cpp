/***************************************************************************
  tag: FMTC  do nov 2 13:06:03 CET 2006  MainThread.cpp

                        MainThread.cpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

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



// this file must be included always first.
#include "os/fosi_internal_interface.hpp"
#include "os/MainThread.hpp"

namespace RTT
{ namespace os {

    boost::shared_ptr<ThreadInterface> MainThread::mt;


    MainThread::MainThread() {
        rtos_task_create_main(&main_task);
    }

    MainThread::~MainThread() {
        rtos_task_delete_main(&main_task);
    }

    ThreadInterface* MainThread::Instance()
    {
        if (mt)
            return mt.get();
        mt.reset( new MainThread() );
        return mt.get();
    }

    void MainThread::Release()
    {
        mt.reset();
    }

    bool MainThread::start()  { return false; }

    bool MainThread::stop()  { return false; }

    Seconds MainThread::getPeriod() const { return 0; }

    nsecs MainThread::getPeriodNS() const { return 0; }

    bool MainThread::isRunning() const { return true; }
    bool MainThread::isActive() const { return true; }

    const char* MainThread::getName() const { return "main"; }

    void MainThread::setName(const char* name) { }

    RTOS_TASK * MainThread::getTask() { return &main_task; }

    const RTOS_TASK * MainThread::getTask() const { return &main_task; }

    bool MainThread::setScheduler(int sched_type)
    {
        if ( rtos_task_set_scheduler(&main_task, sched_type) == 0)
            return true;
        return false;
    }
    int MainThread::getScheduler() const
    {
        return rtos_task_get_scheduler(&main_task);
    }

    bool MainThread::setPriority(int p)
    {
        return rtos_task_set_priority(&main_task, p) == 0;
    }

    int MainThread::getPriority() const
    {
        return rtos_task_get_priority(&main_task);
    }

    unsigned int MainThread::getPid() const
    {
    	return rtos_task_get_pid(&main_task);
    }

    bool MainThread::setPeriod(Seconds period)
    {
        return false;
    }

        void MainThread::setMaxOverrun( int m )
        {
        }

        int MainThread::getMaxOverrun() const
        {
            return 0;
        }

    void MainThread::setWaitPeriodPolicy(int p)
    {
        rtos_task_set_wait_period_policy(&main_task, p);
    }

    void MainThread::yield()
    {
        rtos_task_yield(&main_task);
    }

    unsigned int MainThread::threadNumber() const { return 0; }
}}


