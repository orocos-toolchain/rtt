/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  ProcessorState.hpp 

                        ProcessorState.hpp -  description
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
 
 
#ifndef PROCESSORSTATE_HPP
#define PROCESSORSTATE_HPP

#include <string>

namespace ORO_Execution
{
	
    class SystemContext;
    class ProgramInterface;
    class Processor;
    
    /**
     * An abstract base class representing the internal state of
     * an execution engine (4 possible states-> init, config,
     * load and execute).
     */
    class ProcessorState
    {

        public:
 			
 			virtual ~ProcessorState()
 			{}

            //state transitions
            virtual bool startConfiguration()=0;
            virtual bool endConfiguration()=0;
			virtual bool deleteProgram()=0;
			virtual bool startExecution()=0;
			virtual bool stopExecution() =0;
			virtual bool abort() =0;
			
			//state tasks
			virtual bool loadSystemContext(SystemContext* sc) = 0; 
			virtual bool loadProgram(ProgramInterface* pi) = 0;
			virtual bool resetProgram() = 0; 
			virtual void doStep() = 0;  
			
			

       protected:
       		
       		//only for derived classes
       		ProcessorState(Processor* proc)
                :processor(proc)
        	{}
        	
        	/**
        	 * Corresponding execution engine.
        	 */
			Processor* processor;
			
       		/**
       		 * This function writes the given string to the screen.
       		 */
			virtual void output(std::string _string);
            
            //only for derived classes, states are being hardcoded
            //and can ask for requests themselves
       		virtual void changeState(ProcessorState* newState);
       		virtual void resetState();
       		          	   	

    };

}

#endif 
