/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  CommandExecFunction.hpp 

                        CommandExecFunction.hpp -  description
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
 
 
#ifndef COMMAND_EXEC_FUNCTION_HPP
#define COMMAND_EXEC_FUNCTION_HPP

#include <corelib/ConditionInterface.hpp>
#include <corelib/CommandInterface.hpp>
#include <corelib/DataSources.hpp>
#include "ProgramInterface.hpp"
#include "ProgramProcessor.hpp"
#include "DispatchInterface.hpp"
#include "DataSource.hpp"
#include <boost/shared_ptr.hpp>

namespace ORO_Execution
{
    /**
     * A condition which checks if a CommandExecFunction is done or not.
     */
    class ConditionExecFunction
        : public ORO_CoreLib::ConditionInterface
    {
        DataSource<ProgramInterface*>::shared_ptr _v;
    public:
        ConditionExecFunction( DataSource<ProgramInterface*>* v)
            : _v( v )
        {}

        bool evaluate()
        {
            return _v->get()->isStopped();
        }

        ORO_CoreLib::ConditionInterface* clone() const
        {
            return new ConditionExecFunction( _v.get() );
        }

        ORO_CoreLib::ConditionInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
        {
            // after *all* the copying is done, _v will be set to the correct function
            // by the Command's copy.
            return new ConditionExecFunction( _v->copy( alreadyCloned ) );
        }

    };

    /**
     * A command which queues (dispatches) a FunctionFraph for execution
     * in a ProgramProcessor. See ConditionExecFunction to check if
     * it is done or not. It does not need the CommandDispatch
     * to be thread-safe.
     */
    class CommandExecFunction
        : public DispatchInterface
    {
        CommandInterface* minit;
        ProgramProcessor* _proc;
        AssignableDataSource<ProgramInterface*>::shared_ptr _v;
        boost::shared_ptr<ProgramInterface> _foo;
        bool isqueued;
        AssignableDataSource<bool>::shared_ptr maccept;
    public:
        /**
         * Create a Command to send a function to a ProgramProcessor.
         * @param init_com  The command to execute before sending the
         * function into the processor, in order to initialise it.
         * @param foo The function to run in the processor.
         * @param p The target processor which will run the function.
         * @param v Implementation specific parameter to support copy/clone semantics.
         */
        CommandExecFunction( CommandInterface* init_com, boost::shared_ptr<ProgramInterface> foo, ProgramProcessor* p, AssignableDataSource<ProgramInterface*>* v = 0 , AssignableDataSource<bool>* a = 0 )
            : minit(init_com),
              _proc(p),
              _v( v==0 ? new ORO_CoreLib::detail::UnboundDataSource< ORO_CoreLib::ValueDataSource<ProgramInterface*> >(foo.get()) : v ),
              _foo( foo ), isqueued(false), maccept( a ? a : new ORO_CoreLib::detail::UnboundDataSource<ORO_CoreLib::ValueDataSource<bool> >(false) )
        {
        }

        ~CommandExecFunction() {
            //remove it before destruction.
            _proc->removeFunction( _foo.get() );
        }

        void readArguments()
        {
            minit->readArguments();
        }

        bool dispatch() 
        {
            return execute();
        }

        bool execute()
        {
            // this is asyn behaviour :
            if (isqueued == false ) {
                isqueued = true;
                maccept->set( minit->execute() && _proc->runFunction( _foo.get() ) );
                return maccept->get();
            }
            // if it was queued already return if it is
            // in error or not.
            return maccept->get() && ! _foo->inError();
        }

        void reset()
        {
            // reset the program, so that it is valid to be re-queued again
            _foo->reset();
            minit->reset();
            isqueued = false;
        }

        virtual bool sent() const {
            return isqueued;
        }

        virtual bool accepted() const {
            return maccept->get();
        }

        virtual bool executed() const {
            return isqueued;
        }

        virtual bool valid() const {
            return maccept->get();
        }

        virtual bool evaluate() const {
            return maccept->get() && _v->get()->isStopped();
        }

        /**
         * Create a condition which checks if this command is finished or not.
         */
        ORO_CoreLib::ConditionInterface* createCondition() const
        {
            return new ConditionExecFunction( _v.get() );
        }
        
        DispatchInterface* clone() const
        {
            // _v is shared_ptr, so don't clone.
            return new CommandExecFunction( minit->clone(), _foo, _proc, _v.get(), maccept.get() );
        }
        
        ORO_CoreLib::CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
        {
            // this may seem strange, but :
            // make a copy of foo (a function), make a copy of _v (a datasource), store pointer to new foo in _v !
            boost::shared_ptr<ProgramInterface> fcpy( _foo->copy(alreadyCloned) );
            AssignableDataSource<ProgramInterface*>* vcpy = _v->copy(alreadyCloned);
            vcpy->set( fcpy.get() ); // since we own _foo, we may manipulate the copy of _v
            AssignableDataSource<bool>* acpy = maccept->copy(alreadyCloned);
            return new CommandExecFunction( minit->copy(alreadyCloned), fcpy , _proc, vcpy, acpy );
        }
        
    };

}

#endif
