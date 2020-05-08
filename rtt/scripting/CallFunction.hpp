/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  CallFunction.hpp

                        CallFunction.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_CALL_FUNCTION_HPP
#define ORO_CALL_FUNCTION_HPP

#include "../base/ActionInterface.hpp"
#include "../base/DisposableInterface.hpp"
#include "../internal/DataSources.hpp"
#include "ProgramInterface.hpp"
#include "../internal/DataSource.hpp"
#include "../ExecutionEngine.hpp"
#include "../Logger.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

namespace RTT
{ namespace scripting {
	using namespace detail;

    /**
     * An action which calls a FunctionFraph for execution
     * in a ExecutionEngine.
     * Script functions are always executed in the thread of the component.
     */
    class RTT_SCRIPTING_API CallFunction
        : public base::ActionInterface, public base::DisposableInterface
    {
        base::ActionInterface* minit;
        ExecutionEngine* mrunner;
        ExecutionEngine* mcaller;
        boost::shared_ptr<ProgramInterface> _foo;
        bool maccept;

        /**
         * Check if the function either finished successfully or encountered
         * an error or yielded. Use this as a predicate to wait for partial
         * completion.
         */
        bool checkIfDoneOrYielded() {
            return (!maccept || _foo->inError() || _foo->isStopped());
        }

        /**
         * Check if the function either finished successfully or encountered
         * an error. Use this as a predicate to wait for full completion.
         * @throws false if the function encountered an error
         */
        bool checkIfDone() {
            if ( _foo->inError() ) {
                throw false;
            } else if ( _foo->isStopped() ) {
                return true;
            } else {
                return false;
            }
        }

    public:
        /**
         * Create a Command to send a function to a ExecutionEngine.
         * @param init_com  The command to execute before sending the
         * function into the processor, in order to initialise it.
         * @param foo The function to run in the processor.
         * @param p The target processor which will run the function.
         */
        CallFunction( base::ActionInterface* init_com,
                      boost::shared_ptr<ProgramInterface> foo,
                      ExecutionEngine* p, ExecutionEngine* caller
                      )
        : minit(init_com),
        mrunner(p), mcaller(caller),
        _foo( foo ), maccept(false)
        {
        }

        ~CallFunction() {
            this->reset();
            delete minit;
        }

        /**
         * Enqueue the function in the target engine (mrunner) and wait until
         * it is finished or in error state.
         *
         * As a first attempt the function is enqueued in the process queue of
         * the engine (@see ExecutionEngine::process()), which calls into
         * executeAndDispose() for actual execution in its callback step. If
         * ProgramInterface::execute() would return true and the program did not
         * yet finish (e.g. as a result of a yield command), it will be enqueued
         * in the function queue for synchronous execution with the next update step.
         *
         * @implements RTT::base::ActionInterface::execute()
         *
         */
        virtual bool execute() {
            // prepare execution
            if (!minit->execute()) return false;
            _foo->loaded(mrunner);
            // we ignore the ret value of start(). It could have been auto-started during loading() of the function.
            if ( _foo->needsStart() )
                _foo->start();

            if ( mrunner->isSelf() ) { // The calling thread is the same as the executor (synchronous function call).

                // 1. Inline the function (execute it immediately). Other than the
                //    following step 2, this does not give other operations the
                //    chance to execute.
                //
                //    @sa analogous to OperationCallerInterface::isSend()
                //
                maccept = true;
                this->executeAndDispose();
                if ( checkIfDone() ) return true;
                log(Warning) << "Execution of function '" << _foo->getName() << " did not finish in one cycle "
                                "and paused in line " << _foo->getLineNumber() << ", e.g. because of a yield statement." << nlog()
                             << "As it was called from the executing thread, execution will continue immediately to avoid a dead-lock." << nlog()
                             << "This behavior might be unexpected and can cause busy-wait loops." << endlog();

                // 2. While not done, enqueue as a message callback (for the callback step)
                //    ==> mrunner will call executeAndDispose() (see below)
                //
                //    If the caller thread is the same as the executor and the
                //    funtion yields, loop and enqueue it again. So yielding in
                //    a function called from the same thread only gives other
                //    enqueued messages (operations) a chance to execute, but
                //    afterwards the function continues within the same update
                //    step and without a call to updateHook() in between.
                //
                do {
                    maccept = mrunner->process( this );
                    if ( !maccept ) return false;

                    // block for the result: foo stopped or in error or yielded
                    mrunner->waitForMessages(boost::bind(&CallFunction::checkIfDoneOrYielded, this) );
                } while( !checkIfDone() );

                return true;

            } else { // The caller thread is different from the executor (asynchronous function call).

                // 1. Enqueue as a message callback (for the callback step)
                //    ==> mrunner will call executeAndDispose() (see below)
                //
                maccept = mrunner->process( this );
                if ( !maccept ) return false;

                // block for the result: foo stopped or in error or yielded
                mrunner->waitForMessages(boost::bind(&CallFunction::checkIfDoneOrYielded, this) );
                if ( checkIfDone() ) return true;

                // 2. If not yet done, enqueue as a function and wait for the next update step
                //    ==> mrunner will run _foo until it finishs
                //
                // Note: There is no guarantee that the function will not already execute again in the same cycle,
                //       once as a message and once as function. Not really a problem...
                //
                maccept = mrunner->runFunction( _foo.get() );
                if ( !maccept ) return false;

                // block for the result: foo stopped or in error
                mrunner->waitForMessages(boost::bind(&CallFunction::checkIfDone, this) );
                if ( checkIfDone() ) return true;

                return false;
            }
        }

        /**
         * Message callback that executes the function until it finishes or
         * yields.
         *
         * Will be processed in the mrunner engine. If necessary, enqueue the
         * function in the function queue for later synchronous execution if it
         * did not finish after a single call (@see execute()).
         *
         * @implements RTT::base::DisposableInterface::executeAndDispose()
         */
        virtual void executeAndDispose() {
            if ( _foo->execute() == false ) {
                _foo->unloaded();
            } else {
                // need to execute again
                maccept = false;
            }
            // ExecutionEngine will eventually wake up the caller waiting in mrunner->waitForMessages().
        }

        // we do not want to be disposed
        virtual void dispose() {}

        virtual void reset() {
            maccept = false;
        }

        virtual bool valid() const {
            return maccept;
        }

        virtual void readArguments() {
            // is called before runFunction is executed.
            minit->readArguments();
        }

        base::ActionInterface* clone() const
        {
            return new CallFunction( minit->clone(), _foo, mrunner, mcaller);
        }

        base::ActionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
        {
            boost::shared_ptr<ProgramInterface> fcpy( _foo->copy(alreadyCloned) );
            return new CallFunction( minit->copy(alreadyCloned), fcpy , mrunner, mcaller );
        }

    };

}}

#endif
