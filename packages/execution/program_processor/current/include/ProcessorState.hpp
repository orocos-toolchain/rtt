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
#include <corelib/StateContext.hpp>
#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{
    using ORO_CoreLib::StateContext;
    using ORO_CoreLib::CommandInterface;

    class ProgramInterface;
    class Processor;
    
    /**
     * A base class representing the internal state of
     * an execution engine. It offers the default no-op
     * implementations for each state. Overriding a method
     * allows a state to specify what to do.
     */
    class ProcessorState
    {

        public:
 			
 			virtual ~ProcessorState()
 			{}

            //state transitions
            virtual bool startConfiguration();
            virtual bool endConfiguration();
			virtual bool deleteProgram();
			virtual bool startExecution();
			virtual bool startStepping();
			virtual bool stopExecution();
			virtual bool abort();
			
			//state tasks
			virtual bool loadStateContext(StateContext* sc); 
			virtual bool loadProgram(ProgramInterface* pi);
			virtual bool resetProgram(); 
			virtual void doStep();  
			virtual bool nextStep();  
			virtual bool process( CommandInterface* c);  

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
			virtual void output(const std::string& _string);
            
            //only for derived classes, states are being hardcoded
            //and can ask for requests themselves
       		virtual void changeState(ProcessorState* newState);
       		virtual void resetState();
       		          	   	

    };

}

#endif 
