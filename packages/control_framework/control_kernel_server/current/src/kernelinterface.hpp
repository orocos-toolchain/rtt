/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:46:14 CET 2004  kernelinterface.hpp 

                        kernelinterface.hpp -  description
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
 
 
#ifndef EXECUTION_CLIENT_SRC_KERNELINTERFACE_HPP
#define EXECUTION_CLIENT_SRC_KERNELINTERFACE_HPP

#include <vector>
#include <string>
#include <boost/function.hpp>

    namespace ORO_ControlKernel {
        class ControlKernelInterface;
    }


namespace ExecutionClient
{
  class load_exception
  {
    std::string mdesc;
  public:
    load_exception( const std::string& desc )
      : mdesc( desc )
      {
      }
    std::string what() const
      {
        return mdesc;
      }
  };

  // This class is an interface describing how we are communicating
  // with the OROCOS kernel.  Two subclasses are currently planned:
  // DirectKernelInterface and CORBAKernelInterface
  class KernelInterface
  {
    static KernelInterface* sinstance;
  public:
      static void setKernelInterfaceInstance( KernelInterface* kint );
      static KernelInterface* instance();
  public:
    virtual ~KernelInterface();

      /**
       * Initialise the kernel.
       * @return false on failure.
       */
      virtual bool init() = 0;

      /**
       * Request the kernel to shutdown.
       */
      virtual bool requestShutdown() = 0;

      virtual void setShutdownHook( boost::function< void(void) >  ) {}

      /**
       * Get one line of log info (empty string if nothing to report).
       */
      virtual std::string getLogLine() = 0;

    // programs
    virtual std::vector<std::string> getProgramNames() = 0;

    // state contexts
    virtual std::vector<std::string> getStateContextNames() = 0;
    // The filename is not needed, but it will be used to construct
    // error messages.  This function will throw a
    // file_parse_exception ( defined in
    // <execution/parse_exception.hpp> ) if parsing fails, or a
    // statecontext_load_exception ( defined in ExecutionExtension.hpp
    // ) if loading fails.
    virtual void loadStateContexts( const std::string& code, const std::string& filename ) = 0;
    virtual bool unloadStateContext( const std::string& name ) = 0;

    virtual void loadPrograms( const std::string& code, const std::string& filename ) = 0;
    virtual bool unloadProgram( const std::string& name ) = 0;

    virtual std::string getProgramText(const std::string& name ) = 0;
    virtual std::string getStateText(const std::string& name ) = 0;

    virtual std::string getProgramStatus(const std::string& name ) = 0;
    virtual int getProgramLine(const std::string& name ) = 0;

    virtual std::string getStateStatus(const std::string& name ) = 0;
    virtual int getStateLine(const std::string& name ) = 0;
    virtual std::string getState(const std::string& name ) = 0;

    // return the list of names of all controller components
    virtual std::vector<std::string> getControllers() = 0;
    // return the list of names of all generator components
    virtual std::vector<std::string> getGenerators() = 0;
    // return the list of names of all effector components
    virtual std::vector<std::string> getEffectors() = 0;
    // return the list of names of all estimator components
    virtual std::vector<std::string> getEstimators() = 0;
    // return the list of names of all sensor components
    virtual std::vector<std::string> getSensors() = 0;
    // return the list of names of all support components
    virtual std::vector<std::string> getSupports() = 0;

    // return the list of names of those components that have
    // registered either a DataSourceFactory or a CommandFactory.
    virtual std::vector<std::string> getComponents() = 0;

    // return the list of commands of the given component.  Can return
    // an empty list.
    virtual std::vector<std::string> getCommands( const std::string& component ) = 0;
    // return the list of datasources of the given component.  Can
    // return an empty list.
    virtual std::vector<std::string> getData( const std::string& component ) = 0;
    // return the list of methods of the given component.  Can
    // return an empty list.
    virtual std::vector<std::string> getMethods( const std::string& component ) = 0;

    // This function parses the command in the given string, and
    // starts executing it.  The returned integer is a unique id for
    // the executing command on success.  Will throw a parse_exception
    // on parse error and a load_exception if loading fails...
    virtual int startExecutingCommand( const std::string& code ) = 0;
    // is the command with identifier commandid ( as returned by
    // startExecutingCommand ) finished executing ?  Note that you
    // still need to call abandonCommand if the command is finished.
    virtual bool isCommandFinished( int commandid ) = 0;
    // stop waiting for the execution of the command with the given id
    // ( as returned by startExecutingCommand ) to complete.
    virtual void abandonCommand( int commandid ) = 0;
  };
}

#endif
