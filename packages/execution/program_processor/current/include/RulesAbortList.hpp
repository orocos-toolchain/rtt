/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  RulesAbortList.hpp 

                        RulesAbortList.hpp -  description
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
 
 
#ifndef RULESABORTLIST_HPP
#define RULESABORTLIST_HPP


#include "RulesList.hpp"

namespace ORO_Execution
{
	
	/**
	 * This class represents a list of strong rules.
	 * When this list is checked and a rule is found
	 * which is violated, the checking operation will be 
	 * aborted.
	 */
	class RulesAbortList : public RulesList
	{
		public :
		
			RulesAbortList();
        
        	RulesAbortList(Rule* rule);
        
        	virtual ~RulesAbortList()
        	{} 	
		
			/**
		 	 * This operation checks every rule untill
		  	 * a rule is found which is violated.  Then that rule is 
		 	 * executed and the function returns true (if no
		 	 * rule is violated, the function returns false).
		 	 */
			virtual bool check();
	
	};

}

#endif
