#ifndef PROGRAMNODE_HPP
#define PROGRAMNODE_HPP


#include <corelib/CommandInterface.hpp>
#include "CommandStopExecution.hpp"

#include <boost/intrusive_ptr.hpp>

namespace ORO_Execution
{

	class ProcessorControlInterface;
	class ProcessorInterface;
    class TerminationClause;
    class TerminationClauseList;

	/**
	 * This class represents elements in a program tree.
	 * A program node consists of a command to be executed
	 * and a list of termination clauses containing conditional
	 * branches to other program nodes.
         * ProgramNode's are reference counted.  Use
         * ProgramNode::shared_ptr to hold a reference to them.
	 */
	class ProgramNode
	{

		public:
                // use this pointer as a shared pointer to this class
                typedef boost::intrusive_ptr<ProgramNode> shared_ptr;

			/**
			 * Construct an empty program node with no command,
			 * no conditional branches and line number 0.
			 */
			ProgramNode();

			/**
			 * Construct a program node with given command,
			 * no conditional branches and line number 0.
			 */
		    ProgramNode(CommandInterface* cmd);

		    /**
		     * Construct a program node with given command, given
		     * conditional branch and line number 0.
		     */
		    ProgramNode(CommandInterface* cmd,TerminationClause* tc);

		    /**
		     * Construct a program node with given command, given
		     * list of conditional branches and line number 0.
		     */
		    ProgramNode(CommandInterface* cmd,TerminationClauseList* tcl);


		    /**
		     * Returns a special program node that contains no clauses
		     * and a special stopExecution command.
		     * (all the leaves of a program tree are this type of program node.
		     *
		     * @param <proc> The engine executing this node (which will be stopped)
		     * @return A program node with no conditional branches and that
		     * contains a stop execution command.
		     */
		    static ProgramNode* leafNode( ProcessorControlInterface* proc)
		    {
		    	CommandStopExecution* cmd = new CommandStopExecution( proc );
		    	ProgramNode* result = new ProgramNode( cmd);
		    	return (result);
		    }



		    virtual ~ProgramNode();

            /**
             * This means that the execution engine has just
             * started to execute this node, after some other
             * node has sent it to this one.  If the node is
             * simply executed again, because it returned
             * itself from its execute() method, then this
             * method will not be called.  The reason this is
             * here is because the node has to reset its
             * termination conditions at this point.  See the
             * documentation of ConditionInterface::reset()
             * for more information about this..
             */
            virtual void startExecution();

		    /**
		     * Executes the command currently associated with this
		     * node and returns the program node to be executed
		     * next (either itself, or one from the conditional branches
		     * associated with this node).
		     *
		     * @return Either itself, or a program node in one of the
		     * termination clauses associated with this node.
		     */
		    virtual ProgramNode* execute();

		    /**
		     * Set the command associated with this program node
		     * to the given command.
		     */
		    virtual void setCommand(CommandInterface*);

		    /**
		     * Add given termination clause to the list of
		     * termination clauses for this node.
		     */
		    virtual void addTerminationClause(TerminationClause*);

			/**
			 * Set line number of this program node to given line number.
			 *
			 * @param <ln> The new line number for this node
			 * @post The line number of this node equals the given integer.
			 */
			virtual void setLineNumber(int ln);

			/**
			 * Returns the command currently associated with
			 * this node.
			 * @return The command currently associated with
			 * this node
			 */
			virtual CommandInterface* getCommand();

			/**
			 * Returns the number of termination clauses currently
			 * associated with this node.
			 * @return The number of termination clauses currently
			 * associated with this node
			 */
			virtual int getNbClauses();

			/**
			 * Returns the program line number currently associated
			 * with this program node (default = zero).
			 */
			virtual int getLineNumber();

                // ProgramNode's are reference counted.  Below are the
                // relevant functions.  You are encouraged to not use
                // this directly, but instead use
                // ProgramNode::shared_ptr
                // take a reference to this ProgramNode
                void ref();
                // release a reference to this ProgramNode.
                void deref();
		private:

		 	/**
		 	 * The command to be executed in this node.
		 	 */
		 	CommandInterface* command;

		 	/**
		 	 * List of termination clauses (containing
		 	 * conditional branches to other program nodes)
		 	 */
		 	TerminationClauseList* clauses;

			/**
			 * The line number associated with this node
			 * (default = null, also when line numbers are
			 *  not employed).
			 */
			int lineNumber;



                // our reference count
                int refcount;
	};
}

void intrusive_ptr_add_ref( ORO_Execution::ProgramNode* p );
void intrusive_ptr_release( ORO_Execution::ProgramNode* p );

#endif



