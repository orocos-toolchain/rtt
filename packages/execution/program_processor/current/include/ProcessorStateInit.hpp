/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProcessorStateInit.hpp 

                        ProcessorStateInit.hpp -  description
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
 
 
#ifndef PROCESSORSTATEINIT_HPP
#define PROCESSORSTATEINIT_HPP

#include "ProcessorState.hpp"

namespace ORO_Execution
{	
	
	/**
	 * This class represents the initial/idle state of
	 * a processor (execution engine). In this state the 
	 * system has not yet been configured and no program has 
	 * been loaded.
	 */
	class ProcessorStateInit : public ProcessorState
	{	
    public:
		
        /**
         * Constructs a new processor  init state.
         *
         * @post this state is associated with the given processor			 
         */  
        ProcessorStateInit(Processor* proc);
			
        virtual ~ProcessorStateInit()
        {}
			
        /**
         * Put the associated processor in configuration mode.
         */
        virtual bool startConfiguration();
			
        //rest of interface
        virtual bool endConfiguration(){output("endConfiguration : No configuration defined yet"); return false;}
        virtual bool deleteProgram(){output("deleteProgram : No configuration defined yet"); return false;}
        virtual bool startExecution(){output("startExecution : No configuration defined yet"); return false;}
        virtual bool stopExecution(){output("stopExecution : No configuration defined yet"); return false;}
        virtual bool abort(){output("abort : Already in init state"); return false;}
        virtual bool loadStateContext(StateContext* sc){output("loadStateContext : No configuration defined yet"); return false;} 
        virtual bool loadProgram(ProgramInterface* pi){output("loadProgram : No configuration defined yet"); return false;}
        virtual bool resetProgram(){output("resetProgram : No configuration defined yet"); return false;}
        virtual void doStep(){}
	
	};

}


#endif

