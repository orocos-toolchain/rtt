#include "execution/VertexNode.hpp"
#include <corelib/CommandNOP.hpp>

namespace ORO_Execution
{

	VertexNode::VertexNode()
        : command( new ORO_CoreLib::CommandNOP ),
          lineNumber( 0 ),
          label("")
	{
	}

	VertexNode::VertexNode(CommandInterface* cmd)
        : command( cmd ),
          lineNumber( 0 ),
          label("")
	{
	}

	VertexNode::~VertexNode()
	{
		delete command;
	}


	void VertexNode::execute()
	{
		/**
         * Execute the command in any case.
         * If the command is Asynch, this will do nothing.
         */
		command->execute();
	}

	CommandInterface* VertexNode::getCommand() const
	{
		return command;
	}

	CommandInterface* VertexNode::setCommand(CommandInterface* cmd)
	{
        CommandInterface* old = command;
		command = cmd;
        return old;
	}

	void VertexNode::setLineNumber(int ln)
	{
		lineNumber = ln ;
	}

	int VertexNode::getLineNumber() const
	{
		return lineNumber;
	}

    void VertexNode::startExecution()
    {
        // reset the command
        command->reset();
    }

    void VertexNode::setLabel(const std::string& str)
    {
        label = str;
    }

    const std::string& VertexNode::getLabel() const
    {
        return label;
    }
}
