/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  ScriptingService.cxx

                        ScriptingService.cxx -  description
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



#include "ScriptingService.hpp"
#include "../Logger.hpp"
#include "../TaskContext.hpp"
#include <algorithm>
#include <functional>
#include <fstream>
#include <iterator>
#include "scripting/rtt-scripting-config.h"
#include "ProgramExceptions.hpp"
#include "StatementProcessor.hpp"
#include "../Service.hpp"
#include "Parser.hpp"
#include "parse_exception.hpp"
#include "../OperationCaller.hpp"
#include "../internal/mystd.hpp"
#include "../plugin/ServicePlugin.hpp"
#include "../internal/GlobalEngine.hpp"

ORO_SERVICE_NAMED_PLUGIN( RTT::scripting::ScriptingService, "scripting" )


namespace RTT {
    using namespace detail;
    using namespace std;

    ScriptingService::shared_ptr ScriptingService::Create(TaskContext* parent){
        shared_ptr sp(new ScriptingService(parent));
        parent->provides()->addService( sp );
        return sp;
    }

    ScriptingService::ScriptingService( TaskContext* parent )
        : Service("scripting", parent),
          sproc(0)
    {
        this->doc("Orocos Scripting service. Use this service in order to load or query programs or state machines.");
        this->createInterface();
        ZeroPeriodWarning = true;
        this->addProperty("ZeroPeriodWarning",ZeroPeriodWarning)
			.doc("If this is set to false, the warning log when loading a program or a state machine into a Component"
					" with a null period will not be printed. Be sure you have something else triggering periodically"
					" your Component activity unless your script may not work.");
    }

    ScriptingService::~ScriptingService()
    {
        // since we are refcounted, we don't need to inform the owner
        // that we're being deleted.
        this->clear();
        delete sproc;
    }

    void ScriptingService::clear() {
        while ( !states.empty() ) {
            // try to unload all
            Logger::log() << Logger::Info << "ScriptingService unloads StateMachine "<< states.begin()->first << "..."<<Logger::endl;
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
            Logger::log() << Logger::Info << "ScriptingService unloads Program "<< programs.begin()->first << "..."<<Logger::endl;
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
        Service::clear();
    }

     StateMachine::Status::StateMachineStatus ScriptingService::getStateMachineStatus(const string& name) const
     {
         StateMapIt it = states.find(name);
         if ( it != states.end() ) {
             return it->second->getStatus();
         }
         return StateMachineStatus::unloaded;
     }

     string ScriptingService::getStateMachineStatusStr(const string& name) const
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

    bool ScriptingService::loadStateMachine( StateMachinePtr sc )
    {
        // test if parent ...
        if ( sc->getParent() ) {
            string error(
                "Could not register StateMachine \"" + sc->getName() +
                "\" in the ScriptingService. It is not a root StateMachine." );
            ORO_THROW_OR_RETURN( program_load_exception( error ) , false);
        }

        if (this->recursiveCheckLoadStateMachine( sc ) == false)
            return false; // throws load_exception

        if ( getOwner()->getPeriod() == 0 && ZeroPeriodWarning) {
            log(Warning) << "Loading StateMachine "<< sc->getName()
            << " in a TaskContext with getPeriod() == 0."
            << " Use setPeriod(period) in order to setup execution of scripts."
            << " If you know what you are doing, you may disable this warning using scripting.ZeroPeriodWarning=false"
            <<endlog();
        }

        this->recursiveLoadStateMachine( sc );
        return true;
    }

    bool ScriptingService::recursiveCheckLoadStateMachine( StateMachinePtr sc )
    {
        // test if already present..., this cannot detect corrupt
        // trees with double names...
        if ( states.find(sc->getName()) != states.end() ) {
            string error(
                "Could not register StateMachine \"" + sc->getName() +
                "\" in the ScriptingService. A StateMachine with that name is already present." );
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

    void ScriptingService::recursiveLoadStateMachine( StateMachinePtr sc )
    {
        vector<StateMachinePtr>::const_iterator it;

        // first load parent.
        states[sc->getName()] = sc;
        mowner->engine()->runFunction( sc.get() );

        // then load children.
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveLoadStateMachine( *it );
            }

    }

    bool ScriptingService::unloadStateMachine( const string& name )
    {
        StateMapIt it = states.find(name);

        if ( it != states.end() ) {
            // test if parent ...
            if ( it->second->getParent() ) {
                string error(
                                  "Could not unload StateMachine \"" + it->first +
                                  "\" in the ScriptingService. It is not a root StateMachine." );
                ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
            }
            if (recursiveCheckUnloadStateMachine( it->second ) == false)
                return false;
            recursiveUnloadStateMachine( it->second );
            return true;
        }
        return false;
    }

    bool ScriptingService::recursiveCheckUnloadStateMachine(StateMachinePtr si)
    {
        // check children
        vector<StateMachinePtr>::const_iterator it2;
        for (it2 = si->getChildren().begin();
             it2 != si->getChildren().end();
             ++it2)
        {
            StateMapIt it = states.find( (*it2)->getName() );
            if ( it == states.end() ) {
                string error(
                                  "Could not unload StateMachine \"" + si->getName() +
                                  "\" in the ScriptingService. It contains not loaded child "+ (*it2)->getName() );
                ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
            }
            // all is ok, check child :
            if ( this->recursiveCheckUnloadStateMachine( it->second ) == false)
                return false;
        }
        return true;
    }

    void ScriptingService::recursiveUnloadStateMachine(StateMachinePtr sc) {
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
        mowner->engine()->removeFunction( sc.get() );
    }

    bool ScriptingService::deleteStateMachine(const string& name)
    {
        return this->unloadStateMachine(name);
    }

    const StateMachinePtr ScriptingService::getStateMachine(const string& name) const
    {
        StateMapIt it = states.find(name);
        return it == states.end() ? StateMachinePtr() : it->second;
    }

    StateMachinePtr ScriptingService::getStateMachine(const string& name)
    {
        StateMapIt it = states.find(name);
        return it == states.end() ? StateMachinePtr() : it->second;
    }

    vector<string> ScriptingService::getStateMachineList() const
    {
        return keys(states);
    }

    ProgramInterface::Status::ProgramStatus ScriptingService::getProgramStatus(const string& name) const
    {
        ProgMapIt it = programs.find(name);

        if ( it != programs.end() )
            return it->second->getStatus();
        return ProgramStatus::unknown;
    }

    string ScriptingService::getProgramStatusStr(const string& name) const
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

   bool ScriptingService::loadProgram(ProgramInterfacePtr pi)
   {
       if ( programs.find(pi->getName()) != programs.end() ) {
           log(Error) << "Could not load Program "<< pi->getName() << " in ScriptingService: name already in use."<<endlog();
           return false;
       }
       if ( getOwner()->getPeriod() == 0 && ZeroPeriodWarning ) {
           log(Warning) << "Loading program " << pi->getName()
			   << " in a TaskContext with getPeriod() == 0."
			   << " Use setPeriod(period) in order to setup execution of scripts."
			   << " If you know what you are doing, you may disable this warning using scripting.ZeroPeriodWarning=false"
			   << endlog();
       }
       programs[pi->getName()] = pi;
       pi->reset();
       if ( mowner->engine()->runFunction( pi.get() ) == false) {
           programs.erase(pi->getName());
           log(Error) << "Could not load Program "<< pi->getName() << " in ExecutionEngine."<<endlog();
           return false;
       }
       return true;
   }

   bool ScriptingService::unloadProgram(const string& name)
   {
       ProgMap::iterator it = programs.find(name);

       if ( it != programs.end() )
           {
               mowner->engine()->removeFunction( it->second.get() );
               programs.erase( it );
               return true;
           }
       string error("Could not unload Program \"" + name +
                         "\" in the ScriptingService. It does not exist." );
       ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
   }

   vector<string> ScriptingService::getProgramList() const
   {
       return keys(programs);
   }

   const ProgramInterfacePtr ScriptingService::getProgram(const string& name) const
   {
       ProgMapIt it = programs.find(name);
       return it == programs.end() ? ProgramInterfacePtr() : it->second;
   }

   ProgramInterfacePtr ScriptingService::getProgram(const string& name)
   {
       ProgMapIt it = programs.find(name);
       return it == programs.end() ? ProgramInterfacePtr() : it->second;
   }


    bool ScriptingService::doExecute(const string& code)
    {
        return this->execute(code) >= 0;
    }

    bool ScriptingService::doLoadPrograms( const string& filename )
    {
        return this->loadPrograms(filename, false);
    }

    bool ScriptingService::doLoadProgramText( const string& code )
    {
        return this->loadPrograms(code, "string", false);
    }
    bool ScriptingService::doUnloadProgram( const string& name )
    {
        return this->unloadProgram(name, false);
    }

    bool ScriptingService::doLoadStateMachines( const string& filename )
    {
        return this->loadStateMachines(filename, false);
    }
    bool ScriptingService::doLoadStateMachineText( const string& code )
    {
        return this->loadStateMachines(code, "string", false);
    }
    bool ScriptingService::doUnloadStateMachine( const string& name )
    {
        return this->unloadStateMachine(name, false);
    }

    void ScriptingService::createInterface()
    {
        // OperationCallers for loading and executing scripts
        addOperation("eval", &ScriptingService::eval, this).doc("Evaluate then script in the argument").arg("Code", "Statements, functions, program definitions etc.");
        addOperation("runScript", &ScriptingService::runScript, this).doc("Run a script from a given file.").arg("Filename", "The filename of the script.");

        addOperation("execute", &ScriptingService::execute, this).doc("Execute a line of code (DEPRECATED).").arg("Code", "A single statement.");
        // OperationCallers for loading programs
        addOperation("loadPrograms", &ScriptingService::doLoadPrograms, this).doc("Load a program from a given file.").arg("Filename", "The filename of the script.");
        addOperation("loadProgramText", &ScriptingService::doLoadProgramText, this).doc("Load a program from a string.").arg("Code", "A string containing one or more program scripts.");
        addOperation("unloadProgram", &ScriptingService::doUnloadProgram, this).doc("Remove a loaded program.").arg("Name", "The name of the loaded Program");

        // Query OperationCallers for programs
        addOperation("getProgramList", &ScriptingService::getProgramList, this).doc("Get a list of all loaded program scripts.");
        addOperation("getProgramStatus", &ScriptingService::getProgramStatus, this).doc("Get the status of a program?").arg("Name", "The Name of the loaded Program");
        addOperation("getProgramStatusStr", &ScriptingService::getProgramStatusStr, this).doc("Get the status of a program as a human readable string.").arg("Name", "The Name of the loaded Program");
        addOperation("getProgramLine", &ScriptingService::getProgramLine, this).doc("Get the current line of execution of a program?").arg("Name", "The Name of the loaded Program");
        addOperation("getProgramText", &ScriptingService::getProgramText, this).doc("Get the script of a program.").arg("Name", "The Name of the loaded Program");

        // OperationCallers for loading state machines
        addOperation("loadStateMachines", &ScriptingService::doLoadStateMachines, this).doc("Load a state machine from a given file.").arg("Filename", "The filename of the script.");
        addOperation("loadStateMachineText", &ScriptingService::doLoadStateMachineText, this).doc("Load a state machine from a string.").arg("Code", "A string containing one or more state machine scripts.");
        addOperation("unloadStateMachine", &ScriptingService::doUnloadStateMachine, this).doc("Remove a loaded state machine.").arg("Name", "The name of the loaded State Machine");

        // Query OperationCallers for state machines
        addOperation("getStateMachineList", &ScriptingService::getStateMachineList, this).doc("Get a list of all loaded state machines");
        addOperation("getStateMachineStatus", &ScriptingService::getStateMachineStatus, this).doc("Get the status of a state machine?").arg("Name", "The Name of the loaded State Machine");
        addOperation("getStateMachineStatusStr", &ScriptingService::getStateMachineStatusStr, this).doc("Get the status of a state machine as a human readable string.");
        addOperation("getStateMachineLine", &ScriptingService::getStateMachineLine, this).doc("Get the current line of execution of a state machine?").arg("Name", "The Name of the loaded State Machine");
        addOperation("getStateMachineText", &ScriptingService::getStateMachineText, this).doc("Get the script of a StateMachine.").arg("Name", "The Name of the loaded StateMachine");

        // Query OperationCallers for programs
        addOperation("hasProgram", &ScriptingService::hasProgram, this).doc("Is a program loaded?").arg("Name", "The Name of the loaded Program");
        addOperation("isProgramRunning", &ScriptingService::isProgramRunning, this).doc("Is a program running ?").arg("Name", "The Name of the Loaded Program");
        addOperation("isProgramPaused", &ScriptingService::isProgramPaused, this).doc("Is a program paused ?").arg("Name", "The Name of the Loaded Program");
        addOperation("inProgramError", &ScriptingService::inProgramError, this).doc("Is a program in error ?").arg("Name", "The Name of the Loaded Program");

        // Query OperationCallers for state machines
        addOperation("hasStateMachine", &ScriptingService::hasStateMachine, this).doc("Is a state machine loaded?").arg("Name", "The Name of the loaded State Machine");
        addOperation("isStateMachineActive", &ScriptingService::isStateMachineActive, this).doc("Is a state machine active ?").arg("Name", "The Name of the Loaded StateMachine");
        addOperation("isStateMachineRunning", &ScriptingService::isStateMachineRunning, this).doc("Is a state machine running ?").arg("Name", "The Name of the Loaded StateMachine");
        addOperation("isStateMachinePaused", &ScriptingService::isStateMachinePaused, this).doc("Is a state machine paused ?").arg("Name", "The Name of the Loaded StateMachine");
        addOperation("inStateMachineError", &ScriptingService::inStateMachineError, this).doc("Is a state machine in error ?").arg("Name", "The Name of the Loaded StateMachine");
        addOperation("inStateMachineState", &ScriptingService::inStateMachineState, this).doc("Is a state machine in a given state ?").arg("Name", "The Name of the Loaded StateMachine").arg("State", "The name of the state in which it could be.");
        addOperation("getStateMachineState", &ScriptingService::getStateMachineState, this).doc("Get the current state name of a state machine.").arg("Name", "The Name of the Loaded StateMachine");

        // OperationCallers for programs
        addOperation("startProgram", &ScriptingService::startProgram, this).doc("Start a program").arg("Name", "The Name of the Loaded Program");
        addOperation("stopProgram", &ScriptingService::stopProgram , this).doc("Stop a program").arg("Name", "The Name of the Started Program");

        addOperation("stepProgram", &ScriptingService::stepProgram , this).doc("Step a single program instruction").arg("Name", "The Name of the Paused Program");
        addOperation("pauseProgram", &ScriptingService::pauseProgram , this).doc("Pause a program").arg("Name", "The Name of the Started Program");

        // OperationCallers for state machines
        // Activate/deactivate:
        addOperation("activateStateMachine", &ScriptingService::activateStateMachine , this).doc("Activate a StateMachine").arg("Name", "The Name of the Loaded StateMachine");
        addOperation("deactivateStateMachine", &ScriptingService::deactivateStateMachine , this).doc("Deactivate a StateMachine").arg("Name", "The Name of the Stopped StateMachine");

        // start/stop/pause:
        addOperation("startStateMachine", &ScriptingService::startStateMachine , this).doc("Start a StateMachine").arg("Name", "The Name of the Activated/Paused StateMachine");
        addOperation("pauseStateMachine", &ScriptingService::pauseStateMachine , this).doc("Pause a StateMachine").arg("Name", "The Name of a Started StateMachine");
        addOperation("stopStateMachine", &ScriptingService::stopStateMachine , this).doc("Stop a StateMachine").arg("Name", "The Name of the Started/Paused StateMachine");
        addOperation("resetStateMachine", &ScriptingService::resetStateMachine , this).doc("Reset a StateMachine").arg("Name", "The Name of the Stopped StateMachine");

        // request states
        addOperation("requestStateMachineState", &ScriptingService::requestStateMachineState , this).doc("Request a State change").arg("Name", "The Name of the StateMachine").arg("StateName", "The Name of the State to change to");
    }

    int ScriptingService::execute(const string& code ){
        if (sproc == 0)
            sproc = new StatementProcessor(mowner);
        return sproc->execute( code );
    }

    ScriptingService::Functions  ScriptingService::loadFunctions( const string& file, bool do_throw/* = false*/ )
    {
      ifstream inputfile(file.c_str());
      if ( !inputfile ) {
          Logger::In in("ScriptingService::loadFunctions");
          Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
          return Functions();
      }
      string text;
      inputfile.unsetf( ios_base::skipws );
      istream_iterator<char> streambegin( inputfile );
      istream_iterator<char> streamend;
      std::copy( streambegin, streamend, back_inserter( text ) );
      return this->loadFunctions( text, file, do_throw );
    }

    ScriptingService::Functions  ScriptingService::loadFunctions( const string& code, const string& filename, bool mrethrow )
    {

      Logger::In in("ScriptingService::loadFunctions");
      Parser p(mowner->engine());
      Functions exec;
      Functions ret;
      try {
          Logger::log() << Logger::Info << "Parsing file "<<filename << Logger::endl;
          ret = p.parseFunction(code, mowner, filename);
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
                  if ( mowner->engine()->runFunction( it->get() ) == false) {
                      Logger::log() << Logger::Error << "Could not run Function '"<< (*it)->getName() <<"' :" << Logger::nl;
                      Logger::log() << "Processor not accepting or function queue is full." << Logger::endl;
                  } else
                      exec.push_back( *it ); // is being executed.
              }
          }
      return exec;

    }

    bool ScriptingService::runScript( const string& file )
    {
        ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ScriptingService::runScript");
            Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
            return false;
        }
        string text;
        inputfile.unsetf( ios_base::skipws );
        istream_iterator<char> streambegin( inputfile );
        istream_iterator<char> streamend;
        std::copy( streambegin, streamend, back_inserter( text ) );

        log(Info) << "Running Script "<< file <<" ..." << Logger::endl;
        return evalInternal( file, text );
    }

    bool ScriptingService::eval(const string& code) {
        return evalInternal("eval()", code);
    }

    bool ScriptingService::evalInternal(const string& filename, const string& code )
    {
        Logger::In in("ScriptingService");
        Parser parser( GlobalEngine::Instance() );
        try {
            parser.runScript(code, mowner, this, filename );
        }
        catch( const file_parse_exception& exc )
        {
            log(Error) <<filename<<" :"<< exc.what() << endlog();
            return false;
        }
        return true;
    }

    bool ScriptingService::loadPrograms( const string& file, bool do_throw /*= false*/ )
    {
        ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ScriptingService::loadProgram");
            Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
            return false;
        }
        string text;
        inputfile.unsetf( ios_base::skipws );
        istream_iterator<char> streambegin( inputfile );
        istream_iterator<char> streamend;
        std::copy( streambegin, streamend, back_inserter( text ) );
        return this->loadPrograms( text, file, do_throw );
    }

    bool ScriptingService::loadPrograms( const string& code, const string& filename, bool mrethrow ){

      Logger::In in("ProgramLoader::loadProgram");
      Parser parser(mowner->engine());
      Parser::ParsedPrograms pg_list;
      try {
          Logger::log() << Logger::Info << "Parsing file "<<filename << Logger::endl;
          pg_list = parser.parseProgram(code, mowner, filename );
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

    bool ScriptingService::unloadProgram( const string& name, bool do_throw ){
        Logger::In in("ScriptingService::unloadProgram");
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


    bool ScriptingService::loadStateMachines( const string& file, bool do_throw /*= false*/  )
    {
        ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ScriptingService::loadStateMachine");
          Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
          return false;
        }
        string text;
        inputfile.unsetf( ios_base::skipws );
        istream_iterator<char> streambegin( inputfile );
        istream_iterator<char> streamend;
        std::copy( streambegin, streamend, back_inserter( text ) );
      return this->loadStateMachines( text, file, do_throw );
    }

    bool ScriptingService::loadStateMachines( const string& code, const string& filename, bool mrethrow )
    {
        Logger::In in("ScriptingService::loadStateMachine");
        Parser parser(mowner->engine());
        Parser::ParsedStateMachines pg_list;
        try {
            Logger::log() << Logger::Info << "Parsing file "<<filename << Logger::endl;
            pg_list = parser.parseStateMachine( code, mowner, filename );
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

    bool ScriptingService::unloadStateMachine( const string& name, bool do_throw ) {
        Logger::In in("ScriptingService::unloadStateMachine");
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

    bool ScriptingService::hasProgram(const string& name) const {
        return programs.find(name) != programs.end();
    }

    int ScriptingService::getProgramLine(const string& name) const {
        const ProgramInterfacePtr pi = getProgram(name);
        return pi ? pi->getLineNumber() : -1;
    }

    string ScriptingService::getProgramText(const string& name ) const {
        const ProgramInterfacePtr pi = getProgram(name);
        return pi ? pi->getText() : "";
    }

    bool ScriptingService::hasStateMachine(const string& name) const {
        return states.find(name) != states.end();
    }

    string ScriptingService::getStateMachineText(const string& name ) const {
        const StateMachinePtr sm = getStateMachine(name);
        return sm ? sm->getText() : "";
    }

    int ScriptingService::getStateMachineLine(const string& name ) const {
        const StateMachinePtr sm = getStateMachine(name);
        return sm ? sm->getLineNumber() : -1;
    }

    bool ScriptingService::startProgram(const string& name)
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->start();
        return false;
    }

    bool ScriptingService::isProgramRunning(const string& name) const
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->isRunning();
        return false;
    }

    bool ScriptingService::isProgramPaused(const string& name) const
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->isPaused();
        return false;
    }

    bool ScriptingService::inProgramError(const string& name) const
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->inError();
        return false;
    }

    bool ScriptingService::stopProgram(const string& name)
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->stop();
        return false;
    }

    bool ScriptingService::pauseProgram(const string& name)
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->pause();
        return false;
    }

    bool ScriptingService::stepProgram(const string& name)
    {
        ProgramInterfacePtr pi = getProgram(name);
        if (pi)
            return pi->step();
        return false;
    }

    bool ScriptingService::activateStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->activate();
        return false;
    }

    bool ScriptingService::deactivateStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->deactivate();
        return false;
    }

    bool ScriptingService::startStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->start();
        return false;
    }

    bool ScriptingService::pauseStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->pause();
        return false;
    }

    bool ScriptingService::stopStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->stop();
        return false;
    }

    bool ScriptingService::isStateMachinePaused(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->isPaused();
        return false;
    }

    bool ScriptingService::isStateMachineActive(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->isActive();
        return false;
    }

    bool ScriptingService::isStateMachineRunning(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->isAutomatic();
        return false;
    }

    bool ScriptingService::inStateMachineError(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->inError();
        return false;
    }

    string ScriptingService::getStateMachineState(const string& name) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->getCurrentStateName();
        return "";
    }

    bool ScriptingService::requestStateMachineState(const string& name, const string& state)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->requestState(state);
        return false;
    }

    bool ScriptingService::inStateMachineState(const string& name, const string& state) const
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->inState(state);
        return false;
    }

    bool ScriptingService::resetStateMachine(const string& name)
    {
        StateMachinePtr sm = getStateMachine(name);
        if (sm)
            return sm->reset();
        return false;
    }

}
