/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  SystemState.hpp 

                        SystemState.hpp -  description
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
 
 
#ifndef SYSTEMSTATE_HPP
#define SYSTEMSTATE_HPP

#include "SystemStateInterface.hpp"
#include "Rule.hpp"
#include "RulesCheckList.hpp"
#include "RulesAbortList.hpp"


namespace ORO_Execution
{

	/**
	 * This class represents system states which
	 * are part of an orocos system configuration.
	 * 
	 * @see  base class SystemStateInterface
	 */
	class SystemState : public SystemStateInterface
	{
	
		public:
			
			/**
			 * Constructs an empty system state. 
			 */
			SystemState();
			
			virtual ~SystemState();
			
			
			/*** from StateInterface ***/
			virtual void handle();
			virtual void onEntry();
			virtual void onExit();
			
			
			
			/*** from SystemStateInterface ***/
			virtual void addStrongRule(Rule* r);
			virtual void addWeakRule(Rule* r);
			virtual void checkRules();	
			virtual void setEntryCmd(CommandInterface* c);
			virtual void setExitCmd(CommandInterface* c);		
			
		
		private:
			
			RulesCheckList* checkList;
			RulesAbortList* abortList;
			
			//voorlopig 'rudimentaire' ondersteuning voor configuratie
			CommandInterface* onExitCmd;
			CommandInterface* onEntryCmd;
	};
}


#endif

