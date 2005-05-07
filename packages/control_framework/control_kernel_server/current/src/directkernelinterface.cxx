/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:46:14 CET 2004  directkernelinterface.cc 

                        directkernelinterface.cc -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#include "directkernelinterface.hpp"

#include "corelib/CommandInterface.hpp"
#include "corelib/Logger.hpp"
#include "control_kernel/ExecutionExtension.hpp"
#include "control_kernel/ReportingExtension.hpp"
#include "control_kernel/KernelInterfaces.hpp"
#include "execution/Parser.hpp"
#include "execution/parse_exception.hpp"
#include "execution/TryCommand.hpp"
#include "execution/ConditionComposite.hpp"

#include "execution/ParsedStateMachine.hpp"
#include "execution/ProgramInterface.hpp"

#include <sstream>

#include "execution/Parser.hpp"

namespace ExecutionClient
{
    using namespace ORO_Execution;
    using namespace ORO_Execution::detail;

  int DirectKernelInterface::uniquecommandidcntr;

  DirectKernelInterface::DirectKernelInterface(
    ExecutionExtension* ee, ReportingExtension* re,
    KernelBaseFunction* base )
      : executionext( ee ), reportingext( re ), kernelbase( base ),
        task( ee->getTaskContext() ), shutdHook(0)
  {
  }

    bool DirectKernelInterface::init() {
        return (executionext != 0);
    }

    bool DirectKernelInterface::requestShutdown() {
        if (kernelbase->isRunning() )
            return false;
        Logger::log() << Logger::Info << "Starting Shutdown." <<Logger::endl;
        if (shutdHook)
            shutdHook(); // call shutdownhook
        return true;
    }

    std::string DirectKernelInterface::getLogLine()
    {
        return Logger::log().getLogLine();
    }

  std::vector<std::string> DirectKernelInterface::getProgramNames()
  {
    Processor* proc = executionext->getProcessor();
    return proc->getProgramList();
  }

  std::vector<std::string> DirectKernelInterface::getStateMachineNames()
  {
    Processor* proc = executionext->getProcessor();
    return proc->getStateMachineList();
  }

 std::string DirectKernelInterface::getProgramText(const std::string& name )
 {
     const ProgramInterface* pi = executionext->getProgram(name);
     if (pi)
         return pi->getText();
     return std::string("Error, could not find ")+name;
 }

 int DirectKernelInterface::getProgramLine(const std::string& name )
 {
     const ProgramInterface* pi = executionext->getProgram(name);
     if (pi)
         return pi->getLineNumber();
     return -1;
 }

 std::string DirectKernelInterface::getStateText(const std::string& name )
 {
     const StateMachine* ps = executionext->getStateMachine(name);
     if (ps)
         return ps->getText();
     return std::string("Error, could not find ")+name;
 }

 int DirectKernelInterface::getStateLine(const std::string& name )
 {
     const StateMachine* ps = executionext->getStateMachine(name);
     if (ps)
       return ps->getLineNumber();
     return -1;
 }


    std::string DirectKernelInterface::getProgramStatus(const std::string& name )
    {
        switch ( executionext->getProcessor()->getProgramStatus( name ))
            {
            case Processor::ProgramStatus::unloaded:
                return "unloaded";
                break;
            case Processor::ProgramStatus::stopped:
                return "stopped";
                break;
            case Processor::ProgramStatus::running:
                return "running";
                break;
            case Processor::ProgramStatus::stepmode:
                return "paused";
                break;
            case Processor::ProgramStatus::error:
                return "error";
                break;
            }
        return "na";
    }
    std::string DirectKernelInterface::getStateStatus(const std::string& name )
    {
        switch ( executionext->getProcessor()->getStateMachineStatus( name ))
            {
            case Processor::StateMachineStatus::unloaded:
                return "unloaded";
                break;
            case Processor::StateMachineStatus::inactive:
                return "inactive";
                break;
            case Processor::StateMachineStatus::stopping:
                return "stopping";
                break;
            case Processor::StateMachineStatus::stopped:
                return "stopped";
                break;
            case Processor::StateMachineStatus::running:
                return "running";
                break;
            case Processor::StateMachineStatus::paused:
                return "paused";
                break;
            case Processor::StateMachineStatus::active:
                return "active";
                break;
            case Processor::StateMachineStatus::activating:
                return "activating";
                break;
            case Processor::StateMachineStatus::deactivating:
                return "deactivating";
                break;
            case Processor::StateMachineStatus::resetting:
                return "resetting";
                break;
            case Processor::StateMachineStatus::error:
                return "error";
                break;
            }
        return "na";
    }

    std::string DirectKernelInterface::getState(const std::string& name )
    {
     const StateMachine* ps = executionext->getStateMachine(name);
     if (ps && ps->isActive() )
         return ps->currentState()->getName();
     return std::string("na");
    }


  void DirectKernelInterface::loadStateMachines(
    const std::string& code, const std::string& filename )
  {
        Parser    parser;
        Parser::ParsedStateMachines contexts;
        std::stringstream text_stream( code );
      try {
        try {
          contexts = parser.parseStateMachine( text_stream, executionext->getTaskContext(), filename );
        }
        catch( const file_parse_exception& exc )
        {
            Logger::log() << Logger::Error << "ExecutionExtension : "
                          << "loadStateMachine  : "<< exc.what()<< Logger::endl;
          // no reason to catch this other than clarity
          throw;
        }
        if ( contexts.empty() )
        {
            Logger::log() << Logger::Info << "ExecutionExtension : "
                          << "loadStateMachine no StateMachines instantiated loaded from "<< filename << Logger::endl;
            throw program_load_exception( "No StateMachines instantiated in file \"" + filename + "\"." );
        }

        // this can throw a program_load_exception
        std::vector<ParsedStateMachine*>::iterator it;
        for( it= contexts.begin(); it !=contexts.end(); ++it) {
            executionext->getProcessor()->loadStateMachine( *it );
        }

        Logger::log() << Logger::Info << "ExecutionExtension : "
                      << "loadStateMachine loaded "<< contexts.end() - contexts.begin()<<" StateMachine(s) from " << filename << Logger::endl;
    }
    catch ( program_load_exception& e) {
        throw load_exception( e.what() );
    }
    catch ( file_parse_exception& e) {
        throw load_exception( e.what() );
    }
    catch ( ... ) {
        Logger::log() << Logger::Error << "Unknown exeption in DirectKernelInterface::loadStateMachines"<<Logger::endl;
    }
  }

 void DirectKernelInterface::loadPrograms( const std::string& code, const std::string& filename  )
  {
        Parser    parser;
        Parser::ParsedPrograms pg_list;
        std::stringstream text_stream( code );
      try {
        try {
            pg_list = parser.parseProgram( text_stream, executionext->getTaskContext(), filename );
        }
        catch( const file_parse_exception& exc )
        {
            Logger::log() << Logger::Error << "ExecutionExtension : "
                          << "loadProgram  : "<< exc.what()<< Logger::endl;
          // no reason to catch this other than clarity
          throw;
        }
        if ( pg_list.empty() )
        {
            Logger::log() << Logger::Warning << "ExecutionExtension : "
                          << "loadProgram no programs loaded from "<< filename << Logger::endl;
            return;
            // since functions can be present, this is not so exceptional.
            //throw program_load_exception( "Warning : No Programs defined in inputfile." );
        }
        for_each(pg_list.begin(), pg_list.end(), boost::bind( &Processor::loadProgram, executionext->getProcessor(), _1) );
        Logger::log() << Logger::Info << "ExecutionExtension : "
                      << "loadProgram loaded "<< pg_list.end() - pg_list.begin()<<" program(s) from " << filename << Logger::endl;
      }
    catch ( program_load_exception& e) {
        throw load_exception( e.what() );
    }
    catch ( file_parse_exception& e) {
        throw load_exception( e.what() );
    }
  }

  bool ExecutionClient::DirectKernelInterface::unloadStateMachine( const std::string& name )
  {
      try {
          return executionext->getProcessor()->deleteStateMachine( name );
      }
      catch ( program_unload_exception& e) {
          throw load_exception( e.what() );
      }
  }

  bool ExecutionClient::DirectKernelInterface::unloadProgram( const std::string& name )
  {
      try {
          return executionext->getProcessor()->deleteProgram( name );
      }
      catch ( program_unload_exception& e) {
          throw load_exception( e.what() );
      }
  }

  std::vector<std::string> ExecutionClient::DirectKernelInterface::getComponents()
  {
    std::vector<std::string> datasourcecomps = task->dataFactory.getObjectList();
    std::vector<std::string> commandcomps = task->commandFactory.getObjectList();
    std::vector<std::string> methodcomps = task->methodFactory.getObjectList();
    std::sort( datasourcecomps.begin(), datasourcecomps.end() );
    std::sort( commandcomps.begin(), commandcomps.end() );
    std::sort( methodcomps.begin(), methodcomps.end() );
    std::vector<std::string> ret_1;
    ret_1.reserve( datasourcecomps.size() );
    std::set_union( datasourcecomps.begin(), datasourcecomps.end(),
                    commandcomps.begin(), commandcomps.end(), std::back_inserter( ret_1 ) );
    std::vector<std::string> ret_2;
    ret_2.reserve( ret_1.size() );
    std::set_union( ret_1.begin(), ret_1.end(),
                    methodcomps.begin(), methodcomps.end(), std::back_inserter( ret_2 ) );
    return ret_2;
  }

  std::vector<std::string> ExecutionClient::DirectKernelInterface::getCommands( const std::string& component )
  {
    const CommandFactoryInterface* fact = task->commandFactory.getObjectFactory( component );
    if ( ! fact )
      return std::vector<std::string>();
    return fact->getCommandList();
  }

  std::vector<std::string> ExecutionClient::DirectKernelInterface::getData( const std::string& component )
  {
    const DataSourceFactoryInterface* fact = task->dataFactory.getObjectFactory( component );
    if ( ! fact )
      return std::vector<std::string>();
    return fact->getNames();
  }

  std::vector<std::string> ExecutionClient::DirectKernelInterface::getMethods( const std::string& component )
  {
    const MethodFactoryInterface* fact = task->methodFactory.getObjectFactory( component );
    if ( ! fact )
      return std::vector<std::string>();
    return fact->getNames();
  }

    void printResult(const std::string& code, DataSourceBase* ds) {
        std::string prompt( code + "   = " );
        // this method can print some primitive DataSource<>'s.
        DataSource<bool>* dsb = dynamic_cast<DataSource<bool>*>(ds);
        if (dsb) {
            Logger::log() << Logger::Info <<prompt<< (dsb->get() ? "true" : "false") <<Logger::endl;
            return;
        }
        DataSource<int>* dsi = dynamic_cast<DataSource<int>*>(ds);
        if (dsi) {
            Logger::log() << Logger::Info <<prompt<< dsi->get() <<Logger::endl;
            return;
        }
        DataSource<std::string>* dss = dynamic_cast<DataSource<std::string>*>(ds);
        if (dss) {
            Logger::log() << Logger::Info <<prompt<< dss->get() <<Logger::endl;
            return;
        }
        DataSource<double>* dsd = dynamic_cast<DataSource<double>*>(ds);
        if (dsd) {
            Logger::log() << Logger::Info <<prompt<< dsd->get() <<Logger::endl;
            return;
        }
        DataSource<char>* dsc = dynamic_cast<DataSource<char>*>(ds);
        if (dsc) {
            Logger::log() << Logger::Info <<prompt<< dsc->get() <<Logger::endl;
            return;
        }
        if (ds)
            ds->evaluate();
	
    }

  int DirectKernelInterface::startExecutingCommand( const std::string& code )
  {
      // return zero to 'reject', return + to indicate an accepted command,
      // return -1 for DS/Methods.
        Parser _parser;
        std::pair< CommandInterface*, ConditionInterface*> parseresult;
        try {
            // Check if it was a method or datasource :
            DataSourceBase::shared_ptr ds = _parser.parseExpression( code, task );
            // methods and DS'es are processed immediately.
            if ( ds.get() != 0 ) {
                printResult( code, ds.get() );
                return -1; // done here
            }
            return 0;
        } catch ( syntactic_parse_exception& pe ) { // missing brace etc
            // syntactic errors must be reported immediately
            Logger::log() << Logger::Error << "Invalid Expression."<<Logger::nl
                          << pe.what() <<Logger::endl;
            throw load_exception( pe.what() );
        } catch ( fatal_semantic_parse_exception& pe ) { // incorr args, ...
            // way to fatal,  must be reported immediately
            Logger::log() << Logger::Error << "Invalid Expression."<<Logger::nl
                          << pe.what() <<Logger::endl;
            throw load_exception( pe.what() );
        } catch ( parse_exception &pe ) // Got not a clue exception, so try other parser
            {
                // ignore it, try to parse it as a command :
                //cerr << "Ignoring : "<< pe.what() << endl;
                try {
                    parseresult = _parser.parseCommand( code, task );
                    // parseresult contains : command + implcond 
                    // We wrap the command ourselves because this is actually a dispatch,
                    // but the commandparser assumed it was not, since there is no 'Task-switch' involved.
                    // If a command's or method's condition is ConditionTrue, we might delete it before it
                    // is executed by the Processor ( see checkCommandFinished() ). 
                    TryCommand* tcom = new TryCommand( parseresult.first );
                    // compose impl term cond with executed() filter and do not invert the result :
                    ConditionInterface* implcond = new ConditionBinaryComposite< std::logical_and<bool> >( new TryCommandResult( tcom->executed(), false ), parseresult.second );
                    parseresult = make_pair( tcom, implcond );
                } catch ( parse_exception& pe ) {
                    Logger::log() << Logger::Error  << "Parse Error : Illegal command."<<Logger::nl
                                  << pe.what() << Logger::endl;
                    throw load_exception( pe.what() );
                }
            }
        catch (...) {
            Logger::log() << Logger::Error << "Uncaught Command : '"<< code <<"'"<<Logger::endl;
            return 0;
        }
                
        if ( parseresult.first == 0 ) { // this should not be reached
            Logger::log() << Logger::Error << "Empty Command."<<Logger::endl;
            return 0;
        }
        // It is for sure a real command, dispatch to target processor :
        int id = task->getProcessor()->process( parseresult.first );
        // returns null if Processor not running or not accepting.
        if ( id == 0 ) {
            Logger::log() << Logger::Error << "Command '"<< code <<"' not accepted by "<<task->getName()<<"'s Processor." <<Logger::endl;
            delete parseresult.first;
            delete parseresult.second;
            throw load_exception( "Command '"+ code +"' not accepted by "+task->getName()+"'s Processor." );
        } else {
            Logger::log() << Logger::Debug << "Queueing Command "<<id<<" : '" << code << "'" <<Logger::endl;
            commandlist[ id ] = parseresult;
            checkCommandFinished( id );
            return id;
        }

    return 0;
  }

  void ExecutionClient::DirectKernelInterface::checkCommandFinished( int id )
  {
      if ( id == 0 )
          return;
    commandlist_t::iterator i = commandlist.find( id );
    if ( i == commandlist.end() )
      return;
    CommandInterface* command = i->second.first;
    ConditionInterface* condition = i->second.second;
    if ( command && executionext->getProcessor()->isProcessed( id ) &&
         condition->evaluate() )
    {
        Logger::log() << Logger::Info << "Command " << id <<" finished."<<Logger::endl;
        delete command;
        delete condition;
        commandlist.erase( i );
    }
  }

  void DirectKernelInterface::abandonCommand( int id )
  {
    commandlist_t::iterator i = commandlist.find( id );
    if ( i == commandlist.end() )
      return;
    if ( executionext->getProcessor()->isProcessed( id ) ) {
        Logger::log() << Logger::Debug << "Command " << id << " was processed."<<Logger::endl;
        delete i->second.first;
        delete i->second.second;
        commandlist.erase( i );
    }
  }

  bool DirectKernelInterface::isCommandFinished( int id )
  {
    checkCommandFinished( id );
    return commandlist.find( id ) == commandlist.end();
  }

  std::vector<std::string> DirectKernelInterface::getControllers()
  {
    std::vector<std::string> ret(
      kernelbase->controllers.getNameBegin(),
      kernelbase->controllers.getNameEnd() );
    return ret;
  }

  std::vector<std::string> DirectKernelInterface::getGenerators()
  {
    std::vector<std::string> ret(
      kernelbase->generators.getNameBegin(),
      kernelbase->generators.getNameEnd() );
    return ret;
  }

  std::vector<std::string> DirectKernelInterface::getEffectors()
  {
    std::vector<std::string> ret(
      kernelbase->effectors.getNameBegin(),
      kernelbase->effectors.getNameEnd() );
    return ret;
  }

  std::vector<std::string> DirectKernelInterface::getEstimators()
  {
    std::vector<std::string> ret(
      kernelbase->estimators.getNameBegin(),
      kernelbase->estimators.getNameEnd() );
    return ret;
  }

  std::vector<std::string> DirectKernelInterface::getSensors()
  {
    std::vector<std::string> ret(
      kernelbase->sensors.getNameBegin(),
      kernelbase->sensors.getNameEnd() );
    return ret;
  }

  std::vector<std::string> DirectKernelInterface::getSupports()
  {
    std::vector<std::string> ret(
      kernelbase->supports.getNameBegin(),
      kernelbase->supports.getNameEnd() );
    return ret;
  }
}
