/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ExecutionEngine.cxx 

                        ExecutionEngine.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#include <rtt/os/Semaphore.hpp>
#include <rtt/Logger.hpp>
#include <rtt/ExecutionEngine.hpp>
#include <rtt/TaskCore.hpp>
#include <algorithm>

namespace RTT
{
    
    using namespace std;

    ExecutionEngine::ExecutionEngine( TaskCore* owner, ExecutionEngine* parent /* = 0 */ )
        : work_sem( parent ? 0 : new OS::Semaphore(0)),
          loop_sem( parent ? 0 : new OS::Semaphore(0)),
          taskc(owner),
          mainee(parent ? parent->getParent() : 0),
          cproc( 0 ), pproc( 0 ), smproc( 0 ), eproc( 0 ),
          eerun( false )
    {
        this->setup();

        // add self to parent
        if (mainee)
            mainee->children.push_back(this);
    }
        
    ExecutionEngine::~ExecutionEngine()
    {
        Logger::In in("~ExecutionEngine");
        Logger::log() << Logger::Debug << "Destroying ExecutionEngine of "+taskc->getName()<<Logger::endl;

        // it is possible that the mainee is assigned to a task
        // but that this child EE is already destroyed.
        if (mainee && this->getActivity())
            this->RunnableInterface::setActivity(0);

        delete cproc;
        delete pproc;
        delete smproc;
        delete eproc;

        delete work_sem;
        delete loop_sem;

        // add children to parent (reparent) and remove self from parent
        if (mainee) {
            std::vector<ExecutionEngine*>::iterator it=find(mainee->children.begin(),mainee->children.end(), this);
            if ( it != mainee->children.end() )
                mainee->children.erase(it);
        }
        for (std::vector<ExecutionEngine*>::iterator it = children.begin(); it != children.end();++it){
            // this operation may make a child orphan.
            (*it)->mainee = mainee;
            if (mainee)
                mainee->children.push_back(*it); // not orphan
            else {
                // orphan, provide it new processors.
                (*it)->setup();
            }
        }
    }

    void ExecutionEngine::setup()
    {
        Logger::In in("ExecutionEngine");

        if (mainee)
            Logger::log() << Logger::Debug << "Using ExecutionEngine of task "+mainee->getTaskCore()->getName()<<" in " <<taskc->getName() << Logger::endl;
        else
            Logger::log() << Logger::Debug << "Creating ExecutionEngine for "+taskc->getName()<<Logger::endl;

        if ( mainee == 0 ) {
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
            cproc = new CommandProcessor(ORONUM_EXECUTION_PROC_QUEUE_SIZE, work_sem);
#else
            cproc = 0;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
            pproc = new ProgramProcessor(ORONUM_EXECUTION_PROC_QUEUE_SIZE, work_sem);
#else
            pproc = 0;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
            smproc = new StateMachineProcessor(work_sem);
#else
            smproc = 0;
#endif
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
            eproc = new EventProcessor();
#else
            eproc = 0;
#endif
        } else {
            delete cproc;
            delete pproc;
            delete smproc;
            delete eproc;
        }
    }

    ExecutionEngine* ExecutionEngine::getParent() {
        return mainee ? mainee : this;
    }

    OS::Semaphore* ExecutionEngine::getSemaphore() const {
        return work_sem;
    }

    void ExecutionEngine::reparent(ExecutionEngine* new_parent) {
        Logger::In in("ExecutionEngine");
        Logger::log() << Logger::Debug << "Reparenting "+taskc->getName()<<Logger::endl;

        if (new_parent == this || new_parent == mainee)
            return;
        // first remove from old parent.
        if (mainee) {
            std::vector<ExecutionEngine*>::iterator it=find(mainee->children.begin(),mainee->children.end(), this);
            if ( it != mainee->children.end() )
                mainee->children.erase(it);
            mainee = 0;
        }
        if (new_parent != 0 ) {
            // special order here, first do setActivity without disturbing future mainee
            this->setActivity( new_parent->getParent()->getActivity() );
            mainee = new_parent->getParent();
            mainee->children.push_back(this);
            return;
        }
        if (new_parent == 0) {
            // orphan, reset activity to zero.
            this->setActivity(0);
        }

        this->setup();
    }

    void ExecutionEngine::setActivity(ActivityInterface* t)
    {
        Logger::In in("ExecutionEngine::setActivity");

        if (mainee) {
            // Let parent update RunnableInterface...
            mainee->setActivity(t);
        } else {
            if (t)
                Logger::log() <<Logger::Debug <<taskc->getName()<<": informing processors of new activity."<<Logger::endl;
            else
                Logger::log() <<Logger::Debug <<taskc->getName()<<": disconnecting processors of activity."<<Logger::endl;
                
            // I am an orphan.
            if (cproc)
                cproc->setActivity(t);
            if (pproc)
                pproc->setActivity(t);
            if (smproc)
                smproc->setActivity(t);
            if (eproc)
                eproc->setActivity(t);
            RunnableInterface::setActivity(t);
        }
        
        // inform all children, even if we have a parent.
        // attention, avoid recursive endless loop !
        for (std::vector<ExecutionEngine*>::iterator it = children.begin(); it !=children.end();++it)
            (*it)->RunnableInterface::setActivity( t );

        if (t)
            if ( ! t->isPeriodic() ) {
                Logger::log() << Logger::Info << taskc->getName()+" is not periodic."<< Logger::endl;
            } else {
                Logger::log() << Logger::Info << taskc->getName()+" is periodic."<< Logger::endl;
            }
        else
            Logger::log() << Logger::Info << taskc->getName()+" is disconnected from its activity."<< Logger::endl;
    }

    bool ExecutionEngine::initialize() {
        if ( mainee )
            return true;

        // call user startup code.
        if ( taskc->startup() == false ) {
            //Logger::log() << Logger::Error << "ExecutionEngine's task startup() failed!" << Logger::endl;
            return false;
        }

        // call all children if we are a mainee
        for (std::vector<ExecutionEngine*>::iterator it = children.begin(); it != children.end();++it){
            if ( (*it)->taskc->startup() == false ) {
                std::vector<ExecutionEngine*>::reverse_iterator rit( it );
                --rit;
                for ( ; rit != children.rend(); ++rit )
                    (*rit)->taskc->shutdown();
                //Logger::log() << Logger::Error << "ExecutionEngine's children's startup() failed!" << Logger::endl;
                return false;
            }
        }

        if ( pproc ? pproc->initialize() : true ) {
            if ( smproc ? smproc->initialize() : true ) {
                if( cproc ? cproc->initialize() : true ) {
                    if ( eproc ? eproc->initialize() : true ) {
                        // non periodic loop() uses this flag to detect breakLoop()
                        if ( !this->getActivity()->isPeriodic() )
                            eerun = true;
                        return true;
                    }
                    // failure: shut down again.
                    if (cproc)
                        cproc->finalize();
                } 
                if (smproc)
                    smproc->finalize();
            }
            if ( pproc )
                pproc->finalize();
        }
        //Logger::log() << Logger::Error << "ExecutionEngine's processors failed!" << Logger::endl;
        return false;
    }

    void ExecutionEngine::step() {
        if ( !mainee ) {
            // this #ifdef ... #endif is only for speed optimisations.
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
            if (pproc)
                pproc->step();
#endif
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
            if (smproc)
                smproc->step();
#endif
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
            if (cproc)
                cproc->step();
#endif
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
            if (eproc)
                eproc->step();
#endif
            taskc->update();
            // call all children as well.
            for (std::vector<ExecutionEngine*>::iterator it = children.begin(); it != children.end();++it)
                (*it)->taskc->update();
            return;
        }
    }

    void ExecutionEngine::loop() {
        if (mainee) {
            return;
        }
        while( eerun ) {
            work_sem->wait();
            this->step();
        }
        loop_sem->signal();
    }

    bool ExecutionEngine::breakLoop() {
        if (mainee)
            return false;
        if (eerun) {
            eerun = false;
            work_sem->signal();
            loop_sem->wait();
            return true;
        }
        return false;
    }

    void ExecutionEngine::finalize() {
        if ( !mainee ) {
            eerun = false;
            if (pproc)
                pproc->finalize();
            if (smproc)
                smproc->finalize();
            if (cproc)
                cproc->finalize();
            if (eproc)
                eproc->finalize();
            taskc->shutdown();
            // call all children as well.
            for (std::vector<ExecutionEngine*>::iterator it = children.begin(); it != children.end();++it)
                (*it)->taskc->shutdown();
        }
    }

    CommandProcessor* ExecutionEngine::commands() const {
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
        return mainee ? mainee->commands() : dynamic_cast<CommandProcessor*>(cproc);
#else
        return 0;
#endif
        
    }

    ProgramProcessor* ExecutionEngine::programs() const {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        return mainee ? mainee->programs() : dynamic_cast<ProgramProcessor*>(pproc);
#else
        return 0;
#endif
    }

    StateMachineProcessor* ExecutionEngine::states() const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        return mainee ? mainee->states() : dynamic_cast<StateMachineProcessor*>(smproc);
#else
        return 0;
#endif
    }

    EventProcessor* ExecutionEngine::events() const {
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
        return mainee ? mainee->events() : dynamic_cast<EventProcessor*>(eproc);
#else
        return 0;
#endif
    }

    void ExecutionEngine::setCommandProcessor(CommandProcessor* c) {
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
        if (mainee)
            mainee->setCommandProcessor(c);
        else
            cproc = c;
#endif
    }
    
    void ExecutionEngine::setProgramProcessor(ProgramProcessor* p) {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        if (mainee)
            mainee->setProgramProcessor(p);
        else
            pproc = p;
#endif
    }

    void ExecutionEngine::setStateMachineProcessor(StateMachineProcessor* s) {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        if (mainee)
            mainee->setStateMachineProcessor(s);
        else
            smproc = s;
#endif
    }

    void ExecutionEngine::setEventProcessor(EventProcessor* e) {
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
        if (mainee)
            mainee->setEventProcessor(e);
        else
            eproc = e;
#endif
    }


}

