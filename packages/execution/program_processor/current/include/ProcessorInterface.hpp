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

namespace ORO_Execution
{
    using ORO_CoreLib::StateContext;

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
             * Load a configured system 
             */
            virtual bool loadStateContext(StateContext* sc) = 0; 
			
			/**
			 * Load the data to be executed.
			 */
			virtual bool loadProgram(ProgramInterface* pi) = 0;
			
			/**
             * Execute the next logic step of the associated data.
             */
            virtual void doStep() = 0;
            
			/**
			 * Reset execution logic to the beginning of the associated data.
			 */
			virtual bool resetProgram() = 0;              

    };

}

#endif 
