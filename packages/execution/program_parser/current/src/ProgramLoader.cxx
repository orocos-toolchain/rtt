
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

  ProgramLoader::Functions ProgramLoader::executeFunction( const std::string& file, TaskContext* c)
  {
      std::ifstream inputfile(file.c_str());
      return this->executeFunction( inputfile, c, file );
  }

  ProgramLoader::Functions ProgramLoader::executeFunction( std::istream& s, TaskContext* c, const std::string& filename)
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
              Logger::log() << Logger::Error << exc.what() << Logger::endl;
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
              Logger::log() << Logger::Error << exc.what() << Logger::endl;
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
              return error;
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
                Logger::log() << Logger::Error << exc.what() << Logger::endl;
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
                return error;
            }
        return true;
    }
}
