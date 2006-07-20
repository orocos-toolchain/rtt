#ifndef ORO_TASK_COMMAND_DS_HPP
#define ORO_TASK_COMMAND_DS_HPP

#include <boost/function.hpp>
#include <string>
#include "CommandProcessor.hpp"
#include "BindStorage.hpp"
#include "UnMember.hpp"

namespace ORO_Execution
{
    /**
     * A Command storage container.
     */
    template<class CommandT>
    class CommandDS
        : private detail::BindStorage<CommandT>
    {
    protected:
        std::string mname;

        CommandProcessor* mcp;

        bool minvert;
    public:
        typedef CommandT Signature;

        /** 
         * Create a Command object which executes a plain 'C' function.
         * 
         * @param name The name of this command.
         * @param com A pointer to the 'C' function to execute when the command is invoked.
         * @param con A pointer to the 'C' function that evaluates if the command is done.
         * @param commandp The CommandProcessor which will execute this Command.
         * @param invert Invert the result of \a con when evaluating the completion of the command.
         */
        template<class CommandF, class ConditionF>
        CommandDS(std::string name, CommandF com, ConditionF con, CommandProcessor* commandp, bool invert = false)
            : mname(name),
              mcp( commandp ),
              minvert(invert)
        {
            this->setup(boost::function<CommandT>(com), boost::function<CommandT>(con) );
        }

        boost::function<CommandT> getCommandFunction() const {
            return this->command();
        }

        boost::function<CommandT> getConditionFunction() const {
            return this->condition();
        }

        /** 
         * Returns true if the condition is inverted.
         * 
         * @return true if inverted
         */
        bool isInverted() const {
            return minvert;
        }

        /** 
         * Returns a pointer to the CommandProcessor which will
         * process this command.
         * 
         * @return the pointer.
         */
        CommandProcessor* getCommandProcessor() const {
            return mcp;
        }

        /** 
         * Returns the name of this Command object.
         * 
         * @return the name.
         */
        const std::string& getName() const {
            return mname;
        }
        
    };

    template<class ComF, class ConF>
    CommandDS<typename detail::ArgMember<ComF>::type > command_ds(std::string name, ComF command, ConF condition, CommandProcessor* cp, bool invert = false) {
        return CommandDS<typename detail::ArgMember<ComF>::type >(name, command, condition, cp, invert);
    }


}
#endif
