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
 
 

#include <os/Semaphore.hpp>
#include <corelib/CompletionProcessor.hpp>
#include <corelib/Logger.hpp>
#include <execution/ExecutionEngine.hpp>
#include <execution/TaskCore.hpp>
//#include <execution/TaskCore.hpp>
#include <algorithm>

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    using namespace std;

    ExecutionEngine::ExecutionEngine( TaskCore* owner, ExecutionEngine* parent /* = 0 */ )
        : work_sem( parent ? 0 : new ORO_OS::Semaphore(0)),
          loop_sem( parent ? 0 : new ORO_OS::Semaphore(0)),
          taskc(owner),
          mainee(parent ? parent->getParent() : 0),
          cproc( mainee ? 0 : new CommandProcessor(ORONUM_EXECUTION_PROC_QUEUE_SIZE, work_sem) ),
          pproc( mainee ? 0 : new ProgramProcessor(ORONUM_EXECUTION_PROC_QUEUE_SIZE, work_sem) ),
          smproc( mainee ? 0 : new StateMachineProcessor(work_sem) ),
          eproc( mainee ? 0 : new EventProcessor() ),
          eerun( false )
    {
        Logger::In in("ExecutionEngine");
        if (mainee)
            Logger::log() << Logger::Debug << "Creating child "+owner->getName()+" of task "+mainee->getTaskCore()->getName()<<Logger::endl;
        else
            Logger::log() << Logger::Debug << "Creating root task "+owner->getName()<<Logger::endl;
        // add self to parent
        if (mainee)
            mainee->children.push_back(this);
    }
        
    ExecutionEngine::~ExecutionEngine()
    {
        Logger::In in("~ExecutionEngine");
        Logger::log() << Logger::Debug << "Destroying "+taskc->getName()<<Logger::endl;

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
                (*it)->cproc = new CommandProcessor();
                (*it)->pproc = new ProgramProcessor();
                (*it)->smproc = new StateMachineProcessor();
                (*it)->eproc = new EventProcessor();
            }
        }
    }

    ExecutionEngine* ExecutionEngine::getParent() {
        return mainee ? mainee : this;
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
            delete cproc;
            cproc = 0;
            delete pproc;
            pproc = 0;
            delete smproc;
            smproc = 0;
            delete eproc;
            eproc = 0;
            // special order here, first do setActivity without disturbing future mainee
            this->setActivity( new_parent->getParent()->getActivity() );
            mainee = new_parent->getParent();
            mainee->children.push_back(this);
            return;
        }
        if (new_parent == 0) {
            // orphan, provide new processors.
            cproc = new CommandProcessor();
            pproc = new ProgramProcessor();
            smproc = new StateMachineProcessor();
            eproc = new EventProcessor();
            this->setActivity(0);
        }
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

        if ( pproc->initialize() ) {
            if ( smproc->initialize() ) {
                if( cproc->initialize() ) {
                    if (eproc->initialize()) {
                        // non periodic loop() uses this flag to detect breakLoop()
                        if ( !this->getActivity()->isPeriodic() )
                            eerun = true;
                        return true;
                    }
                    cproc->finalize();
                } 
                smproc->finalize();
            }
            pproc->finalize();
        }
        //Logger::log() << Logger::Error << "ExecutionEngine's processors failed!" << Logger::endl;
        return false;
    }

    void ExecutionEngine::step() {
        if ( !mainee ) {
            pproc->step();
            smproc->step();
            cproc->step();
            eproc->step();
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
            pproc->finalize();
            smproc->finalize();
            cproc->finalize();
            eproc->finalize();
            taskc->shutdown();
            // call all children as well.
            for (std::vector<ExecutionEngine*>::iterator it = children.begin(); it != children.end();++it)
                (*it)->taskc->shutdown();
        }
    }

    ORO_CoreLib::EventProcessor* ExecutionEngine::events() const {
        return mainee ? mainee->events() : eproc;
    }


    ORO_CoreLib::EventProcessor* ExecutionEngine::getEventProcessor() const {
        return mainee ? mainee->events() : eproc;
    }

}

