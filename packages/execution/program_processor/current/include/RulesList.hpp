/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  RulesList.hpp 

                        RulesList.hpp -  description
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
 
 
#ifndef RULESLIST_HPP
#define RULESLIST_HPP

#include <vector>

namespace ORO_Execution
{

	class Rule;

	/**
	 * This abstract class represents a list containing rules.
	 */
	class RulesList
	{

		public:

        	virtual ~RulesList();

			/**
			 * Add given rule to this list.  The list
			 * takes ownership of the rule.
			 */
			virtual void addRule(Rule* rule);

			/**
			 * Check the rules in this list.
			 *
			 * @return True, if there is a rule which is checked to be true.
			 */
			virtual bool check()=0;

		protected:
			//only for derived classes
			RulesList();

		  	std::vector<Rule*>* rules;

	};

}

#endif


