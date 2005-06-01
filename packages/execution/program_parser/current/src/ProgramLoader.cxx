/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  ProgramLoader.cxx 

                        ProgramLoader.cxx -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
 

#include "execution/ProgramLoader.hpp"
#include "execution/Parser.hpp"
#include "execution/ParsedStateMachine.hpp"
#include "corelib/Logger.hpp"

#include <iostream>
#include <fstream>

using namespace boost;
using namespace std;
using namespace ORO_CoreLib;

namespace ORO_Execution
{

  ProgramLoader::Functions ProgramLoader::loadFunction( const std::string& file, TaskContext* c)
  {
      std::ifstream inputfile(file.c_str());
      if ( !inputfile ) {
          Logger::log() << Logger::Error << "ProgramLoader::executeFunction : Script "+file+" does not exist." << Logger::endl;
          return ProgramLoader::Functions();
      }
      return this->loadFunction( inputfile, c, file );
  }

  ProgramLoader::Functions ProgramLoader::loadFunction( std::istream& s, TaskContext* c, const std::string& filename)
  {
      Parser p;
      Functions exec;
      Functions ret;
      try {
          Logger::log() << Logger::Info << "ProgramLoader: Parsing file "<<filename << Logger::endl;
          ret = p.parseFunction(s, c, filename);
      }
      catch( const file_parse_exception& exc )
          {
              Logger::log() << Logger::Error <<filename<<" :"<< exc.what() << Logger::endl;
              return Functions();
          }
      if ( ret.empty() )
          {
              Logger::log() << Logger::Info << "No Functions executed from "<< filename << Logger::endl;
              return Functions();
          } else {
              // Load all listed functions in the TaskContext's Processor:
              for( Parser::ParsedFunctions::iterator it = ret.begin(); it != ret.end(); ++it) {
                  Logger::log() << Logger::Info << "Queueing Function "<< (*it)->getName() << Logger::endl;
                  if ( c->getProcessor()->runFunction( *it ) == false) {
                      Logger::log() << Logger::Error << "Could not run Function "<< (*it)->getName() <<" :" << Logger::nl;
                      Logger::log() << Logger::Error << "Processor not accepting or function queue is full." << Logger::endl;
                      delete *it;
                  } else
                      exec.push_back( *it ); // is being executed.
              }
              return exec;
          }
    return exec;
  }

  bool ProgramLoader::loadProgram( const std::string& file, TaskContext* c)
  {
      std::ifstream inputfile(file.c_str());
      if ( !inputfile ) {
          Logger::log() << Logger::Error << "ProgramLoader::loadProgram : Script "+file+" does not exist." << Logger::endl;
          return false;
      }
      return this->loadProgram( inputfile, c, file );
  }

  bool ProgramLoader::loadProgram( std::istream& s, TaskContext* c, const std::string& filename)
  {
      Parser parser;
      Parser::ParsedPrograms pg_list;
      try {
          Logger::log() << Logger::Info << "ProgramLoader: Parsing file "<<filename << Logger::endl;
          pg_list = parser.parseProgram( s, c, filename );
      }
      catch( const file_parse_exception& exc )
          {
              Logger::log() << Logger::Error <<filename<<" :"<< exc.what() << Logger::endl;
              return false;
          }
      if ( pg_list.empty() )
          {
              Logger::log() << Logger::Info << "No Programs found in "<< filename << Logger::endl;
              return true;
          } else {
              // Load all listed programs in the TaskContext's Processor:
              bool error = false;
              for( Parser::ParsedPrograms::iterator it = pg_list.begin(); it != pg_list.end(); ++it) {
                  try {
                      Logger::log() << Logger::Info << "Loading Program "<< (*it)->getName() << Logger::endl;
                      c->getProcessor()->loadProgram( *it );
                  } catch (program_load_exception& e ) {
                      Logger::log() << Logger::Error << "Could not load Program "<< (*it)->getName() <<" :" << Logger::nl;
                      Logger::log() << Logger::Error << e.what() << Logger::endl;
                      delete *it;
                      error = true;
                  }
              }
              return !error;
          }
    return true;
  }

    bool ProgramLoader::unloadProgram( const std::string& name, TaskContext* target)
    {
        try {
            Logger::log() << Logger::Info << "Unloading Program "<< name << Logger::endl;
            target->getProcessor()->deleteProgram(name);
        } catch (program_unload_exception& e ) {
            Logger::log() << Logger::Error << "Could not unload Program "<< name <<" :" << Logger::nl;
            Logger::log() << Logger::Error << e.what() << Logger::endl;
            return false;
        }
        return true;
    }

    bool ProgramLoader::unloadStateMachine( const std::string& name, TaskContext* target)
    {
        try {
            Logger::log() << Logger::Info << "Unloading StateMachine "<< name << Logger::endl;
            target->getProcessor()->deleteStateMachine(name);
        } catch (program_unload_exception& e ) {
            Logger::log() << Logger::Error << "Could not unload StateMachine "<< name <<" :" << Logger::nl;
            Logger::log() << Logger::Error << e.what() << Logger::endl;
            return false;
        }
        return true;
    }

  bool ProgramLoader::loadStateMachine( const std::string& file, TaskContext* c)
  {
      std::ifstream inputfile(file.c_str());
      if ( !inputfile ) {
          Logger::log() << Logger::Error << "ProgramLoader::loadStateMachine : Script "+file+" does not exist." << Logger::endl;
          return false;
      }
      return this->loadStateMachine( inputfile, c, file );
  }

    bool ProgramLoader::loadStateMachine( std::istream& s, TaskContext* c, const std::string& filename)
    {
        Parser parser;
        Parser::ParsedStateMachines pg_list;
        try {
            Logger::log() << Logger::Info << "Parsing file "<<filename << Logger::endl;
            pg_list = parser.parseStateMachine( s, c, filename );
        }
        catch( const file_parse_exception& exc )
            {
                Logger::log() << Logger::Error <<filename<<" :"<< exc.what() << Logger::endl;
                return false;
            }
        if ( pg_list.empty() )
            {
                Logger::log() << Logger::Info << "No StateMachines instantiated in "<< filename << Logger::endl;
                return false;
            } else {
                bool error = false;
                // Load all listed stateMachines in the TaskContext's Processor:
                for( Parser::ParsedStateMachines::iterator it = pg_list.begin(); it != pg_list.end(); ++it) {
                    try {
                        Logger::log() << Logger::Info << "Loading StateMachine "<< (*it)->getName() << Logger::endl;
                        c->getProcessor()->loadStateMachine( *it );
                    } catch (program_load_exception& e ) {
                        Logger::log() << Logger::Error << "Could not load StateMachine "<< (*it)->getName()<<" :" << Logger::nl;
                        Logger::log() << Logger::Error << e.what() << Logger::endl;
                        delete *it;
                        error = true;
                    }
                }
                return !error;
            }
        return true;
    }
}
