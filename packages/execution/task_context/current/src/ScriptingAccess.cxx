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
 
 

#include "execution/ScriptingAccess.hpp"
#include "corelib/Logger.hpp"
#include "execution/TaskContext.hpp"

namespace ORO_Execution
{
    using namespace std;
    using namespace ORO_CoreLib;

    ScriptingAccess::~ScriptingAccess()
    {
    }

    bool ScriptingAccess::execute( string code ){
        Logger::In in("ScriptingAccess::execute");
        Logger::log() << Logger::Error << "No Parser available to execute '"+code+"' ." << Logger::endl;
        return false;
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
        return mparent->engine()->programs()->getProgram(name);
    }

    std::vector<std::string> ScriptingAccess::getPrograms() const{
        return mparent->engine()->programs()->getProgramList();
    }

    int ScriptingAccess::getProgramLine(string name) const {
        const ProgramInterfacePtr pi = mparent->engine()->programs()->getProgram(name);
        return pi ? pi->getLineNumber() : -1;
    }
    
    string ScriptingAccess::getProgramText(string name ) const {
        const ProgramInterfacePtr pi = mparent->engine()->programs()->getProgram(name);
        return pi ? pi->getText() : "";
    }
        
    string ScriptingAccess::getProgramStatus(string name ) const {
        return mparent->engine()->programs()->getProgramStatusStr(name);
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
        return mparent->engine()->states()->getStateMachine(name);
    }


    std::vector<std::string> ScriptingAccess::getStateMachines() const {
        return mparent->engine()->states()->getStateMachineList();
    }

    string ScriptingAccess::getStateMachineText(string name ) const {
        const StateMachinePtr sm = mparent->engine()->states()->getStateMachine(name);
        return sm ? sm->getText() : "";
    }
    
    string ScriptingAccess::getStateMachineStatus(string name ) const {
        return mparent->engine()->states()->getStateMachineStatusStr(name);
    }

    int ScriptingAccess::getStateMachineLine(string name ) const {
        const StateMachinePtr sm = mparent->engine()->states()->getStateMachine(name);
        return sm ? sm->getLineNumber() : -1;
    }
    
    string ScriptingAccess::getCurrentState( string name ) const {
        const StateMachinePtr sm = mparent->engine()->states()->getStateMachine(name);
        return sm ? sm->getCurrentStateName() : "";
    }
}
