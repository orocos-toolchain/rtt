#ifndef PROGRAMINTERFACE_HPP
#define	PROGRAMINTERFACE_HPP


namespace ORO_Execution
{
	
	/**
	 * This abstract class represents a collection of 
	 * data that can be executed.
	 */
	class ProgramInterface
	{
		public:
		
			ProgramInterface()
			{}
	
			virtual ~ProgramInterface()
			{}

			/**
			 * Execute the next logical step of this program interface.
			 */
			virtual void execute()= 0;

			/**
			 * Reset the execution point to the beginning of this program interface.
			 */
			virtual void reset() = 0;
	
	};


}


#endif


