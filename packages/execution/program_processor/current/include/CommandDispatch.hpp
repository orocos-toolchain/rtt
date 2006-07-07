/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  CommandDispatch.hpp 

                        CommandDispatch.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef EXECUTION_COMMAND_DISPATCH 
#define EXECUTION_COMMAND_DISPATCH 
 
#include "DataSource.hpp"
#include "DispatchInterface.hpp"
#include "corelib/ConditionInterface.hpp"


namespace ORO_Execution
{
    class TryCommand;
    class CommandProcessor;

    /**
     * Dispatch a CommandInterface to a CommandProcessor.
     * The execute() method will return false from the moment
     * on that the dispatched command failed. Hence, this execute()
     * must not wrapped in an AsyncCommandDecorator, but directly
     * executed by the caller.
     */
    class CommandDispatch
        : public DispatchInterface
    {
        bool send;
        bool maccepted;
        CommandProcessor* proc;
        ORO_CoreLib::CommandInterface* com;
        ORO_CoreLib::ConditionInterface* mcn;

        /**
         * Helper class which is sent to the CommandProcessor
         * in order to execute the command.
         */
        struct Dispatcher : public CommandInterface {
            bool mexecuted, mvalid;
            CommandDispatch* cd;
            Dispatcher(CommandDispatch* c) : mexecuted(false), mvalid(true), cd(c) {}
            void readArguments() {}
            bool execute() { mexecuted=true; return mvalid=cd->com->execute();}
            void reset() { mexecuted=false; mvalid=true; }
            virtual Dispatcher* clone() const { return new Dispatcher(cd); }
        } dispatcher;
            
    public:
        /**
         * Create a command to dispatch another command \a c to a CommandProcessor \a p.
         */
        CommandDispatch(CommandProcessor* p, CommandInterface* c, ORO_CoreLib::ConditionInterface* cn );

        /**
         * Be sure only to delete this command if the target processor is
         * not processing the encapsulated command.
         */
        ~CommandDispatch();

        void readArguments();

        bool execute();

        /**
         * Dispatch a command. If it is not accepted, fail, if it is accepted,
         * return the dispatched command's result status.
         */
        bool dispatch();

        /**
         * After reset(), another attempt to dispatch
         * the command will be made upon execute().
         */
        void reset();

        /**
         * Returns true if the command was sent to the CommandProcessor.
         * You can use this flag to check whether execute() was invoked.
         */
        bool sent() const;

        /**
         * Returns true if the command was accepted when sent to the CommandProcessor.
         * A Command is accepted when the CommandProcessor was running and its queue
         * was not full.
         */
        bool accepted() const;

        /**
         * Returns true if the command was executed by the CommandProcessor.
         * When executed() is true, you can check its result().
         */
        bool executed() const;

        /**
         * Returns true if the command was valid, i.e. the command itself
         * was executed and returned true. 
         */
        bool valid() const;

        /**
         * Evaluate if the command is done.
         * @retval true if accepted(), valid() was true and the
         * completion condition was true as well.
         * @retval false otherwise.
         */
        bool evaluate() const;

        virtual ORO_CoreLib::ConditionInterface* createCondition() const;

        ORO_Execution::DispatchInterface* clone() const;

        ORO_Execution::DispatchInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const;
    };
}

#endif
