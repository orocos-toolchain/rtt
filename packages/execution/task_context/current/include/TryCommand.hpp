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

#include <corelib/CommandInterface.hpp>
#include <corelib/ConditionInterface.hpp>

namespace ORO_Execution 
{
    using namespace ORO_CoreLib;

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
        VariableDataSource<bool>::shared_ptr _result;
        VariableDataSource<bool>::shared_ptr _executed;
        CommandInterface* c;
    public:
        /**
         * Try a command.
         */
        TryCommand( CommandInterface* command,
                    VariableDataSource<bool>::shared_ptr storage=0,
                    VariableDataSource<bool>::shared_ptr execstat=0 )
            :_result( storage == 0 ? new VariableDataSource<bool>(true) : storage ),
             _executed( execstat == 0 ? new VariableDataSource<bool>(false) : execstat ),
             c(command) {}

        ~TryCommand() {
            delete c;
        }
        bool execute() {
            _result->set( c->execute() );
            _executed->set(true);
            return true;
        }
        void reset() {
            c->reset();
            _result->set(true);
            _executed->set(false);
        }

        VariableDataSource<bool>::shared_ptr result() {
            return _result;
        }

        VariableDataSource<bool>::shared_ptr executed() {
            return _executed;
        }

        CommandInterface* clone() const {
            return new TryCommand( c->clone(),
                                   _result, _executed );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommand( c->copy( alreadyCloned ),
                                   _result->copy(alreadyCloned),
                                   _executed->copy(alreadyCloned) );
        }
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
         * you want to check. If \a invert is \a true (the default), 
         * TryCommandResult::evaluate() will return true if the original command failed.
         * If \a invert is \a false, evaluate() will return the return value of the
         * original command.
         */
        TryCommandResult( DataSource<bool>::shared_ptr ec, bool invert = true )
            :c(ec), _invert(invert) {}

        ~TryCommandResult() {
            // do not delete !
        }

        bool evaluate() {
            // by default true means reject
            return  _invert != c->get();
        }

        ConditionInterface* clone() const {
            return new TryCommandResult( c, _invert ); // do not clone c !
        }

        ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommandResult( c->copy(alreadyCloned), _invert );
        }
    };

    /**
     * Evaluates a DataSource<bool> in a command.
     * @see EvalCommandResult
     */
    class EvalCommand :
        public CommandInterface
    {
        // the result
        VariableDataSource<bool>::shared_ptr _cache;
        // the data to evaluate in the command.
        DataSource<bool>::shared_ptr _ds;
    public:
        EvalCommand( DataSource<bool>::shared_ptr ds, VariableDataSource<bool>::shared_ptr cache=0)
            :_cache( cache == 0 ? new VariableDataSource<bool>(false) : cache ),
             _ds(ds) {}

        ~EvalCommand() {
        }

        bool execute() {
            _cache->set() = _ds->get();
            return true;
        }

        VariableDataSource<bool>::shared_ptr cache() {
            return _cache;
        }

        CommandInterface* clone() const {
            return new EvalCommand( _ds,
                                    _cache );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new EvalCommand( _ds->copy( alreadyCloned ),
                                    _cache->copy(alreadyCloned) );
        }
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
        EvalCommandResult( DataSource<bool>::shared_ptr ec)
            :c(ec) {}

        ~EvalCommandResult() {
            // do not delete !
        }

        bool evaluate() {
            return c->get();
        }

        ConditionInterface* clone() const {
            return new EvalCommandResult( c ); // do not clone c !
        }

        ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new EvalCommandResult( c->copy( alreadyCloned ) );
        }
    };
}
    
#endif
