#include <rtstl/rtstl.hpp>
#include "CommandInterface.hpp"

namespace ORO_Execution
{
	using namespace ORO_CoreLib;
	using namespace std;
	
    /**
     * Based on the software pattern 'composite', this
     * class allows composing command objects into one command object
     */
    class CommandComposite: public CommandInterface
    {
   			
        public:
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
            
            /*for debugging only*/
          	//TODO: onderstaande debuggen :-D
/*            virtual std::string debugString() {
            
            	std::string str();
				for (vector<CommandInterface*>::iterator it=list.begin(); it!=list.end(); it++) {
            		str+=(*it)->toString();
            		 const char* ad = "  + ";
					 str += ad;
            	}
				return *str;
				
            }
*/
            

			/* composite specific calls */
			
			/*
			 * add a command to the list
			 * 
			 */
			virtual int add(CommandInterface * com) {		
				list.push_back(com);
			};
			
		private:
		
   			vector<CommandInterface*> list;
			
			
			
    };

}
