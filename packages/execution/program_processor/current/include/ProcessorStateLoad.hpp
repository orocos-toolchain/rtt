/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProcessorStateLoad.hpp 

                        ProcessorStateLoad.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
 
#ifndef PROCESSORSTATELOAD_HPP
#define PROCESSORSTATELOAD_HPP

#include "ProcessorState.hpp"


namespace ORO_Execution
{
	
	/**
	 * This class represents a state of a processor (execution engine). 
	 * In this state a system has been configured and a program
	 * can be loaded.
	 */
	class ProcessorStateLoad : public ProcessorState
	{
		public :
		
			/**
             * Constructs a new processor load state.
             *
             * @post this state is associated with the given processor			 
             * @post a valid system configuration is defined
             * @post no valid program is loaded yet
             */  
			ProcessorStateLoad(Processor* proc, StateContext* sc);
			
			virtual ~ProcessorStateLoad();
			
			/**
			 * Leave the system unconfigured and the program undefined
			 * and put the associated processor in init mode.
			 */
			virtual bool abort();
			
			/**
			 * Leave the program undefined and put the 
			 * associated processor in configuration mode.
			 */
			virtual bool deleteProgram();
			
			/**
			 * If a valid program is defined, put the 
			 * associated processor in execution mode.
			 */
			virtual bool startExecution();
			
			/**
			 * If a valid program is defined, put the 
			 * associated processor in stepping mode.
			 */
			virtual bool startStepping();
			
			/**
			 * Loads given program if it is valid.
			 * @param pi The program to be loaded.
			 * @post If given program was valid, it was loaded.
			 */
			virtual bool loadProgram(ProgramInterface* pi);	
			
			/**
			 * Reload the beginning of the program. 
			 */
			virtual bool resetProgram(); 
			
        /**
         * Step over the System Config.
         */
        virtual void doStep();

        virtual bool process( CommandInterface* c);
		private :
		
			/**
			 * Check if given program is a valid program.
			 * 
			 * @param <p> The program to be checked.
			 * @return True if given program is not zero
			 */
			virtual bool isValidProgram(ProgramInterface* p);
		
			/**
			 * The system configuration associated with this state.
			 */
			StateContext* systemContext;
			/**
			 * The program associated with this state.
			 */
			ProgramInterface* program;	  

        CommandInterface* command;
	};

}

#endif

