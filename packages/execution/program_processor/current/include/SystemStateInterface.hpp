#ifndef SYSTEMSTATEINTERFACE_HPP
#define SYSTEMSTATEINTERFACE_HPP

#include <corelib/StateInterface.hpp>
#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{

	using namespace ORO_CoreLib;
	
	class Rule;
	
	/**
	 * This abstract class represents a state interface for
	 * states representing an orocos system configuration.
	 * Each state interface implementing this class must be
	 * coupled with a list of strong rules (abortlist) and 
	 * a list of weak rules (checklist). 
	 * 
	 * @see  base class StateInterface
	 */
 	class SystemStateInterface : public StateInterface
 	{
 		
 		public :
 		
 		/**
 		 * Check all rules belonging to this StateInterface.
 		 */
 		virtual void checkRules() = 0;
 		
 		/**
 		 * Add given strong rule (rule that must not be violated) 
 		 * to this StateInterface.
 		 */
		virtual void addStrongRule(Rule* r) = 0;
		
		/**
		 * Add given weak rule (rule which is simply executed when valid)
		 * to this StateInterface.
		 */	
		virtual void addWeakRule(Rule* r) = 0;
		
		/**
		 * Set the command to be executed upon entering this 
		 * StateInterface to the givend command.
		 */
		virtual void setEntryCmd(CommandInterface* c) = 0;
		
		/**
		 * Set the command to be executed upon exiting this 
		 * StateInterface to the givend command.
		 */	
		virtual void setExitCmd(CommandInterface* c) = 0;
 		
 	};

}

#endif

