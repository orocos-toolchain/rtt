/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  NonPeriodicActivity.cxx 

                        NonPeriodicActivity.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "corelib/NonPeriodicActivity.hpp"
#include "corelib/Logger.hpp"

namespace ORO_CoreLib
{
    NonPeriodicActivity::NonPeriodicActivity(int priority, RunnableInterface* _r )
        : ORO_OS::SingleThread(priority, "NonPeriodicActivity" ),proc( new BlockingEventProcessor() ),
          runner(_r)
    {
        if ( runner )
            runner->setActivity(this);
    }

    NonPeriodicActivity::NonPeriodicActivity(int priority, const std::string& name, RunnableInterface* _r )
        : ORO_OS::SingleThread(priority, name ),proc( new BlockingEventProcessor() ),
          runner(_r)
    {
        if ( runner )
            runner->setActivity(this);
    }

    NonPeriodicActivity::~NonPeriodicActivity() 
    {
        this->stop();
        if ( runner )
            runner->setActivity( 0 );
        delete proc;
    }

    bool NonPeriodicActivity::run( RunnableInterface* r )
    {
        if ( isActive() )
            return false;
        if (runner)
            runner->setActivity(0);
        runner = r;
        if (runner)
            runner->setActivity(this);
        return true;
    }

    Seconds NonPeriodicActivity::getPeriod() const { return 0; }

    EventProcessor* NonPeriodicActivity::getEventProcessor() const { return proc; }

    ORO_OS::ThreadInterface* NonPeriodicActivity::thread() { return this; }

    bool NonPeriodicActivity::isPeriodic() const { return false; }

    bool NonPeriodicActivity::initialize() {
        bool result = proc->initialize();
        if ( runner && result )
            result = result && runner->initialize();
        return  result;
    }

    void NonPeriodicActivity::loop() { 
        if ( runner )
            runner->loop();
        else
            proc->loop(); // block in EventProcessor.
    }

    bool NonPeriodicActivity::breakLoop() {
        if ( runner )
            return runner->breakLoop();
        if ( proc )
            return proc->breakLoop(); // return from EventProcessor.
        return true;
    }

    void NonPeriodicActivity::finalize() {
        if ( runner )
            runner->finalize();
        proc->finalize();
    }


    bool NonPeriodicActivity::start() {
        return SingleThread::start();
    }

    bool NonPeriodicActivity::stop() {
        return SingleThread::stop();
    }

    bool NonPeriodicActivity::isRunning() const {
        return SingleThread::isRunning();
    }

    bool NonPeriodicActivity::isActive() const {
        return SingleThread::isActive();
    }



}
