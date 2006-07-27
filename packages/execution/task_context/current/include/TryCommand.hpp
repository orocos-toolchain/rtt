/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TryCommand.hpp 

                        TryCommand.hpp -  description
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
 
 
#ifndef ORO_TRY_COMMAND_HPP
#define ORO_TRY_COMMAND_HPP

#include "CommandInterface.hpp"
#include "ConditionInterface.hpp"
#include "Logger.hpp"
#include "DataSource.hpp"

namespace RTT 
{
    
    

    /**
     * A command which tries another command
     * and stores the result in a DataSource<bool>.
     * @see TryCommandResult.
     */
    class TryCommand :
        public CommandInterface
    {
        // we must use a DataSource for correct
        // copy sementics ...
        AssignableDataSource<bool>::shared_ptr _result;
        CommandInterface* c;
    public:
        /**
         * Try a command.
         */
        TryCommand( CommandInterface* command,
                    AssignableDataSource<bool>::shared_ptr storage=0);

        ~TryCommand();

        bool execute();
        void reset();

        bool valid() const;

        void readArguments();

        CommandInterface* theCommand() const;

        AssignableDataSource<bool>::shared_ptr result();

        TryCommand* clone() const;

        TryCommand* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

    /**
     * Returns the (accept/reject) status
     * of another command.
     * @see TryCommand
     */
    class TryCommandResult :
        public ConditionInterface
    {
        DataSource<bool>::shared_ptr c;
        bool _invert;
    public:
        /**
         * Pass TryCommand::result() to the first parameter of the command
         * you want to check. If \a invert is \a true, 
         * TryCommandResult::evaluate() will return true if the original command failed.
         * If \a invert is \a false, evaluate() will return the return value of the
         * original command.
         */
        TryCommandResult( DataSource<bool>::shared_ptr ec, bool invert);

        ~TryCommandResult();

        bool evaluate();

        ConditionInterface* clone() const;

        ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

    /**
     * Evaluates a DataSource<bool> in a command. The result will be evaluated
     * in a EvalCommandResult, so this Command returns always true : ie the evaluation
     * itself always succeeds. An EvalCommand should never be dispatched, since the
     * EvalCommandResult assumes the EvalCommand has been executed when evaluated.
     * @see EvalCommandResult
     */
    class EvalCommand :
        public CommandInterface
    {
        // the result
        AssignableDataSource<bool>::shared_ptr _cache;
        // the data to evaluate in the command.
        DataSource<bool>::shared_ptr _ds;
    public:
        EvalCommand( DataSource<bool>::shared_ptr ds, AssignableDataSource<bool>::shared_ptr cache=0);

        ~EvalCommand();

        void readArguments();

        bool execute();

        void reset();

        AssignableDataSource<bool>::shared_ptr cache();

        CommandInterface* clone() const;

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

    /**
     * The result of a command which evaluates
     * a boolean DataSource.
     * @see EvalCommand
     */
    class EvalCommandResult :
        public ConditionInterface
    {
        DataSource<bool>::shared_ptr c;
    public:
        EvalCommandResult( DataSource<bool>::shared_ptr ec);

        ~EvalCommandResult();

        bool evaluate();

        ConditionInterface* clone() const;

        ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };
}
    
#endif
