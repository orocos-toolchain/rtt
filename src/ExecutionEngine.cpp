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



#include <Logger.hpp>
#include <ExecutionEngine.hpp>
#include <TaskCore.hpp>
#include <algorithm>

namespace RTT
{
    /**
     * @todo Rewrite some duplicate code such that 'taskc'
     * is manipulated together with the children, but keep
     * the taskc pointer as a reference in order to keep
     * track of the owner of this EE.
     */

    using namespace std;

    ExecutionEngine::ExecutionEngine( TaskCore* owner )
        : taskc(owner), estate( Stopped ),
          cproc( 0 ), pproc( 0 ), smproc( 0 ), eproc( 0 )
    {
        this->setup();
    }

    ExecutionEngine::~ExecutionEngine()
    {
        Logger::In in("~ExecutionEngine");
        if (taskc)
            Logger::log() << Logger::Debug << "Destroying ExecutionEngine of "+taskc->getName()<<Logger::endl;

        // this order is fragile, so don't change it !
        delete smproc;
        delete pproc;
        delete eproc;
        delete cproc;

        // make a copy to avoid call-back troubles:
        std::vector<TaskCore*> copy = children;
        for (std::vector<TaskCore*>::iterator it = copy.begin(); it != copy.end();++it){
            (*it)->setExecutionEngine( 0 );
        }
        assert( children.empty() );
    }

    void ExecutionEngine::setup()
    {
        Logger::In in("ExecutionEngine");

        if (taskc)
            Logger::log() << Logger::Debug << "Creating ExecutionEngine for "+taskc->getName()<<Logger::endl;

#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
        cproc = new CommandProcessor(ORONUM_EXECUTION_PROC_QUEUE_SIZE);
#else
        cproc = 0;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        pproc = new ProgramProcessor(ORONUM_EXECUTION_PROC_QUEUE_SIZE);
#else
        pproc = 0;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        smproc = new StateMachineProcessor();
#else
        smproc = 0;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
        eproc = new EventProcessor();
#else
        eproc = 0;
#endif
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

    void ExecutionEngine::setActivity(ActivityInterface* t)
    {
        Logger::In in("ExecutionEngine::setActivity");

        if (cproc)
            cproc->setActivity(t);
        if (pproc)
            pproc->setActivity(t);
        if (smproc)
            smproc->setActivity(t);
        if (eproc)
            eproc->setActivity(t);

        RunnableInterface::setActivity(t);

//        // if an owner is present, print some info.
//        if (taskc) {
//            if ( t )
//                if ( ! t->isPeriodic() ) {
//                    Logger::log() << Logger::Info << taskc->getName()+" is not periodic."<< Logger::endl;
//                } else {
//                    Logger::log() << Logger::Info << taskc->getName()+" is periodic."<< Logger::endl;
//                }
//            else
//                Logger::log() << Logger::Info << taskc->getName()+" is disconnected from its activity."<< Logger::endl;
//        }
    }

    bool ExecutionEngine::activate()
    {
        // only do this from stopped.
        if (estate != Stopped ||  ! this->getActivity() )
            return false;
        // Note: use this flag to communicate to startContexts (called within start() below).
        estate = Activating;
        if ( this->getActivity()->start() ) {
            assert( estate == Active );
            return true;
        }
        // failure to activate:
        estate = Stopped;
        return false;
    }

    bool ExecutionEngine::start()
    {
        if ( !this->getActivity() )
            return false;
        // identical to starting the activity if Stopped
        if (estate == Stopped ) {
            assert( this->getActivity()->isActive() == false );
            if ( this->getActivity()->start() ) {
                assert( estate == Running );
                return true;
            }
        }
        // Only start Contexts if already active.
        if (estate == Active) {
            assert( this->getActivity()->isActive() );
            return startContexts();
        }
        return false;
    }

    bool ExecutionEngine::stop()
    {
        if ( !this->getActivity() )
            return false;
        if (this->getActivity()->stop() ) {
            assert( estate == Stopped );
            return true;
        }
        return false;
    }

    bool ExecutionEngine::startContexts()
    {
        // call user startHook or activateHook code.
        if (estate == Running)
            return false;
        if (estate == Stopped || estate == Active ) {
            if ( taskc ) {
                assert( taskc->mTaskState < TaskCore::Running); // we control the running flag.
                // detect error/unconfigured task.
                if ( taskc->mTaskState < TaskCore::Stopped || taskc->startHook() == false )
                    return false;
                taskc->mTaskState = TaskCore::Running;
            }

            // call all children
            for (std::vector<TaskCore*>::iterator it = children.begin(); it != children.end();++it) {
                assert( (*it)->mTaskState < TaskCore::Running);
                if ( (*it)->mTaskState < TaskCore::Stopped || (*it)->startHook() == false) {
                    // rewind
                    std::vector<TaskCore*>::reverse_iterator rit( it );
                    --rit;
                    for ( ; rit != children.rend(); ++rit ) {
                        (*rit)->stopHook();
                        (*it)->mTaskState = estate == Stopped ? TaskCore::Stopped : TaskCore::Active; //revert to old state.
                    }

                    //Logger::log() << Logger::Error << "ExecutionEngine's children's startHook() failed!" << Logger::endl;
                    if (taskc) {
                        this->taskc->stopHook();
                        this->taskc->mTaskState = estate == Stopped ? TaskCore::Stopped : TaskCore::Active;
                    }
                    // estate remains Active or Stopped.
                    return false;
                }
            }
            estate = Running; // got to running
            return true;
        }
        // Make all tasks active:
        if (estate == Activating) {
            if ( taskc ) {
                assert( taskc->mTaskState < TaskCore::Active);
                if ( taskc->mTaskState < TaskCore::Stopped || taskc->activateHook() == false)
                    return false;
                taskc->mTaskState = TaskCore::Active;
            }
            // call all children
            for (std::vector<TaskCore*>::iterator it = children.begin(); it != children.end();++it) {
                assert( (*it)->mTaskState < TaskCore::Active );
                // this line also detects unconfigured/error tasks:
                if ( (*it)->mTaskState < TaskCore::Stopped || (*it)->activateHook() == false) {
                    // rewind
                    std::vector<TaskCore*>::reverse_iterator rit( it );
                    --rit;
                    for ( ; rit != children.rend(); ++rit ) {
                        (*rit)->stopHook();
                        (*it)->mTaskState = TaskCore::Stopped; //revert to old state.
                    }

                    //Logger::log() << Logger::Error << "ExecutionEngine's children's startHook() failed!" << Logger::endl;
                    if (taskc) {
                        this->taskc->stopHook();
                        this->taskc->mTaskState = TaskCore::Stopped;
                    }
                    // estate falls back to Stopped.
                    estate = Stopped;
                    return false;
                }
            }
            estate = Active; // got to active
            return true;
        }
        return false;
    }

    bool ExecutionEngine::initialize() {
        if ( this->startContexts() == false )
            return false;

        // these always return true:
        bool ret = true;
        if ( pproc ) ret = pproc->initialize();
        assert (ret);
        if ( smproc ) ret = smproc->initialize();
        assert (ret);
        if ( cproc  ) ret = cproc->initialize();
        assert (ret);
        if ( eproc ) ret =eproc->initialize();
        assert (ret);
        return ret;
    }

    bool ExecutionEngine::hasWork()
    {
        return cproc->hasWork() && eproc->hasWork();
    }

    void ExecutionEngine::step() {
        Logger::In in( taskc->getName() );
        // this #ifdef ... #endif is only for speed optimisations.
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        if (pproc)
            pproc->step();
        if ( !this->getActivity()->isRunning() ) return;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        if (smproc)
            smproc->step();
        if ( !this->getActivity()->isRunning() ) return;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
        if (cproc)
            cproc->step();
        if ( !this->getActivity()->isRunning() ) return;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
        if (eproc)
            eproc->step();
        if ( !this->getActivity()->isRunning() ) return;
#endif
        // only call updateHook in the Running state.
        if ( taskc ) {
            if ( taskc->mTaskState == TaskCore::Running || taskc->mTaskState == TaskCore::RunTimeWarning )
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
            if ( (*it)->mTaskState == TaskCore::Running || (*it)->mTaskState == TaskCore::RunTimeWarning )
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
        return true;
    }

    void ExecutionEngine::finalize() {
        if (pproc)
            pproc->finalize();
        if (smproc)
            smproc->finalize();
        if (cproc)
            cproc->finalize();
        if (eproc)
            eproc->finalize();
        // call all children, but only if they are not in the error state !
        for (std::vector<TaskCore*>::reverse_iterator rit = children.rbegin(); rit != children.rend();++rit) {
            (*rit)->stopHook();
            if ((*rit)->mTaskState != TaskCore::FatalError ) {
                (*rit)->mTaskState = TaskCore::Stopped;
            }

        }
        if (taskc ) {
            taskc->stopHook();
            if ( taskc->mTaskState != TaskCore::FatalError)
                taskc->mTaskState = TaskCore::Stopped;
        }

        // Finally:
        estate = Stopped;
    }

    CommandProcessor* ExecutionEngine::commands() const {
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
        return dynamic_cast<CommandProcessor*>(cproc);
#else
        return 0;
#endif

    }

    ProgramProcessor* ExecutionEngine::programs() const {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        return dynamic_cast<ProgramProcessor*>(pproc);
#else
        return 0;
#endif
    }

    StateMachineProcessor* ExecutionEngine::states() const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        return dynamic_cast<StateMachineProcessor*>(smproc);
#else
        return 0;
#endif
    }

    EventProcessor* ExecutionEngine::events() const {
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
        return dynamic_cast<EventProcessor*>(eproc);
#else
        return 0;
#endif
    }

    void ExecutionEngine::setCommandProcessor(CommandProcessor* c) {
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
        delete cproc;
        cproc = c;
#endif
    }

    void ExecutionEngine::setProgramProcessor(ProgramProcessor* p) {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        delete pproc;
        pproc = p;
#endif
    }

    void ExecutionEngine::setStateMachineProcessor(StateMachineProcessor* s) {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        delete smproc;
        smproc = s;
#endif
    }

    void ExecutionEngine::setEventProcessor(EventProcessor* e) {
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
        delete eproc;
        eproc = e;
#endif
    }


}

