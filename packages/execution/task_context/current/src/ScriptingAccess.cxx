
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
