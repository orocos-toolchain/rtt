/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  RulesCheckList.hpp 

                        RulesCheckList.hpp -  description
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
 
 
#ifndef RULESCHECKLIST_HPP
#define RULESCHECKLIST_HPP


#include "RulesList.hpp"

namespace ORO_Execution
{
	/**
	 * This class represents a list of weak rules.
	 * When this list is checked, every rule which 
	 * is true will be executed.
	 */
	class RulesCheckList : public RulesList
	{
		public :
		
			RulesCheckList();
        	RulesCheckList(Rule* rule);
        
        	virtual ~RulesCheckList()
        	{} 	
		
			/**
		 	 * This operation checks every rule. 
		 	 * If a rule is valid, that rule will be
		     * executed.
		     */
			virtual bool check();
	
	};

}

#endif
