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
        TryCommand* com;

        /**
         * Create a command to dispatch another command \a c to a CommandProcessor \a p.
         * Used exclusively by clone().
         */
        CommandDispatch(CommandProcessor* p, TryCommand* c );

    public:
        /**
         * Create a command to dispatch another command \a c to a CommandProcessor \a p.
         */
        CommandDispatch(CommandProcessor* p, CommandInterface* c );

        /**
         * Be sure only to delete this command if the target processor is
         * not processing the encapsulated command.
         */
        ~CommandDispatch();

        void readArguments();

        /**
         * Dispatch a command. If it is not accepted, fail, if it is accepted,
         * return the dispatched command's result status.
         */
        bool execute();

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
         * Creates a Condition which evaluates (executed() \a and result() ).
         * Thus it will only return true when the CommandProcessor executed
         * the dispatched command \a and it was valid.
         */
        ORO_CoreLib::ConditionInterface* createValidCondition() const;

        ORO_Execution::DispatchInterface* clone() const;

        ORO_Execution::DispatchInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const;
    };
}

#endif
