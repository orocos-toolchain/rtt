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
			ProcessorStateExec(Processor* proc, SystemContext* sc, ProgramInterface* pi);
			
			virtual ~ProcessorStateExec();
			
			
			/**
			 * Puts the associated processor in init mode.
			 * Stops the execution, unloads the program and 
			 * leaves the system unconfigured.
			 */
			virtual void abort();
			
			/**
			 * Stops execution of associated program and puts
			 * associated processor in load mode.
			 * (does not reset the counter of associated
			 * program to the beginning)
			 */
			virtual void stopExecution();
			
			/**
			 * Stops execution of and deletes the associated program
			 * and puts associated processor in configuration mode.
			 */
			virtual void deleteProgram();
			
			/**
		 	 * Execute the next logic step of associated program. 
		 	 */
			virtual void doStep();			
			
			
            virtual void startConfiguration(){output("no legal action in execution state");}
            
            virtual void endConfiguration(){output("no legal action in execution state");}
			
			virtual void startExecution(){output("no legal action in execution state");}
			
			virtual void loadSystemContext(SystemContext* sc){output("no legal action in execution state");} 
			
			virtual void loadProgram(ProgramInterface* pi){output("no legal action in execution state");}
			
			virtual void resetProgram(){output("no legal action in execution state");} 
			
		private:
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

