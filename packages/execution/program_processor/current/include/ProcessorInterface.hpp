/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  ProcessorInterface.hpp 

                        ProcessorInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
    ***************************************************************************
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    ***************************************************************************/
 
 
#ifndef PROCESSORINTERFACE_HPP
#define PROCESSORINTERFACE_HPP

#include "ProgramInterface.hpp"
#include <corelib/StateContext.hpp>
#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{
    using ORO_CoreLib::StateContext;
    using ORO_CoreLib::CommandInterface;

    /**
     * This abstract base class provides functionality to 
     * execute data on a configured system.	
     */
    class ProcessorInterface
    {
    public:
        	
        virtual ~ProcessorInterface()
        {}                            
            
        /**
         * Load a state context
         */
        virtual bool loadStateContext(const std::string& name, StateContext* sc) = 0; 
			
        /**
         * Reset to the initial state
         */
        virtual bool resetStateContext(const std::string& name) = 0;

        /**
         * Start evaluating a state context.
         */
        virtual bool startStateContext(const std::string& name) = 0;

        /**
         * Stop evaluating a state context and go to the final state.
         */
        virtual bool stopStateContext(const std::string& name) = 0;
            
        /**
         * Discard a state context.
         */
        virtual bool deleteStateContext(const std::string& name) = 0;
            
        /**
         * Load a program to be executed.
         */
        virtual bool loadProgram(const std::string& name, ProgramInterface* pi) = 0;
			
        /**
         * Reset execution logic to the beginning of the associated program.
         */
        virtual bool resetProgram(const std::string& name) = 0;

        /**
         * Start execution mode.
         */
        virtual bool startProgram(const std::string& name) = 0;

        /**
         * Start stepping mode.
         */
        virtual bool startStepping(const std::string& name) = 0;

        /**
         * End stepping or execution mode.
         */
        virtual bool stopProgram(const std::string& name) = 0;
            
//         /**
//          * Start configuration mode.
//          */
//         virtual bool startConfiguration() = 0;
                        
//         /**
//          * End configuration mode and start loading mode, only
//          * if a configuration has been loaded.
//          */
//         virtual bool endConfiguration() = 0; 

        /**
         * Discard the data currently loaded.
         */
        virtual bool deleteProgram(const std::string& name) = 0;
            
//         /**
//          * Retreat to init mode (data and configuration were discarded).
//          */
//         virtual bool abort() = 0;

        /**
         * In stepping mode, do the next step.
         */
        virtual bool nextStep(const std::string& name) = 0;

        /**
         * Execute a given command asynchronously
         *
         * @param c The Command to be executed .
         * @return false if not possible or if
         * the previous command was not executed yet.
         */
        virtual bool process( CommandInterface* c) = 0;
    };

}

#endif 
