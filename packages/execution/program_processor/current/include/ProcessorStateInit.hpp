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
			virtual void startConfiguration();
			
			//rest of interface
            virtual void endConfiguration(){output("endConfiguration : No configuration defined yet");}
			virtual void deleteProgram(){output("deleteProgram : No configuration defined yet");}
			virtual void startExecution(){output("startExecution : No configuration defined yet");}
			virtual void stopExecution(){output("stopExecution : No configuration defined yet");}
			virtual void abort(){output("abort : Already in init state");}
			virtual void loadSystemContext(SystemContext* sc){output("loadSystemContext : No configuration defined yet");} 
			virtual void loadProgram(ProgramInterface* pi){output("loadProgram : No configuration defined yet");}
			virtual void resetProgram(){output("resetProgram : No configuration defined yet");}
			virtual void doStep(){output("doStep : No configuration defined yet");}   
	
	};

}


#endif

