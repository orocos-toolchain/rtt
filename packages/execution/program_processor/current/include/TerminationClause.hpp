#ifndef TERMINATIONCLAUSE_HPP
#define TERMINATIONCLAUSE_HPP

#include "ProgramNode.hpp"

namespace ORO_CoreLib
{
	class ConditionInterface;
}


namespace ORO_Execution
{
	using namespace ORO_CoreLib;

	/**
	 * This class represents a conditional branch in a program
	 * tree.  It contains a condition and a program node.
	 */
	class TerminationClause
	{

		public:

			/**
             * TerminationClause specifies a condition to
             * check, and a ProgramNode* to go to next
             * when the condition becomes true.
             * This TerminationClause takes ownership of cond, and will
             * keep a reference to the ProgramNode.
             */
			TerminationClause(ConditionInterface* cond, ProgramNode* node);

			virtual ~TerminationClause();


            /**
             * Reset the condition in this
             * TerminationClause.  See the documentation
             * of ConditionInterface::reset() for more
             * details..
             */
            virtual void reset();

			/**
			 * Evaluates the condition coupled with this termination clause.
			 */
			virtual bool evaluate();

			/**
			 * Return the condition coupled with this termination clause.
			 */
			virtual ConditionInterface* getCondition();

			/**
			 * Return the program node coupled with this termination clause.
			 */
			virtual ProgramNode* getProgramNode();

		protected:

			virtual void setCondition(ConditionInterface* cond);
			virtual void setProgramNode(ProgramNode* node);

		private:
			ProgramNode::shared_ptr programNode;
			ConditionInterface* condition;

	};

}

#endif
