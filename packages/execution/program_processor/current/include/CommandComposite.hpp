
#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{
	using ORO_CoreLib::CommandInterface;
	using std::vector;
	
    /**
     * Based on the software pattern 'composite', this
     * class allows composing command objects into one command object
     */
    class CommandComposite
        : public CommandInterface
    {
        public:
        CommandComposite( const CommandComposite& orig )
        {
            for (vector<CommandInterface*>::iterator zjakky=list.begin();zjakky!=list.end();zjakky++)
                add( (*zjakky)->clone() );
        }

        virtual ~CommandComposite() {
            for (vector<CommandInterface*>::iterator zjakky=list.begin();zjakky!=list.end();zjakky++) {
                CommandInterface * cif=*zjakky;
                delete cif;
            }
        }

            /**
             * Execute the functionality of all commands.
             * Commands will be executed in the order they have been added
             */
            virtual void execute() {
            	for (vector<CommandInterface*>::iterator zjakky=list.begin();zjakky!=list.end();zjakky++) {
            		(*zjakky)->execute();
            	}
			};

            /**
             * Return a std::string identifying the name
             * of this Command.
             */
            virtual std::string toString() {
            		return "CommandComposite";
            };
        
        /**
         * add a command to the list
         * 
         */
        virtual int add(CommandInterface * com) {		
            list.push_back(com);
        };

        virtual CommandInterface* clone() const
        {
            return new CommandComposite( *this );
        }
			
    private:
        vector<CommandInterface*> list;
    };

}
