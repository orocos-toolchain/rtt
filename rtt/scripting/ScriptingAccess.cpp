/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  ScriptingAccess.cxx

                        ScriptingAccess.cxx -  description
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



#include "ScriptingAccess.hpp"
#include "../Logger.hpp"
#include "../TaskContext.hpp"
#include <algorithm>
#include <functional>
#include <fstream>
#include <iterator>
#include "rtt-config.h"
#include "ProgramExceptions.hpp"
#include "StatementProcessor.hpp"
#include "../internal/TaskObject.hpp"
#include "Parser.hpp"
#include "parse_exception.hpp"
#include "../Method.hpp"

namespace RTT {
    using namespace detail;
    using namespace std;

    ScriptingAccess::ScriptingAccess( TaskContext* parent )
        : TaskObject("scripting","Access to the Scripting interface. \
                Use this object in order to load or query programs or state machines."),
        mparent(parent),sproc(0)
    {
        this->createInterface();
    }

    ScriptingAccess::~ScriptingAccess()
    {
        mparent->removeObject("scripting");
        this->clear();
        delete sproc;
    }

    void ScriptingAccess::clear() {
        while ( !states.empty() ) {
            // try to unload all
            Logger::log() << Logger::Info << "ScriptingAccess unloads StateMachine "<< states.begin()->first << "..."<<Logger::endl;
#ifndef ORO_EMBEDDED
            try {
                this->unloadStateMachine( states.begin()->first );
            }
            catch ( program_load_exception& ple) {
                Logger::log() << Logger::Error << ple.what() <<Logger::endl;
                states.erase( states.begin() ); // plainly remove it to avoid endless loop.
            }
#else
            if (this->unloadStateMachine( states.begin()->first ) == false) {
                Logger::log() << Logger::Error << "Error during unload !" <<Logger::endl;
                states.erase( states.begin() ); // plainly remove it to avoid endless loop.
            }
#endif
        }
        while ( !programs.empty() ) {
            // try to unload all
            Logger::log() << Logger::Info << "ScriptingAccess unloads Program "<< programs.begin()->first << "..."<<Logger::endl;
#ifndef ORO_EMBEDDED
            try {
                this->unloadProgram( programs.begin()->first );
            }
            catch ( program_load_exception& ple) {
                Logger::log() << Logger::Error << ple.what() <<Logger::endl;
                programs.erase( programs.begin() ); // plainly remove it to avoid endless loop.
            }
#else
            if (this->unloadProgram( programs.begin()->first ) == false) {
                Logger::log(Error) << "Error during unload !" <<Logger::endl;
                programs.erase( programs.begin() ); // plainly remove it to avoid endless loop.
            }
#endif
        }
    }

     StateMachine::Status::StateMachineStatus ScriptingAccess::getStateMachineStatus(const string& name) const
     {
         StateMapIt it = states.find(name);
         if ( it != states.end() ) {
             return it->second->getStatus();
         }
         return StateMachineStatus::unloaded;
     }

     string ScriptingAccess::getStateMachineStatusStr(const string& name) const
     {
        switch ( getStateMachineStatus( name ))
            {
            case StateMachineStatus::inactive:
                return "inactive";
                break;
            case StateMachineStatus::stopping:
                return "stopping";
                break;
            case StateMachineStatus::stopped:
                return "stopped";
                break;
            case StateMachineStatus::requesting:
                return "requesting";
                break;
            case StateMachineStatus::running:
                return "running";
                break;
            case StateMachineStatus::paused:
                return "paused";
                break;
            case StateMachineStatus::active:
                return "active";
                break;
            case StateMachineStatus::activating:
                return "activating";
                break;
            case StateMachineStatus::deactivating:
                return "deactivating";
                break;
            case StateMachineStatus::resetting:
                return "resetting";
                break;
            case StateMachineStatus::error:
                return "error";
                break;
            case StateMachineStatus::unloaded:
                return "unloaded";
                break;
            }
        return "na";
     }

    bool ScriptingAccess::loadStateMachine( StateMachinePtr sc )
    {
        // test if parent ...
        if ( sc->getParent() ) {
            string error(
                "Could not register StateMachine \"" + sc->getName() +
                "\" with the processor. It is not a root StateMachine." );
            ORO_THROW_OR_RETURN( program_load_exception( error ) , false);
        }

        if (this->recursiveCheckLoadStateMachine( sc ) == false)
            return false; // throws load_exception
        this->recursiveLoadStateMachine( sc );
        return true;
    }

    bool ScriptingAccess::recursiveCheckLoadStateMachine( StateMachinePtr sc )
    {
        // test if already present..., this cannot detect corrupt
        // trees with double names...
        if ( states.find(sc->getName()) != states.end() ) {
            string error(
                "Could not register StateMachine \"" + sc->getName() +
                "\" with the processor. A StateMachine with that name is already present." );
            ORO_THROW_OR_RETURN( program_load_exception( error ), false );

            vector<StateMachinePtr>::const_iterator it2;
            for (it2 = sc->getChildren().begin(); it2 != sc->getChildren().end(); ++it2)
                {
                    if ( this->recursiveCheckLoadStateMachine( *it2 ) == false)
                        return false;
                }
        }
        return true;
    }

    void ScriptingAccess::recursiveLoadStateMachine( StateMachinePtr sc )
    {
        vector<StateMachinePtr>::const_iterator it;

        // first load parent.
        states[sc->getName()] = sc;
        mparent->engine()->runFunction( sc.get() );

        // then load children.
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveLoadStateMachine( *it );
            }

    }

    bool ScriptingAccess::unloadStateMachine( const string& name )
    {
        StateMapIt it = states.find(name);

        if ( it != states.end() ) {
            // test if parent ...
            if ( it->second->getParent() ) {
                string error(
                                  "Could not unload StateMachine \"" + it->first +
                                  "\" with the processor. It is not a root StateMachine." );
                ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
            }
            if (recursiveCheckUnloadStateMachine( it->second ) == false)
                return false;
            recursiveUnloadStateMachine( it->second );
            return true;
        }
        return false;
    }

    bool ScriptingAccess::recursiveCheckUnloadStateMachine(StateMachinePtr si)
    {
        // check this state
        if ( si->isActive() ) {
            string error(
                              "Could not unload StateMachine \"" + si->getName() +
                              "\" with the processor. It is still active, status is "+
                    this->getStateMachineStatusStr( si->getName() ) );
            ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
        }

        // check children
        vector<StateMachinePtr>::const_iterator it2;
        for (it2 = si->getChildren().begin();
             it2 != si->getChildren().end();
             ++it2)
        {
            StateMapIt it = states.find( (*it2)->getName() );
            if ( it != states.end() ) {
                string error(
                                  "Could not unload StateMachine \"" + si->getName() +
                                  "\" with the processor. It contains not loaded child "+ (*it2)->getName() );
                ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
            }
            // all is ok, check child :
            if ( this->recursiveCheckUnloadStateMachine( it->second ) == false)
                return false;
        }
        return true;
    }

    void ScriptingAccess::recursiveUnloadStateMachine(StateMachinePtr sc) {
        // first erase children
        for (vector<StateMachinePtr>::const_iterator it = sc->getChildren().begin();
             it != sc->getChildren().end(); ++it)
        {
            this->recursiveUnloadStateMachine( *it );
        }

        // erase this sc :
        StateMap::iterator it = states.find( sc->getName() );

        assert( it != states.end() ); // we checked that this is possible

        // lastly, unload the parent.
        states.erase(it);
        mparent->engine()->removeFunction( sc.get() );
    }

    bool ScriptingAccess::deleteStateMachine(const string& name)
    {
        return this->unloadStateMachine(name);
    }

    const StateMachinePtr ScriptingAccess::getStateMachine(const string& name) const
    {
        StateMapIt it = states.find(name);
        return it == states.end() ? StateMachinePtr() : it->second;
    }

    StateMachinePtr ScriptingAccess::getStateMachine(const string& name)
    {
        StateMapIt it = states.find(name);
        return it == states.end() ? StateMachinePtr() : it->second;
    }

    vector<string> ScriptingAccess::getStateMachineList() const
    {
        return keys(states);
    }

    ProgramInterface::Status::ProgramStatus ScriptingAccess::getProgramStatus(const string& name) const
    {
        ProgMapIt it = programs.find(name);

        if ( it != programs.end() )
            return it->second->getStatus();
        return ProgramStatus::unknown;
    }

    string ScriptingAccess::getProgramStatusStr(const string& name) const
    {
       switch ( getProgramStatus( name ))
           {
           case ProgramStatus::unknown:
               return "unknown";
               break;
           case ProgramStatus::stopped:
               return "stopped";
               break;
           case ProgramStatus::running:
               return "running";
               break;
           case ProgramStatus::paused:
               return "paused";
               break;
           case ProgramStatus::error:
               return "error";
               break;
           }
       return "na";
    }

   bool ScriptingAccess::loadProgram(ProgramInterfacePtr pi)
   {
       if ( programs.find(pi->getName()) == programs.end() )
           return false;
       programs[pi->getName()] = pi;
       pi->reset();
       mparent->engine()->runFunction( pi.get() );
       return true;
   }

   bool ScriptingAccess::unloadProgram(const string& name)
   {
       ProgMap::iterator it = programs.find(name);

       if ( it != programs.end() )
           {
               mparent->engine()->removeFunction( it->second.get() );
               programs.erase( it );
               return true;
           }
       string error("Could not unload Program \"" + name +
                         "\" with the processor. It does not exist." );
       ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
   }

   vector<string> ScriptingAccess::getProgramList() const
   {
       return keys(programs);
   }

   const ProgramInterfacePtr ScriptingAccess::getProgram(const string& name) const
   {
       ProgMapIt it = programs.find(name);
       return it == programs.end() ? ProgramInterfacePtr() : it->second;
   }

   ProgramInterfacePtr ScriptingAccess::getProgram(const string& name)
   {
       ProgMapIt it = programs.find(name);
       return it == programs.end() ? ProgramInterfacePtr() : it->second;
   }


    bool ScriptingAccess::doExecute(const string& code)
    {
        return this->execute(code) >= 0;
    }

    bool ScriptingAccess::doLoadPrograms( const string& filename )
    {
        return this->loadPrograms(filename, false);
    }

    bool ScriptingAccess::doLoadProgramText( const string& code )
    {
        return this->loadPrograms(code, "string", false);
    }
    bool ScriptingAccess::doUnloadProgram( const string& name )
    {
        return this->unloadProgram(name, false);
    }

    bool ScriptingAccess::doLoadStateMachines( const string& filename )
    {
        return this->loadStateMachines(filename, false);
    }
    bool ScriptingAccess::doLoadStateMachineText( const string& code )
    {
        return this->loadStateMachines(code, "string", false);
    }
    bool ScriptingAccess::doUnloadStateMachine( const string& name )
    {
        return this->unloadStateMachine(name, false);
    }

    void ScriptingAccess::createInterface()
    {
        methods()->addMethod( method( "execute", &ScriptingAccess::execute, this),
                                   "Execute a line of code.", "Code", "A single statement.");
        // Methods for loading programs
        methods()->addMethod( method( "loadPrograms", &ScriptingAccess::doLoadPrograms, this),
                                   "Load a program from a given file.", "Filename", "The filename of the script."  );
        methods()->addMethod( method( "loadProgramText", &ScriptingAccess::doLoadProgramText, this),
                                   "Load a program from a string.", "Code", "A string containing one or more program scripts." );
        methods()->addMethod( method( "unloadProgram", &ScriptingAccess::doUnloadProgram, this),
                                   "Remove a loaded program.", "Name", "The name of the loaded Program"  );

        // Query Methods for programs
        methods()->addMethod( method( "getProgramStatus", &ScriptingAccess::getProgramStatus, this),
                                            "Get the status of a program?", "Name", "The Name of the loaded Program"  );
        methods()->addMethod( method( "getProgramLine", &ScriptingAccess::getProgramLine, this),
                                            "Get the current line of execution of a program?", "Name", "The Name of the loaded Program"  );

        // Methods for loading state machines
        methods()->addMethod( method( "loadStateMachines", &ScriptingAccess::doLoadStateMachines, this),
                                   "Load a state machine from a given file.", "Filename", "The filename of the script."  );
        methods()->addMethod( method( "loadStateMachineText", &ScriptingAccess::doLoadStateMachineText, this),
                                   "Load a state machine from a string.", "Code", "A string containing one or more state machine scripts." );
        methods()->addMethod( method( "unloadStateMachine", &ScriptingAccess::doUnloadStateMachine, this),
                                   "Remove a loaded state machine.", "Name", "The name of the loaded State Machine"  );

        // Query Methods for state machines
        methods()->addMethod( method( "getStateMachineStatus", &ScriptingAccess::getStateMachineStatus, this),
                                            "Get the status of a state machine?", "Name", "The Name of the loaded State Machine"  );
        methods()->addMethod( method( "getStateMachineLine", &ScriptingAccess::getStateMachineLine, this),
                                            "Get the current line of execution of a state machine?", "Name", "The Name of the loaded State Machine"  );

        // Query Methods for programs
        methods()->addMethod( method( "hasProgram", &ScriptingAccess::hasProgram, this),
                                            "Is a program loaded?", "Name", "The Name of the loaded Program"  );
        methods()->addMethod( method( "isProgramRunning", &ScriptingAccess::isProgramRunning, this),
                                            "Is a program running ?", "Name", "The Name of the Loaded Program"  );
        methods()->addMethod( method( "isProgramPaused", &ScriptingAccess::isProgramPaused, this),
                                            "Is a program paused ?", "Name", "The Name of the Loaded Program"  );
        methods()->addMethod( method( "inProgramError", &ScriptingAccess::inProgramError, this),
                                            "Is a program in error ?", "Name", "The Name of the Loaded Program"  );

        // Query Methods for state machines
        methods()->addMethod( method( "hasStateMachine", &ScriptingAccess::hasStateMachine, this),
                                            "Is a state machine loaded?", "Name", "The Name of the loaded State Machine"  );
        methods()->addMethod( method( "isStateMachineActive", &ScriptingAccess::isStateMachineActive, this),
                                            "Is a state machine active ?", "Name", "The Name of the Loaded StateMachine"  );
        methods()->addMethod( method( "isStateMachineRunning", &ScriptingAccess::isStateMachineRunning, this),
                                            "Is a state machine running ?", "Name", "The Name of the Loaded StateMachine"  );
        methods()->addMethod( method( "isStateMachinePaused", &ScriptingAccess::isStateMachinePaused, this),
                                            "Is a state machine paused ?", "Name", "The Name of the Loaded StateMachine"  );
        methods()->addMethod( method( "inStateMachineError", &ScriptingAccess::inStateMachineError, this),
                                            "Is a state machine in error ?", "Name", "The Name of the Loaded StateMachine"  );
        methods()->addMethod( method( "inStateMachineState", &ScriptingAccess::inStateMachineState, this),
                                         "Is a state machine in a given state ?",
                                         "Name", "The Name of the Loaded StateMachine",
                                         "State", "The name of the state in which it could be.");
        methods()->addMethod( method( "getStateMachineState", &ScriptingAccess::getStateMachineState, this),
                                         "Get the current state name of a state machine.",
                                         "Name", "The Name of the Loaded StateMachine");

        // Methods for programs
        methods()->addMethod( method( "startProgram", &ScriptingAccess::startProgram, this),
                    "Start a program", "Name", "The Name of the Loaded Program"  );
        methods()->addMethod( method( "stopProgram", &ScriptingAccess::stopProgram ,this),
                                           "Stop a program", "Name", "The Name of the Started Program" );

        methods()->addMethod( method( "stepProgram", &ScriptingAccess::stepProgram ,this),
                    "Step a single program instruction", "Name", "The Name of the Paused Program"  );
        methods()->addMethod( method( "pauseProgram", &ScriptingAccess::pauseProgram ,this),
                    "Pause a program", "Name", "The Name of the Started Program"  );

        // Methods for state machines
        // Activate/deactivate:
        methods()->addMethod( method( "activateStateMachine", &ScriptingAccess::activateStateMachine ,this),
                    "Activate a StateMachine", "Name", "The Name of the Loaded StateMachine"  );
        methods()->addMethod( method( "deactivateStateMachine", &ScriptingAccess::deactivateStateMachine ,this),
                    "Deactivate a StateMachine", "Name", "The Name of the Stopped StateMachine"  );

        // start/stop/pause:
        methods()->addMethod( method( "startStateMachine", &ScriptingAccess::startStateMachine ,this),
                    "Start a StateMachine", "Name", "The Name of the Activated/Paused StateMachine"  );
        methods()->addMethod( method( "pauseStateMachine", &ScriptingAccess::pauseStateMachine ,this),
                    "Pause a StateMachine", "Name", "The Name of a Started StateMachine"  );
        methods()->addMethod( method( "stopStateMachine", &ScriptingAccess::stopStateMachine ,this),
                    "Stop a StateMachine", "Name", "The Name of the Started/Paused StateMachine"  );
        methods()->addMethod( method( "resetStateMachine", &ScriptingAccess::resetStateMachine ,this),
                    "Reset a StateMachine", "Name", "The Name of the Stopped StateMachine" );

        // request states
        methods()->addMethod( method( "requestStateMachineState", &ScriptingAccess::requestStateMachineState ,this),
                                           "Request a State change",
                                           "Name", "The Name of the StateMachine",
                                           "StateName", "The Name of the State to change to" );
        mparent->addObject(this);
    }

    int ScriptingAccess::execute(const string& code ){
        if (sproc == 0)
            sproc = new StatementProcessor(mparent);
        return sproc->execute( code );
    }

    DispatchInterface::shared_ptr ScriptingAccess::getCommand( int ticket ){
        if (sproc)
            return sproc->getCommand(ticket);
        return DispatchInterface::shared_ptr();
    }

    ScriptingAccess::Functions  ScriptingAccess::loadFunctions( const string& file, bool do_throw/* = false*/ )
    {
      ifstream inputfile(file.c_str());
      if ( !inputfile ) {
          Logger::In in("ScriptingAccess::loadFunctions");
          Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
          return Functions();
      }
      string text;
      inputfile.unsetf( ios_base::skipws );
      istream_iterator<char> streambegin( inputfile );
      istream_iterator<char> streamend;
      copy( streambegin, streamend, back_inserter( text ) );
      return this->loadFunctions( text, file, do_throw );
    }

    ScriptingAccess::Functions  ScriptingAccess::loadFunctions( const string& code, const string& filename, bool mrethrow )
    {

      Logger::In in("ScriptingAccess::loadFunctions");
      Parser p;
      Functions exec;
      Functions ret;
      try {
          Logger::log() << Logger::Info << "Parsing file "<<filename << Logger::endl;
          ret = p.parseFunction(code, mparent, filename);
      }
      catch( const file_parse_exception& exc )
          {
#ifndef ORO_EMBEDDED
              Logger::log() << Logger::Error << filename<<" :"<< exc.what() << Logger::endl;
              if ( mrethrow )
                  throw;
#endif
              return Functions();
          }
      if ( ret.empty() )
          {
              Logger::log() << Logger::Debug << "No Functions executed from "<< filename << Logger::endl;
              Logger::log() << Logger::Info << filename <<" : Successfully parsed." << Logger::endl;
              return Functions();
          } else {
              // Load all listed functions in the TaskContext's Processor:
              for( Parser::ParsedFunctions::iterator it = ret.begin(); it != ret.end(); ++it) {
                  Logger::log() << "Queueing Function "<< (*it)->getName() << Logger::endl;
                  if ( mparent->engine()->runFunction( it->get() ) == false) {
                      Logger::log() << Logger::Error << "Could not run Function '"<< (*it)->getName() <<"' :" << Logger::nl;
                      Logger::log() << "Processor not accepting or function queue is full." << Logger::endl;
                  } else
                      exec.push_back( *it ); // is being executed.
              }
          }
      return exec;

    }

    bool ScriptingAccess::loadPrograms( const string& file, bool do_throw /*= false*/ )
    {
        ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ScriptingAccess::loadProgram");
            Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
            return false;
        }
        string text;
        inputfile.unsetf( ios_base::skipws );
        istream_iterator<char> streambegin( inputfile );
        istream_iterator<char> streamend;
        copy( streambegin, streamend, back_inserter( text ) );
        return this->loadPrograms( text, file, do_throw );
    }

    bool ScriptingAccess::loadPrograms( const string& code, const string& filename, bool mrethrow ){

      Logger::In in("ProgramLoader::loadProgram");
      Parser parser;
      Parser::ParsedPrograms pg_list;
      try {
          Logger::log() << Logger::Info << "Parsing file "<<filename << Logger::endl;
          pg_list = parser.parseProgram(code, mparent, filename );
      }
      catch( const file_parse_exception& exc )
          {
#ifndef ORO_EMBEDDED
              Logger::log() << Logger::Error <<filename<<" :"<< exc.what() << Logger::endl;
              if ( mrethrow )
                  throw;
#endif
              return false;
          }
      if ( pg_list.empty() )
          {
              Logger::log() << Logger::Info << filename <<" : Successfully parsed." << Logger::endl;
              return true;
          } else {
              // Load all listed programs in the TaskContext's Processor:
              bool error = false;
              string errors;
              for( Parser::ParsedPrograms::iterator it = pg_list.begin(); it != pg_list.end(); ++it) {
                  try {
                      Logger::log() << Logger::Info << "Loading Program '"<< (*it)->getName() <<"'" <<Logger::endl;
                      if (this->loadProgram( *it ) == false)
                          error = true;
                  } catch (program_load_exception& e ) {
                      Logger::log() << Logger::Error << "Could not load Program '"<< (*it)->getName() <<"' :" << Logger::nl;
#ifndef ORO_EMBEDDED
                      Logger::log() << e.what() << Logger::endl;
                      if ( mrethrow )
                          errors += "Could not load Program '"+ (*it)->getName() +"' :\n"+e.what()+'\n';
#endif
                      error = true;
                  }
              }
#ifndef ORO_EMBEDDED
              if (error && mrethrow )
                  throw program_load_exception( errors );
#endif
              return !error;
          }
      // never reached
    }

    bool ScriptingAccess::unloadProgram( const string& name, bool do_throw ){
        Logger::In in("ScriptingAccess::unloadProgram");
        try {
            Logger::log() << Logger::Info << "Unloading Program '"<< name <<"'"<< Logger::endl;
            if (this->unloadProgram(name) == false)
                return false;
        } catch (program_unload_exception& e ) {
            Logger::log() << Logger::Error << "Could not unload Program '"<< name <<"' :" << Logger::nl;
#ifndef ORO_EMBEDDED
            Logger::log() << e.what() << Logger::endl;
            if ( do_throw )
                throw;
#endif
            return false;
        }
        return true;
    }


    bool ScriptingAccess::loadStateMachines( const string& file, bool do_throw /*= false*/  )
    {
        ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ScriptingAccess::loadStateMachine");
          Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
          return false;
        }
        string text;
        inputfile.unsetf( ios_base::skipws );
        istream_iterator<char> streambegin( inputfile );
        istream_iterator<char> streamend;
        copy( streambegin, streamend, back_inserter( text ) );
      return this->loadStateMachines( text, file, do_throw );
    }

    bool ScriptingAccess::loadStateMachines( const string& code, const string& filename, bool mrethrow )
    {
        Logger::In in("ScriptingAccess::loadStateMachine");
        Parser parser;
        Parser::ParsedStateMachines pg_list;
        try {
            Logger::log() << Logger::Info << "Parsing file "<<filename << Logger::endl;
            pg_list = parser.parseStateMachine( code, mparent, filename );
        }
        catch( const file_parse_exception& exc )
            {
#ifndef ORO_EMBEDDED
                Logger::log() << Logger::Error <<filename<<" :"<< exc.what() << Logger::endl;
                if ( mrethrow )
                    throw;
#endif
                return false;
            }
        if ( pg_list.empty() )
            {
                Logger::log() << Logger::Error << "No StateMachines instantiated in "<< filename << Logger::endl;
                return false;
            } else {
                bool error = false;
                string errors;
                // Load all listed stateMachines in the TaskContext's Processor:
                for( Parser::ParsedStateMachines::iterator it = pg_list.begin(); it != pg_list.end(); ++it) {
                    try {
                        Logger::log() << Logger::Info << "Loading StateMachine '"<< (*it)->getName()<<"'" << Logger::endl;
                        if (this->loadStateMachine( *it ) == false)
                            return false;
                    } catch (program_load_exception& e ) {
                        Logger::log() << Logger::Error << "Could not load StateMachine '"<< (*it)->getName()<<"' :" << Logger::nl;
#ifndef ORO_EMBEDDED
                        Logger::log() << e.what() << Logger::endl;
                        if ( mrethrow )
                            errors += "Could not load Program '"+ (*it)->getName() +"' :\n"+e.what()+'\n';
#endif
                        error = true;
                    }
                }
#ifndef ORO_EMBEDDED
                if ( error && mrethrow )
                    throw program_load_exception( errors );
#endif
                return !error;
            }
        // never reached
        return false;
    }

    bool ScriptingAccess::unloadStateMachine( const string& name, bool do_throw ) {
        Logger::In in("ScriptingAccess::unloadStateMachine");
        try {
            Logger::log() << Logger::Info << "Unloading StateMachine '"<< name <<"'"<< Logger::endl;
            if (this->unloadStateMachine(name) == false)
                return false;
        } catch (program_unload_exception& e ) {
            Logger::log() << Logger::Error << "Could not unload StateMachine '"<< name <<"' :" << Logger::nl;
#ifndef ORO_EMBEDDED
            Logger::log() << e.what() << Logger::endl;
            if ( do_throw )
                throw;
#endif
            return false;
        }
        return true;
    }

    bool ScriptingAccess::hasProgram(const string& name) const {
        return programs.find(name) != programs.end();
    }

    int ScriptingAccess::getProgramLine(const string& name) const {
        const ProgramInterfacePtr pi = getProgram(name);
        return pi ? pi->getLineNumber() : -1;
    }

    string ScriptingAccess::getProgramText(const string& name ) const {
        const ProgramInterfacePtr pi = getProgram(name);
        return pi ? pi->getText() : "";
    }

    bool ScriptingAccess::hasStateMachine(const string& name) const {
        return states.find(name) != states.end();
    }

    string ScriptingAccess::getStateMachineText(const string& name ) const {
        const StateMachinePtr sm = getStateMachine(name);
        return sm ? sm->getText() : "";
    }

    int ScriptingAccess::getStateMachineLine(const string& name ) const {
        const StateMachinePtr sm = getStateMachine(name);
        return sm ? sm->getLineNumber() : -1;
    }

    string ScriptingAccess::getCurrentState( const string& name ) const {
        const StateMachinePtr sm = getStateMachine(name);
        return sm ? sm->getCurrentStateName() : "";
    }

    bool ScriptingAccess::startProgram(const string& name)
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->start();
        return false;
    }

    bool ScriptingAccess::isProgramRunning(const string& name) const
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->isRunning();
        return false;
    }

    bool ScriptingAccess::isProgramPaused(const string& name) const
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->isPaused();
        return false;
    }

    bool ScriptingAccess::inProgramError(const string& name) const
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->inError();
        return false;
    }

    bool ScriptingAccess::stopProgram(const string& name)
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->stop();
        return false;
    }

    bool ScriptingAccess::pauseProgram(const string& name)
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->pause();
        return false;
    }

    bool ScriptingAccess::stepProgram(const string& name)
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->step();
        return false;
    }

    bool ScriptingAccess::activateStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->activate();
        return false;
    }

    bool ScriptingAccess::deactivateStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->deactivate();
        return false;
    }

    bool ScriptingAccess::startStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->start();
        return false;
    }

    bool ScriptingAccess::pauseStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->pause();
        return false;
    }

    bool ScriptingAccess::stopStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->stop();
        return false;
    }

    bool ScriptingAccess::isStateMachinePaused(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->isPaused();
        return false;
    }

    bool ScriptingAccess::isStateMachineActive(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->isActive();
        return false;
    }

    bool ScriptingAccess::isStateMachineRunning(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->isAutomatic();
        return false;
    }

    bool ScriptingAccess::inStateMachineError(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->inError();
        return false;
    }

    const string& ScriptingAccess::getStateMachineState(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->getCurrentStateName();
        return NA<const string&>::na();
    }

    bool ScriptingAccess::requestStateMachineState(const string& name, const string& state)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->requestState(state);
        return false;
    }

    bool ScriptingAccess::inStateMachineState(const string& name, const string& state) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->inState(state);
        return false;
    }

    bool ScriptingAccess::resetStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->reset();
        return false;
    }

}
