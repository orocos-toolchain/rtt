#ifndef COMMAND_EXEC_FUNCTION_HPP
#define COMMAND_EXEC_FUNCTION_HPP

#include <corelib/ConditionInterface.hpp>
#include <corelib/CommandInterface.hpp>
#include "FunctionGraph.hpp"
#include "Processor.hpp"
#include "DataSource.hpp"
#include <boost/shared_ptr.hpp>

namespace ORO_Execution
{
    /**
     * A condition which checks if a CommandExecFunction is done or not.
     */
    class ConditionExecFunction
        : public ConditionInterface
    {
        VariableDataSource<FunctionGraph*>::shared_ptr _v;
        Processor* _proc;
    public:
        ConditionExecFunction( VariableDataSource<FunctionGraph*>* v, Processor* p )
            : _v( v ), _proc(p)
        {}

        bool evaluate()
        {
            return _v->get()->isFinished();
        }

        ConditionInterface* clone() const
        {
            return new ConditionExecFunction( _v.get(), _proc );
        }

        ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        {
            // after *all* the copying is done, _v will be set to the correct function
            // by the Command's copy.
            return new ConditionExecFunction( _v->copy( alreadyCloned ), _proc );
        }

    };

    /**
     * A command which queues (dispatches) a FunctionFraph for execution
     * in a Processor. See ConditionExecFunction to check if
     * it is done or not. It does not need the CommandDispatch
     * to be thread-safe.
     */
    class CommandExecFunction
        : public CommandInterface
    {
        Processor* _proc;
        VariableDataSource<FunctionGraph*>::shared_ptr _v;
        boost::shared_ptr<FunctionGraph> _foo;
        bool isqueued;
    public:
        CommandExecFunction( boost::shared_ptr<FunctionGraph> foo, Processor* p, VariableDataSource<FunctionGraph*>* v = 0 )
            : _proc(p), _v( v==0 ? new VariableDataSource<FunctionGraph*>(foo.get()) : v ),  _foo( foo ), isqueued(false)
        {
        }

        ~CommandExecFunction() {
            delete _foo;
        }

        bool execute()
        {
            // this is asyn behaviour :
            if (isqueued == false ) {
                isqueued = true;
                return _proc->runFunction( _foo.get() );
            }
            // if it was queued already return if it is
            // in error or not.
            return ! _foo->inError();
        }
        void reset()
        {
            // reset the program, so that it is valid to be re-queued again
            _foo->reset();
            isqueued = false;
        }

        /**
         * Create a condition which checks if this command is finished or not.
         */
        ConditionInterface* createCondition()
        {
            return new ConditionExecFunction( _v.get(),  _proc );
        }
        
        CommandInterface* clone() const
        {
            // _v is shared_ptr, so don't clone.
            return new CommandExecFunction( _foo, _proc, _v.get() );
        }
        
        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        {
            // this may seem strange, but :
            // make a copy of foo, make a copy of _v, store pointer to new foo in _v !
            boost::shared_ptr<FunctionGraph> fcpy( _foo->copy(alreadyCloned) );
            VariableDataSource<FunctionGraph*>* vcpy = _v->copy(alreadyCloned);
            vcpy->set( fcpy.get() ); // since we own _foo, we may manipulate the copy of _v
            return new CommandExecFunction( fcpy , _proc, vcpy );
        }
        
    };

}

#endif
