#ifndef PROCESSORSTATECONFIG_HPP
#define PROCESSORSTATECONFIG_HPP

#include "ProcessorState.hpp"

namespace ORO_Execution
{
	
	/**
	 * This class represents the configuration state
	 * of a processor (execution engine). 
	 * In this state a system can be configured.
	 */
	class ProcessorStateConfig : public ProcessorState 
	{
		
		public :
			
			/**
             * Constructs a new processor configuration state.
             *
             * @post this state is associated with the given processor			 
             * @post no valid system configuration is defined yet
             *       (systemContext == 0)
             */  
			ProcessorStateConfig(Processor* proc);
			
			virtual ~ProcessorStateConfig();
			
			
			/**
			 * Leave the system unconfigured and put the associated
			 * processor in init mode.
			 */
			virtual void abort();
			
			/**
			 * If a valid configuration has been defined, put the
			 * associated processor in load mode. 
			 */
			virtual void endConfiguration(); 
			
			/**
			 * Loads a given valid system configuration.
			 * 
			 * @param <sc> The system configuration to be loaded. 			   
			 * @post If given system configuration is valid, it is loaded.		
			 */
			virtual void loadSystemContext(SystemContext* sc);
			
			
			
            virtual void startConfiguration(){output("already in configuration state");}           
			
			virtual void deleteProgram(){output("no program defined in configuration state");}
			
			virtual void startExecution(){output("no start execution in configuration state");}
			
			virtual void stopExecution(){output("no stop execution in configuration state");}
 			
 			virtual void loadProgram(ProgramInterface* pi){output("no loading in configuration state");}
			
			virtual void resetProgram(){output("no legal action in configuration state");} 
			
			virtual void doStep(){output("no execution in configuration state");}
			
		private :
		
			/**
			 * Check if given system configuration is a valid
			 * configuration.
			 * 
			 * @param <sc> The system configuration to be checked.
			 * @return True if given system configuration is not zero
			 */
			virtual bool isValidConfig(SystemContext* sc);
			
			/**
			 * The system configuration associated with this state.
			 */
			SystemContext* systemContext;	   
	
	};


}

#endif

