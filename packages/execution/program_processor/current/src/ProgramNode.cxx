#include "execution/ProgramNode.hpp"
#include "execution/TerminationClause.hpp"
#include "execution/TerminationClauseList.hpp"

namespace ORO_Execution
{

	ProgramNode::ProgramNode()
          : command( 0 ), clauses( new TerminationClauseList ),
            lineNumber( 0 ), refcount( 0 )
	{
	}

	ProgramNode::ProgramNode(CommandInterface* cmd)
          : command( cmd ), clauses( new TerminationClauseList ),
            lineNumber( 0 ), refcount( 0 )
	{
	}

	ProgramNode::ProgramNode(CommandInterface* cmd,TerminationClause* tc)
          : command( cmd ), clauses( new TerminationClauseList ),
            lineNumber( 0 ), refcount( 0 )
	{
		addTerminationClause(tc);
	}

	ProgramNode::ProgramNode(CommandInterface* cmd,TerminationClauseList* tcl)
          : command( cmd ), clauses( tcl ),
            lineNumber( 0 ), refcount( 0 )
	{
	}

	ProgramNode::~ProgramNode()
	{
		delete command;
		delete clauses;
	}


	ProgramNode* ProgramNode::execute()
	{
		//command executen
		command->execute();

		//condities sjekken
		ProgramNode* result = clauses->evaluate();

		//resultaat teruggeven (this of next)
		if ( result == 0) {result = this;}
		return result;

	}

	CommandInterface* ProgramNode::getCommand()
	{
		return command;
	}

	void ProgramNode::setCommand(CommandInterface* cmd)
	{
		command = cmd;
	}

	void ProgramNode::addTerminationClause(TerminationClause* tc)
	{
		clauses->addClause(tc);
	}

	void ProgramNode::setLineNumber(int ln)
	{
		lineNumber = ln ;
	}

	int ProgramNode::getNbClauses()
	{
		return clauses->size();
	}

	int ProgramNode::getLineNumber()
	{
		return lineNumber;
	}

        void ProgramNode::ref()
        {
                ++refcount;
        }

        void ProgramNode::deref()
        {
                if ( --refcount <= 0 ) delete this;
        }

        void ProgramNode::startExecution()
        {
                command->reset();
                clauses->reset();
        };

}

void intrusive_ptr_add_ref( ORO_Execution::ProgramNode* p )
{
        p->ref();
}

void intrusive_ptr_release( ORO_Execution::ProgramNode* p )
{
        p->deref();
}
