/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  ScriptingAccess.cxx 

                        ScriptingAccess.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 

#include "ScriptingAccess.hpp"
#include "Logger.hpp"
#include "TaskContext.hpp"

#include <execution_task_context.h>
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
#include "ProgramProcessor.hpp"
#endif
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
#include "StateMachineProcessor.hpp"
#endif

namespace RTT
{
    using namespace std;
    

    ScriptingAccess::~ScriptingAccess()
    {
    }

    int ScriptingAccess::execute(const string& code ){
        Logger::In in("ScriptingAccess::execute");
        Logger::log() << Logger::Error << "No Parser available to execute '"+code+"' ." << Logger::endl;
        return -1;
    }

    CommandC ScriptingAccess::getCommand(int ticket) {
        return CommandC();
    }

    ScriptingAccess::Functions  ScriptingAccess::loadFunctions( std::string file, bool do_throw/* = false*/ )
    {
        Logger::In in("ScriptingAccess::loadFunctions");
        Logger::log() << Logger::Error << "No Parser available to load '"+file+"' ." << Logger::endl;
        return Functions();
    }

    ScriptingAccess::Functions  ScriptingAccess::loadFunctions( string code, string filename, bool mrethrow )
    {
      Logger::In in("ScriptingAccess::loadFunctions");
      Logger::log() << Logger::Error << "No Parser available to load '"+filename+"' ." << Logger::endl;
      return Functions();
    }
    
    bool ScriptingAccess::loadPrograms( std::string file, bool do_throw /*= false*/ )
    {
        Logger::In in("ScriptingAccess::loadProgram");
        Logger::log() << Logger::Error << "No Parser available to load '"+file+"' ." << Logger::endl;
        return false;
    }

    bool ScriptingAccess::loadPrograms( string code, string filename, bool mrethrow )
    {
        Logger::In in("ScriptingAccess::loadProgram");
        Logger::log() << Logger::Error << "No Parser available to load '"+filename+"' ." << Logger::endl;
        return false;
    }
    
    bool ScriptingAccess::unloadProgram( string name, bool do_throw ){
        Logger::In in("ScriptingAccess::unloadProgram");
        Logger::log() << Logger::Error << "No Parser available to unload '"+name+"' ." << Logger::endl;
        return false;
    }

    bool ScriptingAccess::hasProgram(string name) const {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        return mparent->engine()->programs()->getProgram(name);
#else
        return false;
#endif
    }

    std::vector<std::string> ScriptingAccess::getPrograms() const{
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        return mparent->engine()->programs()->getProgramList();
#else
        return std::vector<std::string>();
#endif
    }

    int ScriptingAccess::getProgramLine(string name) const {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        const ProgramInterfacePtr pi = mparent->engine()->programs()->getProgram(name);
        return pi ? pi->getLineNumber() : -1;
#else
        return -1;
#endif
    }
    
    string ScriptingAccess::getProgramText(string name ) const {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        const ProgramInterfacePtr pi = mparent->engine()->programs()->getProgram(name);
        return pi ? pi->getText() : "";
#else
        return "";
#endif
    }
        
    string ScriptingAccess::getProgramStatus(string name ) const {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        return mparent->engine()->programs()->getProgramStatusStr(name);
#else
        return "";
#endif
    }

    bool ScriptingAccess::loadStateMachines( std::string file, bool do_throw /*= false*/  )
    {
        Logger::In in("ScriptingAccess::loadStateMachine");
        Logger::log() << Logger::Error << "No Parser available to load '"+file+"' ." << Logger::endl;
        return false;
    }

    bool ScriptingAccess::loadStateMachines( string code, string filename, bool mrethrow ) 
    {
        Logger::In in("ScriptingAccess::loadStateMachine");
        Logger::log() << Logger::Error << "No Parser available to load '"+filename+"' ." << Logger::endl;
        return false;
    }
    
    bool ScriptingAccess::unloadStateMachine( string name, bool do_throw ) {
        Logger::In in("ScriptingAccess::unloadStateMachine");
        Logger::log() << Logger::Error << "No Parser available to unload '"+name+"' ." << Logger::endl;
        return false;
    }

    bool ScriptingAccess::hasStateMachine(string name) const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        return mparent->engine()->states()->getStateMachine(name);
#else
        return false;
#endif
    }


    std::vector<std::string> ScriptingAccess::getStateMachines() const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        return mparent->engine()->states()->getStateMachineList();
#else
        return std::vector<std::string>();
#endif
    }

    string ScriptingAccess::getStateMachineText(string name ) const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        const StateMachinePtr sm = mparent->engine()->states()->getStateMachine(name);
        return sm ? sm->getText() : "";
#else
        return "";
#endif
    }
    
    string ScriptingAccess::getStateMachineStatus(string name ) const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        return mparent->engine()->states()->getStateMachineStatusStr(name);
#else
        return "";
#endif
    }

    int ScriptingAccess::getStateMachineLine(string name ) const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        const StateMachinePtr sm = mparent->engine()->states()->getStateMachine(name);
        return sm ? sm->getLineNumber() : -1;
#else
        return -1;
#endif
    }
    
    string ScriptingAccess::getCurrentState( string name ) const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        const StateMachinePtr sm = mparent->engine()->states()->getStateMachine(name);
        return sm ? sm->getCurrentStateName() : "";
#else
        return "";
#endif
    }
}
