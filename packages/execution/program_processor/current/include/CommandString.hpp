#include <corelib/CommandInterface.hpp>

#include <string>

namespace ORO_Execution
{
	using ORO_CoreLib::CommandInterface;

	/**
	 * This command displays a string when executed. 
     * (Currently solely used by jilske for testing purposes.)
	 */
	class CommandString : public CommandInterface
	{
		
		public:
		
			CommandString(std::string s);

   	    	virtual ~CommandString()
   	    	{}	

    	    virtual void execute();

    	    virtual std::string toString();
        
        private:
        	std::string command;
                
	};

}
