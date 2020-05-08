/***************************************************************************
  tag: Peter Soetens Sat Oct 25 00:52:59 2014 +0200 CmdFunction.hpp

                        CmdFunction.hpp -  description
                           -------------------
    begin                : Sat Oct 25 2014
    copyright            : (C) 2014 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_CMD_FUNCTION_HPP
#define ORO_CMD_FUNCTION_HPP

#include "../SendStatus.hpp"
#include "../base/ActionInterface.hpp"
#include "../base/DisposableInterface.hpp"
#include "ConditionInterface.hpp"
#include "../internal/DataSources.hpp"
#include "ProgramInterface.hpp"
#include "../internal/DataSource.hpp"
#include "../ExecutionEngine.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <iostream>

namespace RTT
{ namespace scripting {
	using namespace detail;
    using namespace std;

    /**
     * A DataSource which sends a FunctionFraph for execution
     * in a ExecutionEngine.
     */
    class RTT_SCRIPTING_API CmdFunction
        : public internal::DataSource<SendStatus>
    {
        base::ActionInterface* minit;
        ExecutionEngine* mrunner;
        ExecutionEngine* mcaller;
        boost::shared_ptr<ProgramInterface> _foo;
        mutable SendStatus ss;
        mutable bool isqueued;
        mutable bool maccept;

    public:
        /**
         * Create a Command to send a function to a ExecutionEngine.
         * @param init_com  The command to execute before sending the
         * function into the processor, in order to initialise it.
         * @param foo The function to run in the processor.
         * @param p The target processor which will run the function.
         */
        CmdFunction( base::ActionInterface* init_com,
                      boost::shared_ptr<ProgramInterface> foo,
                      ExecutionEngine* p, ExecutionEngine* caller
                      )
        : minit(init_com),
        mrunner(p), mcaller(caller),
        _foo( foo ), ss(SendFailure), isqueued(false), maccept(false)
        {
        }

        ~CmdFunction() {
            this->reset();
            delete minit;
        }

        virtual SendStatus get() const {
            try {
                // this is asyn behaviour :
                if (isqueued == false ) {
                    isqueued = true;
                    ss = SendNotReady;
                    // is called before runFunction is executed.
                    minit->readArguments();
                    maccept = minit->execute() && mrunner->runFunction( _foo.get() );
                    // we ignore the ret value of start(). It could have been auto-started during loading() of the function.
                    if ( _foo->needsStart() ) { // _foo might be auto-started in runFunction()
                        _foo->start();
                    }
                    if ( ! maccept ) {
                        return ss = SendFailure;
                    }
                }
                if ( _foo->inError() ) {
                    return ss = CollectFailure;
                }
                if ( _foo->isStopped() ) {
                    return ss = SendSuccess;
                }
            } catch (...) {
                cout << "CmdFunction threw an exception" <<endl;
            }
            return ss;
        }

        virtual SendStatus value() const {
            return ss;
        }

        virtual SendStatus const& rvalue() const {
            return ss;
        }

        virtual bool evaluate() const {
            // return true if ready to be read
            return get() != SendNotReady;
        }

        virtual void reset() {
            if (_foo->isLoaded()) mrunner->removeFunction( _foo.get() );
            maccept = false;
            isqueued = false;
            ss = SendFailure;
        }

        CmdFunction* clone() const
        {
            return new CmdFunction( minit->clone(), _foo, mrunner, mcaller );
        }

        CmdFunction* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
        {
            // if somehow a copy exists, return the copy, otherwise return this (see Attribute copy)
            if ( alreadyCloned[this] == 0 ) {
                boost::shared_ptr<ProgramInterface> fcpy( _foo->copy(alreadyCloned) );
                alreadyCloned[this] = new CmdFunction( minit->copy(alreadyCloned), fcpy , mrunner, mcaller);
            }

            assert ( dynamic_cast<CmdFunction*>( alreadyCloned[this] ) == static_cast<CmdFunction*>( alreadyCloned[this] ) );
            return static_cast<CmdFunction*>( alreadyCloned[this] );
        }

    };

        /**
         * A DataSource that collects the result of a CmdFunction
         */
        struct RTT_SCRIPTING_API CmdCollectCondition
        : public ConditionInterface
        {
            DataSource<SendStatus>::shared_ptr collectds;
        public:

              CmdCollectCondition(DataSource<SendStatus>::shared_ptr ds ) :
                  collectds(ds)
              {
              }

              void reset() {}

              bool evaluate()
              {
                  // We may not call collectds->evaluate() inhere, because this actively tries a collect,
                  // and if it would succeed, we would proceed immediately, without giving the vertex node
                  // a chance to do something with the return value of the cmd.
                  return collectds->rvalue() != SendNotReady;
              }

              virtual CmdCollectCondition* clone() const
              {
                  return new CmdCollectCondition( collectds );
              }
              virtual CmdCollectCondition* copy(
                                                          std::map<
                                                                  const base::DataSourceBase*,
                                                                  base::DataSourceBase*>& alreadyCloned) const
              {
                  return new CmdCollectCondition ( collectds->copy(alreadyCloned) );
              }
          };
}}

#endif
