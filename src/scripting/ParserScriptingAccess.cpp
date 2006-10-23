/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  ParserScriptingAccess.cxx 

                        ParserScriptingAccess.cxx -  description
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
 
 

#include "ParserScriptingAccess.hpp"
#include "StatementProcessor.hpp"
#include "Parser.hpp"
#include "Logger.hpp"
#include "TaskContext.hpp"
#include "Method.hpp"
#include "Command.hpp"
#include "ProgramProcessor.hpp"
#include "StateMachineProcessor.hpp"
#include <sstream>
#include <fstream>

namespace RTT
{
    using namespace std;

    ParserScriptingAccess::ParserScriptingAccess( TaskContext* parent )
        : ScriptingAccess(parent), sproc(0)
    {
        OperationInterface* obj = parent->getObject("scripting");
        obj = this->createTaskObject( obj );
        parent->addObject( obj );
    }

    bool ParserScriptingAccess::doExecute(const std::string& code)
    {
        return this->execute(code) >= 0;
    }

    bool ParserScriptingAccess::doLoadPrograms( std::string filename )
    {
        return this->loadPrograms(filename, false);
    }

    bool ParserScriptingAccess::doLoadProgramText( std::string code )
    {
        return this->loadPrograms(code, "string", false);
    }
    bool ParserScriptingAccess::doUnloadProgram( std::string name )
    {
        return this->unloadProgram(name, false);
    }

    bool ParserScriptingAccess::doLoadStateMachines( std::string filename )
    {
        return this->loadStateMachines(filename, false);
    }
    bool ParserScriptingAccess::doLoadStateMachineText( std::string code )
    {
        return this->loadStateMachines(code, "string", false);
    }
    bool ParserScriptingAccess::doUnloadStateMachine( std::string name )
    {
        return this->unloadStateMachine(name, false);
    }

    OperationInterface* ParserScriptingAccess::createTaskObject(OperationInterface* obj)
    {
        if ( !obj )
            obj = new TaskObject("scripting","Access to the Scripting interface. \
Use this object in order to load or query programs or state machines.");

        obj->methods()->addMethod( method( "execute", &ParserScriptingAccess::execute, this),
                                   "Execute a line of code.", "Code", "A single statement.");
        // Methods for loading programs
        obj->methods()->addMethod( method( "loadPrograms", &ParserScriptingAccess::doLoadPrograms, this),
                                   "Load a program from a given file.", "Filename", "The filename of the script."  );
        obj->methods()->addMethod( method( "loadProgramText", &ParserScriptingAccess::doLoadProgramText, this),
                                   "Load a program from a string.", "Code", "A string containing one or more program scripts." );
        obj->methods()->addMethod( method( "unloadProgram", &ParserScriptingAccess::doUnloadProgram, this),
                                   "Remove a loaded program.", "Name", "The name of the loaded Program"  );

        // Query Methods for programs
        obj->methods()->addMethod( method( "getProgramStatus", &ParserScriptingAccess::getProgramStatus, this),
                                            "Get the status of a program?", "Name", "The Name of the loaded Program"  );
        obj->methods()->addMethod( method( "getProgramLine", &ParserScriptingAccess::getProgramLine, this),
                                            "Get the current line of execution of a program?", "Name", "The Name of the loaded Program"  );

        // Methods for loading state machines
        obj->methods()->addMethod( method( "loadStateMachines", &ParserScriptingAccess::doLoadStateMachines, this),
                                   "Load a state machine from a given file.", "Filename", "The filename of the script."  );
        obj->methods()->addMethod( method( "loadStateMachineText", &ParserScriptingAccess::doLoadStateMachineText, this),
                                   "Load a state machine from a string.", "Code", "A string containing one or more state machine scripts." );
        obj->methods()->addMethod( method( "unloadStateMachine", &ParserScriptingAccess::doUnloadStateMachine, this),
                                   "Remove a loaded state machine.", "Name", "The name of the loaded State Machine"  );

        // Query Methods for state machines
        obj->methods()->addMethod( method( "getStateMachineStatus", &ParserScriptingAccess::getStateMachineStatus, this),
                                            "Get the status of a state machine?", "Name", "The Name of the loaded State Machine"  );
        obj->methods()->addMethod( method( "getStateMachineLine", &ParserScriptingAccess::getStateMachineLine, this),
                                            "Get the current line of execution of a state machine?", "Name", "The Name of the loaded State Machine"  );

        return obj;
    }

    ParserScriptingAccess::~ParserScriptingAccess()
    {
        delete sproc;
    }

    int ParserScriptingAccess::execute(const std::string& code ){
        if (sproc == 0)
            sproc = new StatementProcessor(mparent);
        return sproc->execute( code );
    }

    CommandC ParserScriptingAccess::getCommand( int ticket ){
        if (sproc)
            return sproc->getCommand(ticket);
        return CommandC();
    }

    ParserScriptingAccess::Functions  ParserScriptingAccess::loadFunctions( std::string file, bool do_throw/* = false*/ )
    {
      std::ifstream inputfile(file.c_str());
      if ( !inputfile ) {
          Logger::In in("ParserScriptingAccess::loadFunctions");
          Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
          return Functions();
      }
      std::string text;
      inputfile.unsetf( std::ios_base::skipws );
      std::istream_iterator<char> streambegin( inputfile );
      std::istream_iterator<char> streamend;
      std::copy( streambegin, streamend, std::back_inserter( text ) );
      return this->loadFunctions( text, file, do_throw );
    }

    ParserScriptingAccess::Functions  ParserScriptingAccess::loadFunctions( string code, string filename, bool mrethrow )
    {

      Logger::In in("ParserScriptingAccess::loadFunctions");
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
                  if ( mparent->engine()->programs()->runFunction( it->get() ) == false) {
                      Logger::log() << Logger::Error << "Could not run Function '"<< (*it)->getName() <<"' :" << Logger::nl;
                      Logger::log() << "Processor not accepting or function queue is full." << Logger::endl;
                  } else
                      exec.push_back( *it ); // is being executed.
              }
          }
      return exec;

    }
    
    bool ParserScriptingAccess::loadPrograms( std::string file, bool do_throw /*= false*/ )
    {
        std::ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ParserScriptingAccess::loadProgram");
            Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
            return false;
        }
        std::string text;
        inputfile.unsetf( std::ios_base::skipws );
        std::istream_iterator<char> streambegin( inputfile );
        std::istream_iterator<char> streamend;
        std::copy( streambegin, streamend, std::back_inserter( text ) );
        return this->loadPrograms( text, file, do_throw );
    }

    bool ParserScriptingAccess::loadPrograms( string code, string filename, bool mrethrow ){

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
                      if (mparent->engine()->programs()->loadProgram( *it ) == false)
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
    
    bool ParserScriptingAccess::unloadProgram( string name, bool do_throw ){
        Logger::In in("ParserScriptingAccess::unloadProgram");
        try {
            Logger::log() << Logger::Info << "Unloading Program '"<< name <<"'"<< Logger::endl;
            if (mparent->engine()->programs()->unloadProgram(name) == false)
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


    bool ParserScriptingAccess::loadStateMachines( std::string file, bool do_throw /*= false*/  )
    {
        std::ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ParserScriptingAccess::loadStateMachine");
          Logger::log() << Logger::Error << "Script "+file+" does not exist." << Logger::endl;
          return false;
        }
        std::string text;
        inputfile.unsetf( std::ios_base::skipws );
        std::istream_iterator<char> streambegin( inputfile );
        std::istream_iterator<char> streamend;
        std::copy( streambegin, streamend, std::back_inserter( text ) );
      return this->loadStateMachines( text, file, do_throw );
    }

    bool ParserScriptingAccess::loadStateMachines( string code, string filename, bool mrethrow ) 
    {
        Logger::In in("ParserScriptingAccess::loadStateMachine");
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
                        if (mparent->engine()->states()->loadStateMachine( *it ) == false)
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
    
    bool ParserScriptingAccess::unloadStateMachine( string name, bool do_throw ) {
        Logger::In in("ParserScriptingAccess::unloadStateMachine");
        try {
            Logger::log() << Logger::Info << "Unloading StateMachine '"<< name <<"'"<< Logger::endl;
            if (mparent->engine()->states()->unloadStateMachine(name) == false) 
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

}
