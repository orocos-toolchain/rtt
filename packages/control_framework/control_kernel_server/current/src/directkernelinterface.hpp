/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:46:14 CET 2004  directkernelinterface.hpp 

                        directkernelinterface.hpp -  description
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
 
 
#ifndef TRUNK_SRC_DIRECTKERNELINTERFACE_HPP
#define TRUNK_SRC_DIRECTKERNELINTERFACE_HPP

#include "kernelinterface.hpp"

#include <map>

namespace ORO_ControlKernel
{
  class ExecutionExtension;
  class ReportingExtension;
  class KernelBaseFunction;
}

namespace ORO_CoreLib
{
  class ConditionInterface;
  class CommandInterface;
}

namespace ORO_Execution
{
    class TaskContext;
}

using namespace ORO_ControlKernel;
using namespace ORO_CoreLib;
using namespace ORO_Execution;

namespace ExecutionClient
{
  class DirectKernelInterface
    : public KernelInterface
  {
      ExecutionExtension* executionext;
      ReportingExtension* reportingext;
      KernelBaseFunction* kernelbase;
      TaskContext* task;
      boost::function<void(void) > shutdHook;
      void checkCommandFinished( int id );

      typedef std::map<int, std::pair<CommandInterface*, ConditionInterface*> > commandlist_t;
      commandlist_t commandlist;
      static int uniquecommandidcntr;
  public:
      virtual bool init();

      /**
       * Pass 0 for one of these interfaces if your kernel doesn't have
       * them.  ( won't work properly yet )
       */
      DirectKernelInterface( ExecutionExtension* executionext, ReportingExtension* reportingext,
                             KernelBaseFunction* base );

      /**
       * The destructor.
       */
      ~DirectKernelInterface() {}

      bool requestShutdown();
      void setShutdownHook( boost::function< void(void) > foo ) { shutdHook = foo; }
      std::string getLogLine();

    // programs
    std::vector<std::string> getProgramNames();

    // state contexts
    std::vector<std::string> getStateMachineNames();
    void loadStateMachines( const std::string& code, const std::string& filename );
    bool unloadStateMachine( const std::string& name );

    void loadPrograms( const std::string& code, const std::string& filename );
    bool unloadProgram( const std::string& name );

    std::string getProgramText(const std::string& name );
    std::string getStateText(const std::string& name );

    std::string getProgramStatus(const std::string& name );
    int getProgramLine(const std::string& name );
    std::string getStateStatus(const std::string& name );
    int getStateLine(const std::string& name );
    std::string getState(const std::string& name );

    std::vector<std::string> getControllers();
    std::vector<std::string> getGenerators();
    std::vector<std::string> getEffectors();
    std::vector<std::string> getEstimators();
    std::vector<std::string> getSensors();
    std::vector<std::string> getSupports();

    std::vector<std::string> getComponents();
    std::vector<std::string> getCommands( const std::string& component );
    std::vector<std::string> getData( const std::string& component );
    std::vector<std::string> getMethods( const std::string& component );

    int startExecutingCommand( const std::string& code );
    bool isCommandFinished( int id );
    void abandonCommand( int id );
  };
}
#endif
