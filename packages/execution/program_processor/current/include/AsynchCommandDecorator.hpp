#ifndef ASYNCHCOMMANDDECORATOR_HPP
#define ASYNCHCOMMANDDECORATOR_HPP


#include <corelib/CommandInterface.hpp>


namespace ORO_Execution
{

	using namespace ORO_CoreLib;

    /**
     * Based on the software patterns 'command' and
     * 'decorator', this interface represents a
     * wrapper around a given command, which decorates
     * that command as an asynchronous command.
     */
    class AsynchCommandDecorator: public CommandInterface
    {

    public:

        /**
         * Construct a new decorator with given command.
         *
         * @pre cmd != 0
         * @param <cmd> The command to be decorated
         * @post The given command is decorated as an
         * 		 asynchronous command.
         */
        AsynchCommandDecorator(CommandInterface* cmd);

        AsynchCommandDecorator(const AsynchCommandDecorator& orig);

        virtual ~AsynchCommandDecorator();

        /**
         * Execute the functionality of the corresponding command.
         */
        virtual void execute();

        /**
         * Return a std::string identifying the name
         * of this Command.
         */
        virtual std::string toString();

        virtual void reset();

        const CommandInterface* getCommand() const;

        virtual CommandInterface* clone() const;
    private:

        /**
         * Asynchronous command being decorated.
         */
        CommandInterface* command;

        /**
         * Variable indicating whether corresponding command
         * has been executed or not.
         */
        bool executed;

    };

}

#endif
