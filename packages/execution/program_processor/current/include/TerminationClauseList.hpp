#ifndef TERMINATIONCLAUSELIST_HPP
#define TERMINATIONCLAUSELIST_HPP


#include "TerminationClause.hpp"

#include <vector>

namespace ORO_Execution
{

	/**
	 * This class represents a list containing termination clauses.
	 * When this list is evaluated it returns the program node coupled
	 * with the first termination clauses that evaluates true.
	 */
	class TerminationClauseList
	{
		public:

			/**
			 * Construct an empty list.
			 */
			TerminationClauseList();

			/**
			 * Construct a list with the given clause.
			 */
			TerminationClauseList(TerminationClause* clause);

			virtual ~TerminationClauseList();


			/**
			 * Add given clause to this list.
			 */
			virtual void addClause(TerminationClause* clause);


                        /**
                         * Reset the termination clauses in this
                         * list.  See the documentation of
                         * ConditionInterface::reset() for more
                         * details..
                         */
                        virtual void reset();

			/**
			 * Evaluates this list and returns the program node
			 * of the first termination clause that evaluates true
			 * (or zero if no clauses are found valid).
			 */
			virtual ProgramNode* evaluate();

			/**
			 * Checks the clauses in this list and returns
			 * a pointer to the first clause that evaluates true
			 * (or 0 in case no clauses are fulfilled).
			 */
			virtual TerminationClause* check();

			/**
			 * Return the number of clauses in this list.
			 */
			virtual int size();


		private:
			std::vector<TerminationClause*>* clauses;

	};

}

#endif
