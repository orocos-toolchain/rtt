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
 
 

#include "rtt/ParserScriptingAccess.hpp"
#include "rtt/StatementProcessor.hpp"
#include "rtt/Parser.hpp"
#include "rtt/Logger.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/ProgramProcessor.hpp"
#include "rtt/StateMachineProcessor.hpp"
#include <sstream>
#include <fstream>

namespace RTT
{
    using namespace std;
    

    ParserScriptingAccess::~ParserScriptingAccess()
    {
        delete sproc;
    }

    int ParserScriptingAccess::execute( string code ){
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
