/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProcessorStateExec.hpp 

                        ProcessorStateExec.hpp -  description
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
 
 
#ifndef PROCESSORSTATEEXEC_HPP
#define PROCESSORSTATEEXEC_HPP

#include "ProcessorState.hpp"

namespace ORO_Execution
{
		
	
	/**
	 * This class represent a state of a processor 
	 * (execution engine). In this state a program
	 * can be executed (within a configured system). 
	 */
	class ProcessorStateExec : public ProcessorState
	{
		public:
		
			/**
             * Constructs a new processor execution state.
             *
             * @post this state is associated with the given processor			 
             * @post a valid system configuration is defined
             * @post a valid program is defined
             */ 
			ProcessorStateExec(Processor* proc, StateContext* sc, ProgramInterface* pi);
			
			virtual ~ProcessorStateExec();
			
			
			/**
			 * Puts the associated processor in init mode.
			 * Stops the execution, unloads the program and 
			 * leaves the system unconfigured.
			 */
			virtual bool abort();
			
			/**
			 * Stops execution of associated program and puts
			 * associated processor in load mode.
			 * (does not reset the counter of associated
			 * program to the beginning)
			 */
			virtual bool stopExecution();
			
			/**
			 * Stops execution of and deletes the associated program
			 * and puts associated processor in configuration mode.
			 */
			virtual bool deleteProgram();
			
			/**
		 	 * Execute the next logic step of associated program. 
		 	 */
			virtual void doStep();			
			
			
            virtual bool startConfiguration(){output("startConfiguration: no legal action in execution state"); return false;}
            
            virtual bool endConfiguration(){output("endConfiguration: no legal action in execution state"); return false;}
			
			virtual bool startExecution(){output("startExecution: no legal action in execution state"); return false;}
			
			virtual bool loadStateContext(StateContext* sc){output("loadStateContext: no legal action in execution state"); return false;} 
			
			virtual bool loadProgram(ProgramInterface* pi){output("loadProgram: no legal action in execution state"); return false;}
			
			virtual bool resetProgram(){output("resetProgram: no legal action in execution state"); return false;} 
			
		private:

			/**
			 * The system configuration associated with this state.
			 */
			StateContext* systemContext;
			/**
			 * The program associated with this state.
			 */
			ProgramInterface* program;				
	
	};

}

#endif

