#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "ProgramInterface.hpp"
#include "ProgramNode.hpp"

namespace ORO_Execution
{

	/**
	 * This class represents a program consisting of
	 * data contained in a program node tree.
	 */
	class Program : public ProgramInterface
	{

		public:

		   /**
		    * Constructs an empty program (NOPprogram)
		    *
		    * @post This program is created with a NOP root.
		    */
		   Program();

		   /**
		    * Constructs a program with a given root node.
		    *
		    * @param <rootNode> The node which will serve as root.
		    * @post Given node is registered as root node for this program
		    * @post Given node is set as the node to be executed next.
		    */
		   Program(ProgramNode* rootNode);


		   virtual ~Program();


		   /**
		    * Executes the next step of this program.
		    */
		   virtual void execute();


		   /**
		    *Reset the next node to be executed to the root node.
		    */
		   virtual void reset();


		protected:

		   //setters
		   virtual void setRootNode(ProgramNode* rootNode);
		   virtual void setCurrentNode(ProgramNode* currentNode);

		private:

		   /**
		    * The node registered as root for this program.
		    */
		   ProgramNode::shared_ptr root;

		   /**
		    * The node which will be executed next.
		    */
		   ProgramNode* current;

                   /**
                    * The node that was run before this one.
                    */
                   ProgramNode* previous;
	};


}

#endif


