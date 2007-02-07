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
    
    using namespace std;

    ExecutionEngine::ExecutionEngine( TaskCore* owner )
        : taskc(owner),
          cproc( 0 ), pproc( 0 ), smproc( 0 ), eproc( 0 )
    {
        this->setup();
    }
        
    ExecutionEngine::~ExecutionEngine()
    {
        Logger::In in("~ExecutionEngine");
        if (taskc)
            Logger::log() << Logger::Debug << "Destroying ExecutionEngine of "+taskc->getName()<<Logger::endl;

        delete cproc;
        delete pproc;
        delete smproc;
        delete eproc;

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
        
        // if an owner is present, print some info.
        if (taskc) {
            if ( t)
                if ( ! t->isPeriodic() ) {
                    Logger::log() << Logger::Info << taskc->getName()+" is not periodic."<< Logger::endl;
                } else {
                    Logger::log() << Logger::Info << taskc->getName()+" is periodic."<< Logger::endl;
                }
            else
                Logger::log() << Logger::Info << taskc->getName()+" is disconnected from its activity."<< Logger::endl;
        }
    }

    bool ExecutionEngine::initialize() {
        // call user startup code.
        if ( taskc && taskc->startup() == false ) {
            //Logger::log() << Logger::Error << "ExecutionEngine's task startup() failed!" << Logger::endl;
            return false;
        }

        // call all children
        for (std::vector<TaskCore*>::iterator it = children.begin(); it != children.end();++it){
            if ( (*it)->startup() == false ) {
                std::vector<TaskCore*>::reverse_iterator rit( it );
                --rit;
                for ( ; rit != children.rend(); ++rit )
                    (*rit)->shutdown();
                //Logger::log() << Logger::Error << "ExecutionEngine's children's startup() failed!" << Logger::endl;
                if (taskc)
                    this->taskc->shutdown();
                return false;
            }
        }

        if ( pproc ? pproc->initialize() : true ) {
            if ( smproc ? smproc->initialize() : true ) {
                if( cproc ? cproc->initialize() : true ) {
                    if ( eproc ? eproc->initialize() : true ) {
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

        // call all children
        for (std::vector<TaskCore*>::reverse_iterator rit = children.rbegin(); rit != children.rend();++rit){
            (*rit)->shutdown();
        }
        return false;
    }

    void ExecutionEngine::step() {
        const char* location;
        if (taskc)
            location = taskc->getName().c_str();
        else
            location = "ExecutionEngine";
        Logger::In in( location );
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
        if (taskc)
            taskc->update();
        // call all children as well.
        for (std::vector<TaskCore*>::iterator it = children.begin(); it != children.end();++it) {
            Logger::In in( (*it)->getName().c_str() );
            (*it)->update();
        }
        return;
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
        // call all children
        for (std::vector<TaskCore*>::reverse_iterator rit = children.rbegin(); rit != children.rend();++rit) {
            (*rit)->shutdown();
        }
        if (taskc)
            taskc->shutdown();
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

