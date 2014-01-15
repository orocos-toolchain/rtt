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
#include "internal/MWSRQueue.hpp"
#include "TaskContext.hpp"
#include "internal/CatchConfig.hpp"
#include "extras/SlaveActivity.hpp"
#include "os/traces.h"

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#ifndef USE_CPP11
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#endif
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
        : taskc(owner),
          mqueue(new MWSRQueue<DisposableInterface*>(ORONUM_EE_MQUEUE_SIZE) ),
          f_queue( new MWSRQueue<ExecutableInterface*>(ORONUM_EE_MQUEUE_SIZE) ),
          mmaster(0)
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

        ExecutableInterface* foo;
        while ( f_queue->dequeue( foo ) )
            foo->unloaded();

        DisposableInterface* dis;
        while ( mqueue->dequeue( dis ) )
            dis->dispose();

        delete f_queue;
        delete mqueue;
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
        int nbr = f_queue->size(); // nbr to process.
        // 1. Fetch new ones from queue.
        while ( f_queue->dequeue(foo) ) {
            assert(foo);
            if ( foo->execute() == false ){
                foo->unloaded();
                msg_cond.broadcast(); // required for waitForFunctions() (3rd party thread)
            } else {
                f_queue->enqueue( foo );
            }
            if ( --nbr == 0) // we did a round-trip
                break;
        }
    }

    bool ExecutionEngine::runFunction( ExecutableInterface* f )
    {
        if (this->getActivity() && f) {
            // We only reject running functions when we're in the FatalError state.
            if (taskc && taskc->mTaskState == TaskCore::FatalError )
                return false;
            f->loaded(this);
            bool result = f_queue->enqueue( f );
            // signal work is to be done:
            this->getActivity()->trigger();
            return result;
        }
        return false;
    }

    struct RemoveMsg : public DisposableInterface {
        ExecutableInterface* mf;
        ExecutionEngine* mee;
        bool found;
        RemoveMsg(ExecutableInterface* f, ExecutionEngine* ee)
        : mf(f),mee(ee), found(false) {}
        virtual void executeAndDispose() {
            mee->removeSelfFunction( mf );
            found = true; // always true in order to be able to quit waitForMessages.
        }
        virtual void dispose() {}
        virtual bool isError() const { return false;}

    };

    bool ExecutionEngine::removeFunction( ExecutableInterface* f )
    {
        // Remove from the queue.
        if ( !f )
            return false;

        if ( !f->isLoaded() )
            return true;

        // When not running, just remove.
        if ( getActivity() == 0 || !this->getActivity()->isActive() ) {
            if ( removeSelfFunction( f ) == false )
                return false;
        } else {
            // Running: create message on stack.
            RemoveMsg rmsg(f,this);
            if ( this->process(&rmsg) )
#ifdef USE_CPP11
                this->waitForMessages( ! bind(&ExecutableInterface::isLoaded, f) || bind(&RemoveMsg::found,boost::ref(rmsg)) );
#else
                this->waitForMessages( ! lambda::bind(&ExecutableInterface::isLoaded, f) || lambda::bind(&RemoveMsg::found,boost::ref(rmsg)) );
#endif
            if (!rmsg.found)
                return false;
        }
        // unloading was succesful, now notify unloading:
        f->unloaded();
        return true;
    }

    bool ExecutionEngine::removeSelfFunction(ExecutableInterface* f  )
    {
        // since this function is executed in process messages, it is always safe to execute.
        if ( !f )
            return false;
        int nbr = f_queue->size();
        while (nbr != 0) {
            ExecutableInterface* foo = 0;
            if ( !f_queue->dequeue(foo) )
                return false;
            if ( f  == foo) {
                return true;
            }
            f_queue->enqueue(foo);
            --nbr;
        }
        return true;
    }

    bool ExecutionEngine::initialize() {
        // nop
        return true;
    }

    bool ExecutionEngine::hasWork()
    {
        return !mqueue->isEmpty();
    }

    void ExecutionEngine::processMessages()
    {
        // execute all commands from the AtomicQueue.
        // msg_lock may not be held when entering this function !
        DisposableInterface* com(0);
        {
            while ( mqueue->dequeue(com) ) {
                assert( com );
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
        // forward message to master ExecutionEngine if available
        if (mmaster) {
            return mmaster->process(c);
        }

        if ( c && this->getActivity() ) {
            // We only reject running functions when we're in the FatalError state.
            if (taskc && taskc->mTaskState == TaskCore::FatalError )
                return false;

            bool result = mqueue->enqueue( c );
            this->getActivity()->trigger();
            msg_cond.broadcast(); // required for waitAndProcessMessages() (EE thread)
            return result;
        }
        return false;
    }

    void ExecutionEngine::waitForMessages(const boost::function<bool(void)>& pred)
    {
        // forward the call to the master ExecutionEngine which is processing messages for us...
        if (mmaster) {
            mmaster->waitForMessages(pred);
            return;
        }

        if (this->getActivity()->thread()->isSelf())
            waitAndProcessMessages(pred);
        else
            waitForMessagesInternal(pred);
    }


    void ExecutionEngine::waitForFunctions(const boost::function<bool(void)>& pred)
    {
        if (this->getActivity()->thread()->isSelf())
            waitAndProcessFunctions(pred);
        else
            waitForMessagesInternal(pred); // same as for messages.
    }

    void ExecutionEngine::setMaster(ExecutionEngine *master)
    {
        mmaster = master;
    }

    void ExecutionEngine::setActivity( base::ActivityInterface* task )
    {
        extras::SlaveActivity *slave_activity = dynamic_cast<extras::SlaveActivity *>(task);
        if (slave_activity && slave_activity->getMaster()) {
            ExecutionEngine *master = dynamic_cast<ExecutionEngine *>(slave_activity->getMaster()->getRunner());
            setMaster(master);
        } else {
            setMaster(0);
        }
        RTT::base::RunnableInterface::setActivity(task);
    }

    void ExecutionEngine::waitForMessagesInternal(boost::function<bool(void)> const& pred)
    {
        if ( pred() )
            return;
        // only to be called from the thread not executing step().
        os::MutexLock lock(msg_lock);
        while (!pred()) { // the mutex guards that processMessages can not run between !pred and the wait().
            msg_cond.wait(msg_lock); // now processMessages may run.
        }
    }


    void ExecutionEngine::waitAndProcessMessages(boost::function<bool(void)> const& pred)
    {
        while ( !pred() ){
            // may not be called while holding the msg_lock !!!
            this->processMessages();
            {
                // only to be called from the thread executing step().
                // We must lock because the cond variable will unlock msg_lock.
                os::MutexLock lock(msg_lock);
                if (!pred()) {
                    msg_cond.wait(msg_lock); // now processMessages may run.
                } else {
                    return; // do not process messages when pred() == true;
                }
            }
        }
    }

    void ExecutionEngine::waitAndProcessFunctions(boost::function<bool(void)> const& pred)
    {
        while ( !pred() ){
            // may not be called while holding the msg_lock !!!
            this->processFunctions();
            {
                // only to be called from the thread executing step().
                // We must lock because the cond variable will unlock msg_lock.
                os::MutexLock lock(msg_lock);
                if (!pred()) {
                    msg_cond.wait(msg_lock); // now processMessages may run.
                } else {
                    return; // do not process messages when pred() == true;
                }
            }
        }
    }

    void ExecutionEngine::step() {
        processMessages();
        processFunctions();
        processChildren(); // aren't these ExecutableInterfaces ie functions ?
    }

    void ExecutionEngine::processChildren() {
        // only call updateHook in the Running state.
        if ( taskc ) {
            // A trigger() in startHook() will be ignored, we trigger in TaskCore after startHook finishes.
            if ( taskc->mTaskState == TaskCore::Running && taskc->mTargetState == TaskCore::Running ) {
                TRY (
                    { tracepoint_context(orocos_rtt, TaskContext_prepareUpdateHook, "");
                        taskc->prepareUpdateHook(); }
                    { tracepoint_context(orocos_rtt, TaskContext_updateHook, "");
                        taskc->updateHook(); }
                ) CATCH(std::exception const& e,
                    log(Error) << "in updateHook(): switching to exception state because of unhandled exception" << endlog();
                    log(Error) << "  " << e.what() << endlog();
                    taskc->exception();
               ) CATCH_ALL (
                    log(Error) << "in updateHook(): switching to exception state because of unhandled exception" << endlog();
                    taskc->exception(); // calls stopHook,cleanupHook
                )
            }
            // in case start() or updateHook() called error(), this will be called:
            if (taskc->mTaskState == TaskCore::RunTimeError && taskc->mTargetState >= TaskCore::Running) {
                TRY (
                    { tracepoint_context(orocos_rtt, TaskContext_errorHook, "");
                        taskc->errorHook(); }
                ) CATCH(std::exception const& e,
                    log(Error) << "in errorHook(): switching to exception state because of unhandled exception" << endlog();
                    log(Error) << "  " << e.what() << endlog();
                    taskc->exception();
               ) CATCH_ALL (
                    log(Error) << "in errorHook(): switching to exception state because of unhandled exception" << endlog();
                    taskc->exception(); // calls stopHook,cleanupHook
                )
            }
        }
        if ( !this->getActivity() || ! this->getActivity()->isRunning() ) return;

        // call all children as well.
        for (std::vector<TaskCore*>::iterator it = children.begin(); it != children.end();++it) {
            if ( (*it)->mTaskState == TaskCore::Running  && (*it)->mTargetState == TaskCore::Running  ){
                TRY (
                    { tracepoint_context(orocos_rtt, TaskContext_prepareUpdateHook, "");
                        (*it)->prepareUpdateHook(); }
                    { tracepoint_context(orocos_rtt, TaskContext_updateHook, "");
                        (*it)->updateHook(); }
                ) CATCH(std::exception const& e,
                    log(Error) << "in updateHook(): switching to exception state because of unhandled exception" << endlog();
                    log(Error) << "  " << e.what() << endlog();
                    (*it)->exception();
               ) CATCH_ALL (
                    log(Error) << "in updateHook(): switching to exception state because of unhandled exception" << endlog();
                    (*it)->exception(); // calls stopHook,cleanupHook
                )
            }
            if ((*it)->mTaskState == TaskCore::RunTimeError && (*it)->mTargetState == TaskCore::RunTimeError){
                TRY (
                    (*it)->errorHook();
                ) CATCH(std::exception const& e,
                    log(Error) << "in errorHook(): switching to exception state because of unhandled exception" << endlog();
                    log(Error) << "  " << e.what() << endlog();
                    (*it)->exception();
               ) CATCH_ALL (
                    log(Error) << "in errorHook(): switching to exception state because of unhandled exception" << endlog();
                    (*it)->exception(); // calls stopHook,cleanupHook
                )
            }
            if ( !this->getActivity() || ! this->getActivity()->isRunning() ) return;
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

    bool ExecutionEngine::stopTask(TaskCore* task) {
        // stop and start where former will call breakLoop() in case of non-periodic.
        // this is a forced synchronization point, since stop() will only return when
        // step() returned.
        if ( getActivity() && this->getActivity()->stop() ) {
            this->getActivity()->start();
            return true;
        }
        return false;
    }

    void ExecutionEngine::setExceptionTask() {
        std::string name;
        TaskContext* tc = dynamic_cast<TaskContext*>(taskc);
        if (tc)
            name = tc->getName();
        else if (taskc)
            name = "TaskCore";
        else
            name = "GlobalEngine";
        log(Error) << "in "<<name<<": unhandled exception in sent operation." << endlog();
        if(taskc)
            taskc->exception();
    }
            

    void ExecutionEngine::finalize() {
        // nop
    }

}

