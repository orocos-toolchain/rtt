/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ScriptingAccessProxy.cxx 

                        ScriptingAccessProxy.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 

#include "ScriptingAccessProxy.hpp"
#include "ScriptingAccessC.h"
#include "ProgramExceptions.hpp"
#include "CommandProxy.hpp"

namespace RTT
{namespace Corba
{
    using namespace std;
    

    ScriptingAccessProxy::~ScriptingAccessProxy()
    {
    }

    int ScriptingAccessProxy::execute( const string& code ) {
        return msa->execute( code.c_str() );
    }

    CommandC ScriptingAccessProxy::getCommand( int ticket ) {
        Command_var result = msa->getCommand(ticket);
        if ( CORBA::is_nil(result) )
            return CommandC();
        return CommandC( CommandProxy::Create( result.in() ) );
    }

    bool ScriptingAccessProxy::loadPrograms( string code, string filename, bool do_throw ) {
        try {
            msa->loadPrograms( code.c_str(), filename.c_str(), true );
        } catch (Corba::ScriptingAccess::LoadException& le)
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
            msa->unloadProgram( name.c_str(), true  );
        } catch (Corba::ScriptingAccess::LoadException& le)
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
        Corba::ScriptingAccess::ProgramNames_var cret = msa->getPrograms();
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
            msa->loadStateMachines( code.c_str(), filename.c_str(), true  );
        } catch (Corba::ScriptingAccess::LoadException& le)
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
            msa->unloadStateMachine( name.c_str(), true  );
        } catch (Corba::ScriptingAccess::LoadException& le)
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
        Corba::ScriptingAccess::StateMachineNames_var cret = msa->getStateMachines();
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
}}
