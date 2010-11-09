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
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

namespace RTT
{ namespace scripting {
	using namespace detail;

    /**
     * An action which calls a FunctionFraph for execution
     * in a ExecutionEngine.
     * Script functions are always executed in the thread of the component.
     *
     * It is an DataSource such that it can be executed by program scripts.
     */
    class RTT_SCRIPTING_API CallFunction
        : public base::ActionInterface
    {
        base::ActionInterface* minit;
        ExecutionEngine* mrunner;
        ExecutionEngine* mcaller;
        /**
         * _v is only necessary for the copy/clone semantics.
         */
        internal::AssignableDataSource<ProgramInterface*>::shared_ptr _v;
        /**
         * _foo contains the exact same pointer as _v->get(), but also serves
         * as a shared_ptr handle for cleanup after clone().
         */
        boost::shared_ptr<ProgramInterface> _foo;
        bool isqueued;
        bool maccept;

        bool fooDone() {
            return _foo->inError() || _foo->isStopped();
        }
        public:
        /**
         * Create a Command to send a function to a ExecutionEngine.
         * @param init_com  The command to execute before sending the
         * function into the processor, in order to initialise it.
         * @param foo The function to run in the processor.
         * @param p The target processor which will run the function.
         * @param v Implementation specific parameter to support copy/clone semantics.
         */
        CallFunction( base::ActionInterface* init_com,
                      boost::shared_ptr<ProgramInterface> foo,
                      ExecutionEngine* p, ExecutionEngine* caller,
                      internal::AssignableDataSource<ProgramInterface*>* v = 0 ,
                      internal::AssignableDataSource<bool>* a = 0 )
        : minit(init_com),
        mrunner(p), mcaller(caller),
        _v( v==0 ? new internal::UnboundDataSource< internal::ValueDataSource<ProgramInterface*> >(foo.get()) : v ),
        _foo( foo ), isqueued(false), maccept(false)
        {
        }

        ~CallFunction() {
            this->reset();
        }

        virtual bool execute() {
            // this is asyn behaviour :
            if (isqueued == false ) {
                isqueued = true;
                maccept = minit->execute() && mrunner->runFunction( _foo.get() );
                // we ignore the ret value of start(). It could have been auto-started during loading() of the function.
                _foo->start();
                if ( maccept ) {
                    // block for the result: foo stopped or in error
                    //mcaller->waitForFunctions(boost::bind(&CallFunction::fooDone,this) );
                    mrunner->waitForFunctions(boost::bind(&CallFunction::fooDone,this) );
                    if ( _foo->inError() ) {
                        throw false;
                    }
                    return true;
                }
                return false;
            }
            return true;
        }

        virtual void reset() {
            mrunner->removeFunction( _foo.get() );
            maccept = false;
            isqueued = false;
        }

        virtual bool valid() {
            return maccept;
        }

        virtual void readArguments() {
            // is called before runFunction is executed.
            minit->readArguments();
        }

        base::ActionInterface* clone() const
        {
            // _v is shared_ptr, so don't clone.
            return new CallFunction( minit->clone(), _foo, mrunner, mcaller, _v.get() );
        }

        base::ActionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
        {
            // this may seem strange, but :
            // make a copy of foo (a function), make a copy of _v (a datasource), store pointer to new foo in _v !
            boost::shared_ptr<ProgramInterface> fcpy( _foo->copy(alreadyCloned) );
            internal::AssignableDataSource<ProgramInterface*>* vcpy = _v->copy(alreadyCloned);
            vcpy->set( fcpy.get() ); // since we own _foo, we may manipulate the copy of _v
            return new CallFunction( minit->copy(alreadyCloned), fcpy , mrunner, mcaller, vcpy );
        }

    };

}}

#endif
