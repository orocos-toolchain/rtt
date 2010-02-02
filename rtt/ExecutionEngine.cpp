/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ExecutionEngine.cxx

                        ExecutionEngine.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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



#include "Logger.hpp"
#include "ExecutionEngine.hpp"
#include "base/TaskCore.hpp"
#include "rtt-fwd.hpp"
#include "os/MutexLock.hpp"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <functional>
#include <algorithm>

#define ORONUM_EE_MQUEUE_SIZE 100

namespace RTT
{
    /**
     * @todo Rewrite some duplicate code such that 'taskc'
     * is manipulated together with the children, but keep
     * the taskc pointer as a reference in order to keep
     * track of the owner of this EE.
     */

    using namespace std;
    using namespace detail;
    using namespace boost;

    ExecutionEngine::ExecutionEngine( TaskCore* owner )
        : taskc(owner), mqueue(ORONUM_EE_MQUEUE_SIZE),
          funcs( ORONUM_EE_MQUEUE_SIZE),
          f_queue( new Queue<ExecutableInterface*>(ORONUM_EE_MQUEUE_SIZE) )
    {
    }

    ExecutionEngine::~ExecutionEngine()
    {
        Logger::In in("~ExecutionEngine");

        // make a copy to avoid call-back troubles:
        std::vector<TaskCore*> copy = children;
        for (std::vector<TaskCore*>::iterator it = copy.begin(); it != copy.end();++it){
            (*it)->setExecutionEngine( 0 );
        }
        assert( children.empty() );

        // empty executable interface list.
        ExecutableInterface* foo = 0;
        for(std::vector<ExecutableInterface*>::iterator it = funcs.begin();
                it != funcs.end(); ++it ) {
            foo = *it;
            if ( foo ) {
                foo->unloaded();
                (*it) = 0;
            }
        }
        while ( !f_queue->isEmpty() ) {
            f_queue->dequeue( foo );
            foo->unloaded();
        }

    }

    TaskCore* ExecutionEngine::getParent() {
        return taskc;
    }

    void ExecutionEngine::addChild(TaskCore* tc) {
        children.push_back( tc );
    }

    void ExecutionEngine::removeChild(TaskCore* tc) {
        vector<TaskCore*>::iterator it = find (children.begin(), children.end(), tc );
        if ( it != children.end() )
            children.erase(it);
    }

    void ExecutionEngine::processFunctions()
    {
        // Execute all loaded Functions :
        ExecutableInterface* foo = 0;
        // 1. Fetch new ones from queue.
        while ( !f_queue->isEmpty() ) {
            // 0. find an empty spot to store :
            std::vector<ExecutableInterface*>::iterator f_it = find(funcs.begin(), funcs.end(), foo );
            if ( f_it == funcs.end() )
                break; // no spot found, leave in queue.
            f_queue->dequeue( foo );
            *f_it = foo;
            // stored successfully, now reset for next item from queue.
            foo = 0;
        }
        assert(foo == 0);
        // 2. execute all present (if any):
        if ( find_if( funcs.begin(), funcs.end(), lambda::_1 != foo) != funcs.end() ) {
            MutexLock ml( syncer ); // we block until removeFunction finished.
            for(std::vector<ExecutableInterface*>::iterator it = funcs.begin();
                    it != funcs.end(); ++it ) {
                foo = *it;
                if ( foo ) {
                    if ( foo->execute() == false ){
                        foo->unloaded();
                        (*it) = 0;
                    }
                }
            }
        }
    }

    bool ExecutionEngine::runFunction( ExecutableInterface* f )
    {
        if (this->getActivity() && this->getActivity()->isActive() && f) {
            f->loaded(this);
            int result = f_queue->enqueue( f );
            // signal work is to be done:
            this->getActivity()->trigger();
            return result != 0;
        }
        return false;
    }

    bool ExecutionEngine::removeFunction( ExecutableInterface* f )
    {
        // Remove from the queue.
        std::vector<ExecutableInterface*>::iterator f_it = find(funcs.begin(), funcs.end(), f );
        if ( f_it != funcs.end() ) {
            {
                MutexLock ml(syncer);
                f->unloaded();
                *f_it = 0;
            }
            return true;
        }
        return false;
    }

    bool ExecutionEngine::initialize() {
        f_queue->clear();
        return true;
    }

    bool ExecutionEngine::hasWork()
    {
        return !mqueue.isEmpty();
    }

    void ExecutionEngine::processMessages()
    {
        // execute all commands from the AtomicQueue.
        // msg_lock may not be held when entering this function !
        DisposableInterface* com(0);
        {
            while ( !mqueue.isEmpty() ) {
                mqueue.dequeue( com );
                com->executeAndDispose();
            }
            // there's no need to hold the lock during
            // emptying the queue. But we must hold the
            // lock once between excuteAndDispose and the
            // broadcast to avoid the race condition in
            // waitForMessages().
            // This allows us to recurse into processMessages.
            MutexLock locker( msg_lock );
        }
        if ( com )
            msg_cond.broadcast(); // required for waitForMessages() (3rd party thread)
    }

    bool ExecutionEngine::process( DisposableInterface* c )
    {
        if ( c ) {
            bool result = mqueue.enqueue( c );
            this->getActivity()->trigger();
            msg_cond.broadcast(); // required for waitAndProcessMessages() (EE thread)
            return result;
        }
        return false;
    }

    void ExecutionEngine::step() {
        processMessages();
        processFunctions();
        processChildren(); // aren't these ExecutableInterfaces ie functions ?
    }

    void ExecutionEngine::processChildren() {
        // XXX this code belongs in TaskCore.
        // only call updateHook in the Running state.
        if ( taskc ) {
            if ( taskc->mTaskState == TaskCore::Running )
                taskc->updateHook();
            if (  taskc->mTaskState == TaskCore::RunTimeError )
                taskc->errorHook();
            // If an error occured (Running=>FatalError state), abort all !
            if ( taskc->mTaskState == TaskCore::FatalError) {
                this->getActivity()->stop(); // calls finalize()
                return;
            }
        }
        if ( !this->getActivity()->isRunning() ) return;

        // call all children as well.
        for (std::vector<TaskCore*>::iterator it = children.begin(); it != children.end();++it) {
            if ( (*it)->mTaskState == TaskCore::Running )
                (*it)->updateHook();
            if (  (*it)->mTaskState == TaskCore::RunTimeError )
                (*it)->errorHook();
            // If an error occured (Running=>FatalError state), abort all !
            if ( (*it)->mTaskState == TaskCore::FatalError) {
                this->getActivity()->stop(); // calls finalize()
                return;
            }
            if ( !this->getActivity()->isRunning() ) return;
        }
    }

    bool ExecutionEngine::breakLoop() {
        bool ok = true;
        if (taskc)
            ok = taskc->breakUpdateHook();
        for (std::vector<TaskCore*>::iterator it = children.begin(); it != children.end();++it) {
            ok = (*it)->breakUpdateHook() && ok;
            }
        return ok;
    }

    void ExecutionEngine::finalize() {
        // call all children, but only if they are not in the error state !
        for (std::vector<TaskCore*>::reverse_iterator rit = children.rbegin(); rit != children.rend();++rit) {
            (*rit)->stop();
            if ((*rit)->mTaskState != TaskCore::FatalError ) {
                (*rit)->mTaskState = TaskCore::Stopped;
            }

        }
        if (taskc ) {
            taskc->stop();
            if ( taskc->mTaskState != TaskCore::FatalError)
                taskc->mTaskState = TaskCore::Stopped;
        }
    }

}

