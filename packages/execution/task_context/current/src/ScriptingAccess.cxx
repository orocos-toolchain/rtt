
#include "execution/ScriptingAccess.hpp"
#include "execution/StatementProcessor.hpp"
#include "execution/Parser.hpp"
#include "corelib/Logger.hpp"
#include "execution/TaskContext.hpp"
#include <sstream>

namespace ORO_Execution
{
    using namespace std;
    using namespace ORO_CoreLib;

    ScriptingAccess::~ScriptingAccess()
    {
        delete sproc;
    }

    bool ScriptingAccess::execute( string code ){
        if (sproc == 0)
            sproc = new StatementProcessor(mparent);
        return sproc->execute( code );
    }

    ScriptingAccess::Functions  ScriptingAccess::loadFunctions( std::string file, bool do_throw/* = false*/ )
    {
      std::ifstream inputfile(file.c_str());
      if ( !inputfile ) {
          Logger::In in("ScriptingAccess::loadFunctions");
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

    ScriptingAccess::Functions  ScriptingAccess::loadFunctions( string code, string filename, bool mrethrow )
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
              Logger::log() << Logger::Error << filename<<" :"<< exc.what() << Logger::endl;
              if ( mrethrow )
                  throw;
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
    
    bool ScriptingAccess::loadPrograms( std::string file, bool do_throw /*= false*/ )
    {
        std::ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ScriptingAccess::loadProgram");
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

    bool ScriptingAccess::loadPrograms( string code, string filename, bool mrethrow ){

      Logger::In in("ProgramLoader::loadProgram");
      Parser parser;
      Parser::ParsedPrograms pg_list;
      try {
          Logger::log() << Logger::Info << "Parsing file "<<filename << Logger::endl;
          pg_list = parser.parseProgram(code, mparent, filename );
      }
      catch( const file_parse_exception& exc )
          {
              Logger::log() << Logger::Error <<filename<<" :"<< exc.what() << Logger::endl;
              if ( mrethrow )
                  throw;
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
                      mparent->engine()->programs()->loadProgram( *it );
                  } catch (program_load_exception& e ) {
                      Logger::log() << Logger::Error << "Could not load Program '"<< (*it)->getName() <<"' :" << Logger::nl;
                      Logger::log() << e.what() << Logger::endl;
                      if ( mrethrow )
                          errors += "Could not load Program '"+ (*it)->getName() +"' :\n"+e.what()+'\n';
                      error = true;
                  }
              }
              if (error && mrethrow )
                  throw program_load_exception( errors );
              return !error;
          }
      // never reached
    }
    
    bool ScriptingAccess::unloadProgram( string name, bool do_throw ){
        Logger::In in("ScriptingAccess::unloadProgram");
        try {
            Logger::log() << Logger::Info << "Unloading Program '"<< name <<"'"<< Logger::endl;
            mparent->engine()->programs()->unloadProgram(name);
        } catch (program_unload_exception& e ) {
            Logger::log() << Logger::Error << "Could not unload Program '"<< name <<"' :" << Logger::nl;
            Logger::log() << e.what() << Logger::endl;
            if ( do_throw )
                throw;
            return false;
        }
        return true;
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
        std::ifstream inputfile(file.c_str());
        if ( !inputfile ) {
            Logger::In in("ScriptingAccess::loadStateMachine");
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

    bool ScriptingAccess::loadStateMachines( string code, string filename, bool mrethrow ) 
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
                Logger::log() << Logger::Error <<filename<<" :"<< exc.what() << Logger::endl;
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
                        mparent->engine()->states()->loadStateMachine( *it );
                    } catch (program_load_exception& e ) {
                        Logger::log() << Logger::Error << "Could not load StateMachine '"<< (*it)->getName()<<"' :" << Logger::nl;
                        Logger::log() << e.what() << Logger::endl;
                      if ( mrethrow )
                          errors += "Could not load Program '"+ (*it)->getName() +"' :\n"+e.what()+'\n';
                        error = true;
                    }
                }
                if ( error && mrethrow )
                    throw program_load_exception( errors );
                return !error;
            }
        // never reached
        return false;
    }
    
    bool ScriptingAccess::unloadStateMachine( string name, bool do_throw ) {
        Logger::In in("ScriptingAccess::unloadStateMachine");
        try {
            Logger::log() << Logger::Info << "Unloading StateMachine '"<< name <<"'"<< Logger::endl;
            mparent->engine()->states()->unloadStateMachine(name);
        } catch (program_unload_exception& e ) {
            Logger::log() << Logger::Error << "Could not unload StateMachine '"<< name <<"' :" << Logger::nl;
            Logger::log() << e.what() << Logger::endl;
            if ( do_throw )
                throw;
            return false;
        }
        return true;
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
