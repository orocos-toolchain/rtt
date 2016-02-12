/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  FunctionFactory.cxx

                        FunctionFactory.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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


#include "FunctionFactory.hpp"

#include "../ExecutionEngine.hpp"
#include "../internal/GlobalEngine.hpp"
#include "CommandComposite.hpp"
#include "CommandBinary.hpp"
#include "CallFunction.hpp"
#include "CmdFunction.hpp"
#include "ConditionComposite.hpp"
#include "TryCommand.hpp"
#include <sstream>
#include <map>
#include <string>
#include "../internal/mystd.hpp"
#include <PropertyBag.hpp>
#include <Property.hpp>
#include "../Attribute.hpp"
#include "parse_exception.hpp"
#include "../internal/DataSourceCommand.hpp"
#include "../FactoryExceptions.hpp"
#include "../../Handle.hpp"


namespace RTT {
    using namespace detail;

        class CmdFunctionWrapper
            : public DataSource<SendStatus>
        {
            DataSource<SendStatus>::shared_ptr alias;
        public:
            typedef boost::intrusive_ptr<CmdFunctionWrapper> shared_ptr;

            CmdFunctionWrapper(DataSource<SendStatus>* ds)
            : alias(ds)
              {}

            ~CmdFunctionWrapper() { }

            bool evaluate() const {
                return alias->evaluate();
            }

            DataSource<SendStatus>::result_t get() const
            {
                return alias->get();
            }

            DataSource<SendStatus>::result_t value() const
            {
                return alias->value();
            }

            DataSource<SendStatus>::const_reference_t rvalue() const
            {
                return alias->rvalue();
            }

            virtual void reset() { /* nop, don't reset ! */ }

            virtual CmdFunctionWrapper* clone() const {
                return new CmdFunctionWrapper(alias.get());
            }
            virtual CmdFunctionWrapper* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                return new CmdFunctionWrapper(alias->copy(alreadyCloned) );
            }
        };


        FunctionFactory::FunctionFactory(ProgramInterfacePtr pi, ExecutionEngine* procs)
            : func(pi), proc(procs) {}

        std::string FunctionFactory::resultType() const {
            return func->getResult() ? func->getResult()->getDataSource()->getTypeName() : "void";
        }

        std::string FunctionFactory::getName() const {
            return func->getName();
        }

        std::string FunctionFactory::description() const {
            return "Orocos Program Script Function.";
        }

        std::vector< ArgumentDescription > FunctionFactory::getArgumentList() const
        {
            std::vector<AttributeBase*> origlist = func->getArguments();
            std::vector< ArgumentDescription > mlist;
            for ( std::vector<AttributeBase*>::const_iterator it = origlist.begin();
                  it != origlist.end(); ++it)
                mlist.push_back( ArgumentDescription( "arg", "Function Argument", (*it)->getDataSource()->getType() ) );
            return mlist;
        }

        unsigned int FunctionFactory::arity( ) const
        {
            return func->getArguments().size();
        }

        unsigned int FunctionFactory::collectArity( ) const
        {
            return func->getResult() ? 1 : 0;
        }

        const types::TypeInfo* FunctionFactory::getArgumentType(unsigned int arg) const
        {
            if (arg == 0 ) {
                if ( func->getResult() )
                    return func->getResult()->getDataSource()->getTypeInfo();
                else
                    return DataSourceTypeInfo<void>::getTypeInfo();
            }
            std::vector<AttributeBase*> origlist = func->getArguments();
            if ( arg > origlist.size() )
                return 0;
            return origlist[arg - 1]->getDataSource()->getTypeInfo();
        }

        const types::TypeInfo* FunctionFactory::getCollectType(unsigned int arg) const {
            if (arg == 1 && collectArity() )
                return func->getResult()->getDataSource()->getTypeInfo();
            return 0;
        }

        DataSourceBase::shared_ptr FunctionFactory::produce(
                      const std::vector<DataSourceBase::shared_ptr>& args
                      , ExecutionEngine* caller
                      ) const {
            bool issend = false;
            return produceHelper(args, caller, issend);
        }

            DataSourceBase::shared_ptr FunctionFactory::produceHelper(
                          const std::vector<DataSourceBase::shared_ptr>& args
                          , ExecutionEngine* caller, bool issend
                          ) const {
            // check if correct number of args :
            boost::shared_ptr<ProgramInterface> orig = func;
            std::vector<AttributeBase*> origlist = orig->getArguments();
            if ( args.size() != origlist.size() )
                throw wrong_number_of_args_exception( origlist.size(), args.size() );

            // make a semi-deep copy of the function :
            // copy the local variables, but clone() the remote datasources.
            std::map<const DataSourceBase*, DataSourceBase*> replacementdss;
            assert( orig );
            boost::shared_ptr<ProgramInterface> fcopy( orig->copy( replacementdss ) );
            assert( fcopy );
            // create commands that init all the args :
            CommandComposite* icom=  new CommandComposite();

            // get the correct pointers.
            origlist = fcopy->getArguments();
            std::vector<DataSourceBase::shared_ptr>::const_iterator dit = args.begin();
            std::vector<AttributeBase*>::const_iterator tit =  origlist.begin();
#ifndef ORO_EMBEDDED
            try {
                for (; dit != args.end(); ++dit, ++tit)
                    icom->add( (*tit)->getDataSource()->updateAction( dit->get() ) );
            }
            catch( const bad_assignment& ) {
                delete icom;
                int parnb = (dit - args.begin()) + 1;
                throw wrong_types_of_args_exception(parnb, (*tit)->getDataSource()->getType() ,(*dit)->getType() );
            }
#else
            for (; dit != args.end(); ++dit, ++tit) {
                ActionInterface* ret = (*tit)->getDataSource()->updateAction( dit->get() );
                if (ret)
                    icom->add( ret );
                else {
                    delete icom;
                    return 0;
                }
            }
#endif

            // the args of the copy can now safely be removed (saves memory):
            //fcopy->clearArguments();

            if (!caller)
                caller = GlobalEngine::Instance();
            if (issend == false) {
                // the command gets ownership of the new function :
                // this command is a DataSourceBase...
                AttributeBase* ar= fcopy->getResult();
                if (ar)
                    return ar->getDataSource()->getTypeInfo()->buildActionAlias( new CallFunction( icom, fcopy, proc, caller ), ar->getDataSource()).get();
                else // void case, returns result of runFunction (backwards compatibility).
                    return new DataSourceCommand( new CallFunction( icom, fcopy, proc, caller ) );
            } else {
                return new CmdFunction( icom, fcopy, proc, caller );
            }
        }

        base::DataSourceBase::shared_ptr FunctionFactory::produceHandle() const {
            return new ValueDataSource<SendStatus>(SendNotReady);
        }
        base::DataSourceBase::shared_ptr FunctionFactory::produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller
                                   ) const {
            return produceHelper(args, caller, true);
        }
        base::DataSourceBase::shared_ptr FunctionFactory::produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, DataSource<bool>::shared_ptr blocking
                                   ) const {
            if (blocking->get() == true )
                throw no_asynchronous_operation_exception("Blocking call to collect() not yet implemented for scripting functions. Use collectIfDone().");

            if (args.size() >= 1) {
                if ( dynamic_cast<CmdFunction* > (args[0].get()) != 0 ) {
                    // The CmdFunction : wrap it and return it
                    // wrapping is necessary because we don't want to propagate reset()
                    return new CmdFunctionWrapper( dynamic_cast<CmdFunction*>(args[0].get()) );
                } else if ( dynamic_cast<CmdFunctionWrapper* > (args[0].get()) != 0 ) {
                    // Return argument.
                    return args[0];
                } else {
                    log(Error) <<"FunctionFactory: Please define your SendHandle with 'var SendHandle' for script functions." <<endlog();
                    return 0;
                }

            }
            log(Error) <<"FunctionFactory: Must provide an argument in produceCollect." <<endlog();
            return 0;
        }
#ifdef ORO_SIGNALLING_OPERATIONS
        Handle FunctionFactory::produceSignal(base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* subscriber
                                   ) const {
        	throw no_asynchronous_operation_exception("Signal not yet implemented for scripting functions.");
            return Handle();
        }
#endif
}


