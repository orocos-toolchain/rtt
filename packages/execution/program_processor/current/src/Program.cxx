#include "execution/Program.hpp"
#include "execution/ProgramNode.hpp"
#include "corelib/CommandNOP.hpp"
#include "execution/TerminationClause.hpp"
#include "corelib/ConditionFalse.hpp"

namespace ORO_Execution
{

	Program::Program()
          : root( new ProgramNode )
	{
		root->setCommand(new CommandNOP());
		root->addTerminationClause(new TerminationClause(new ConditionFalse(), root.get()));
		reset();
	}

	Program::Program(ProgramNode* rootNode)
	{
		//init();
		setRootNode(rootNode);
		reset();
	}


	Program::~Program()
	{
	}

	void Program::execute()
	{
                if ( previous != current )
                        current->startExecution();
                previous = current;
		ProgramNode* next = current->execute();
		setCurrentNode(next);
	}

	void Program::reset()
	{
		current = root.get();
		previous = 0;
	}

	void Program::setRootNode(ProgramNode* rootNode)
	{
                root = rootNode;
	}

	void Program::setCurrentNode(ProgramNode* currentNode)
	{
		current = currentNode;
	}
}

