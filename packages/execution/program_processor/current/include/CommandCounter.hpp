#ifndef COMMANDCOUNTER_HPP
#define COMMANDCOUNTER_HPP

#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{

	using  ORO_CoreLib::CommandInterface;
	
    /**
     * This command increments a counter when executed. 
     * (Currently solely used by jilske for testing purposes.)
     */
    class CommandCounter
        : public CommandInterface
    {

        public:
        
            CommandCounter();

            virtual ~CommandCounter();

            virtual void execute();

            virtual std::string toString();
            
            /**
             * set number of times executed to zero
             */
            virtual void reset();
            
            /**
             * return the number of times this command has been 
             * executed since the last reset
             */
            virtual int getCounter();
        
        virtual CommandInterface* clone() const
        {
            return new CommandCounter( *this );
        }

        private:
        	int counter;
    };

}

#endif

