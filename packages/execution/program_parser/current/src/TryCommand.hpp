#ifndef ORO_TRY_COMMAND_HPP
#define ORO_TRY_COMMAND_HPP

#include <corelib/CommandInterface.hpp>
#include <corelib/ConditionInterface.hpp>

namespace ORO_Execution 
{
    using namespace ORO_CoreLib;

    class TryCommand :
        public CommandInterface
    {
        bool result;
        CommandInterface* c;
    public:
        TryCommand( CommandInterface* command)
            :result(false), c(command) {}

        ~TryCommand() {
            delete c;
        }
        bool execute() {
            result = c->execute();
            return true;
        }
        void reset() {
            c->reset();
            result = false;
        }

        bool getResult() { 
            return result;
        }

        CommandInterface* clone() const {
            return new TryCommand( c->clone() );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommand( c->copy( alreadyCloned ) );
        }
    };

    class TryCommandResult :
        public ConditionInterface
    {
        TryCommand* c;
    public:
        TryCommandResult( TryCommand* ec)
            :c(ec) {}

        ~TryCommandResult() {
            // do not delete !
        }

        bool evaluate() {
            return c->getResult();
        }

        CommandInterface* clone() const {
            return new TryCommandResult( c ); // do not clone c !
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommandResult( c->copy( alreadyCloned ) );
        }
    };

    class EvalCommand :
        public CommandInterface
    {
        bool result;
        DataSource<bool>* c;
    public:
        EvalCommand( DataSource<bool>* ds)
            :c(ds) {}

        ~EvalCommand() {
            delete c;
        }

        bool execute() {
            result = c->get();
            return true;
        }

        CommandInterface* clone() const {
            return new EvalCommand( c->clone() );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new EvalCommand( c->copy( alreadyCloned ) );
        }
    };

    class EvalCommandResult :
        public ConditionInterface
    {
        EvalCommand* c;
    public:
        EvalCommandResult( EvalCommand* ec)
            :c(ec) {}

        ~EvalCommandResult() {
            // do not delete !
        }

        bool evaluate() {
            return c->getResult();
        }

        CommandInterface* clone() const {
            return new EvalCommandResult( c ); // do not clone c !
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new EvalCommandResult( c->copy( alreadyCloned ) );
        }
    };
}
    
#endif
