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
        CommandInterface* c;
    public:
        TryCommand( CommandInterface* command, VariableDataSource<bool>::shared_ptr storage=0)
            :_result( storage == 0 ? new VariableDataSource<bool>(false) : storage ), c(command) {}

        ~TryCommand() {
            delete c;
        }
        bool execute() {
            _result->set( c->execute() );
            return true;
        }
        void reset() {
            c->reset();
            _result->set(false);
        }

        VariableDataSource<bool>::shared_ptr result() {
            return _result;
        }

        CommandInterface* clone() const {
            return new TryCommand( c->clone(),
                                   _result );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommand( c->copy( alreadyCloned ),
                                   _result->copy(alreadyCloned) );
        }
    };

    /**
     * Returns the (accept/reject) status
     * of a \a TryCommand, where true means a reject !
     * @see TryCommand
     */
    class TryCommandResult :
        public ConditionInterface
    {
        DataSource<bool>::shared_ptr c;
    public:
        TryCommandResult( DataSource<bool>::shared_ptr ec )
            :c(ec) {}

        ~TryCommandResult() {
            // do not delete !
        }

        bool evaluate() {
            // true means reject
            return !c->get();
        }

        ConditionInterface* clone() const {
            return new TryCommandResult( c ); // do not clone c !
        }

        ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommandResult( c->copy(alreadyCloned) );
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
