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
			ProcessorStateLoad(Processor* proc, SystemContext* sc);
			
			virtual ~ProcessorStateLoad();
			
			
			/**
			 * Leave the system unconfigured and the program undefined
			 * and put the associated processor in init mode.
			 */
			virtual void abort();
			
			/**
			 * Leave the program undefined and put the 
			 * associated processor in configuration mode.
			 */
			virtual void deleteProgram();
			
			/**
			 * If a valid program is defined, put the 
			 * associated processor in execution mode.
			 */
			virtual void startExecution();
			
			/**
			 * Loads given program if it is valid.
			 * @param <pi> The program to be loaded.
			 * @post If given program was valid, it was loaded.
			 */
			virtual void loadProgram(ProgramInterface* pi);	
			
			/**
			 * Reload the beginning of the program. 
			 */
			virtual void resetProgram(); 
			
			
			//rest of interface
            virtual void startConfiguration(){output("startConfiguration : Processor is in load state");}
            virtual void endConfiguration(){output("endConfiguration : Processor is in load state");}
			virtual void loadSystemContext(SystemContext* sc){output("loadSystemContext : Processor is in load state");}  
			virtual void doStep(){output(" doStep : Processor is in load state");}  
			virtual void stopExecution(){output("stopExecution : Processor is in load state");}
			
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
			SystemContext* systemContext;
			/**
			 * The program associated with this state.
			 */
			ProgramInterface* program;	  
	};

}

#endif

