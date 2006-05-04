
#include "corba/ScriptingAccessProxy.hpp"
#include "corba/ScriptingAccessC.h"
#include "execution/ProgramExceptions.hpp"


namespace ORO_Corba
{
    using namespace std;
    using namespace ORO_Execution;

    ScriptingAccessProxy::~ScriptingAccessProxy()
    {
    }

    bool ScriptingAccessProxy::execute( string code ) {
        return msa->execute( code.c_str() );
    }

    bool ScriptingAccessProxy::loadPrograms( string code, string filename, bool do_throw ) {
        try {
            msa->loadPrograms( code.c_str(), filename.c_str() );
        } catch (Orocos::ScriptingAccess::LoadException& le)
            {
                if ( do_throw) {
                    throw program_load_exception( string(le.what) );
                }
                return false;
            }
        return true;
    }
    
    bool ScriptingAccessProxy::unloadProgram( string name, bool do_throw ){
        try {
            msa->unloadProgram( name.c_str() );
        } catch (Orocos::ScriptingAccess::LoadException& le)
            {
                if ( do_throw) {
                    throw program_load_exception( string(le.what) );
                }
                return false;
            }
        return true;
    }

    std::vector<std::string> ScriptingAccessProxy::getPrograms() const{
        std::vector<std::string> ret;
        Orocos::ScriptingAccess::ProgramNames_var cret = msa->getPrograms();
        for(CORBA::ULong i= 0; i != cret->length(); ++i)
            ret.push_back( string(cret[i]) );
        return ret;
    }

    int ScriptingAccessProxy::getProgramLine(string name) const {
        return msa->getProgramLine( name.c_str() );
    }
    
    bool ScriptingAccessProxy::hasProgram(string name) const {
        return msa->hasProgram( name.c_str() );
    }
    
    string ScriptingAccessProxy::getProgramText(string name ) const {
        return msa->getProgramText( name.c_str() );
    }
        
    string ScriptingAccessProxy::getProgramStatus(string name ) const {
        return msa->getProgramStatus( name.c_str() );
    }

    bool ScriptingAccessProxy::loadStateMachines( string code, string filename, bool do_throw ) {
        try {
            msa->loadStateMachines( code.c_str(), filename.c_str() );
        } catch (Orocos::ScriptingAccess::LoadException& le)
            {
                if ( do_throw) {
                    throw program_load_exception( string(le.what) );
                }
                return false;
            }
        return true;
    }
    
    bool ScriptingAccessProxy::unloadStateMachine( string name, bool do_throw ) {
        try {
            msa->unloadStateMachine( name.c_str() );
        } catch (Orocos::ScriptingAccess::LoadException& le)
            {
                if ( do_throw) {
                    throw program_load_exception( string(le.what) );
                }
                return false;
            }
        return true;
    }

    std::vector<std::string> ScriptingAccessProxy::getStateMachines() const {
        std::vector<std::string> ret;
        Orocos::ScriptingAccess::StateMachineNames_var cret = msa->getStateMachines();
        for(CORBA::ULong i= 0; i != cret->length(); ++i)
            ret.push_back( string(cret[i]) );
        return ret;
    }

    bool ScriptingAccessProxy::hasStateMachine(string name ) const {
        return msa->hasStateMachine( name.c_str() );
    }
    
    string ScriptingAccessProxy::getStateMachineText(string name ) const {
        return msa->getStateMachineText( name.c_str() );
    }
    
    string ScriptingAccessProxy::getStateMachineStatus(string name ) const {
        return msa->getStateMachineStatus( name.c_str() );
    }

    int ScriptingAccessProxy::getStateMachineLine(string name ) const {
        return msa->getStateMachineLine( name.c_str() );
    }
    
    string ScriptingAccessProxy::getCurrentState( string name ) const {
        return msa->getCurrentState( name.c_str() );
    }
}
