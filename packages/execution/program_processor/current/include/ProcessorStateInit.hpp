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
        virtual bool loadSystemContext(SystemContext* sc){output("loadSystemContext : No configuration defined yet"); return false;} 
        virtual bool loadProgram(ProgramInterface* pi){output("loadProgram : No configuration defined yet"); return false;}
        virtual bool resetProgram(){output("resetProgram : No configuration defined yet"); return false;}
        virtual void doStep(){}
	
	};

}


#endif

