/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  FunctionFactory.cxx 

                        FunctionFactory.cxx -  description
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
 
 
#include <rtt/FunctionFactory.hpp>

#include <rtt/ExecutionEngine.hpp>
#include "rtt/CommandComposite.hpp"
#include "rtt/CommandBinary.hpp"
#include "rtt/CommandExecFunction.hpp"
#include "rtt/CommandDispatch.hpp"
#include "rtt/ConditionComposite.hpp"
#include "rtt/TryCommand.hpp"
#include <sstream>
#include <map>
#include <string>
#include "rtt/mystd.hpp"
#include <rtt/PropertyBag.hpp>
#include <rtt/Property.hpp>
#include "rtt/Attribute.hpp"
#include "rtt/parse_exception.hpp"
#include <boost/bind.hpp>

namespace RTT
{
    using namespace boost;
    

        FunctionFactory::FunctionFactory(ProgramInterfacePtr pi, ExecutionEngine* procs)
            : detail::OperationFactoryPart<DispatchInterface*>("A function."), func(pi), proc(procs) {}

        std::string FunctionFactory::resultType() const {
            return std::string("bool");
        }

        PropertyBag
        FunctionFactory::getArgumentSpec() const
        {
            std::vector<AttributeBase*> origlist = func->getArguments();

            PropertyBag ret;
            for ( std::vector<AttributeBase*>::const_iterator it = origlist.begin();
                  it != origlist.end(); ++it)
                ret.add( (*it)->getDataSource()->getTypeInfo()->buildProperty("arg","Function Argument."));
            return ret;
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

        int FunctionFactory::arity( ) const
        {
            return func->getArguments().size();
        }

        DispatchInterface* FunctionFactory::produce(
                      const std::vector<DataSourceBase::shared_ptr>& args
                      ) const {

            // check if correct number of args :
            shared_ptr<ProgramInterface> orig = func;
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
                    icom->add( (*tit)->getDataSource()->updateCommand( dit->get() ) );
            }
            catch( const bad_assignment& e) {
                delete icom;
                int parnb = (dit - args.begin()) + 1;
                throw wrong_types_of_args_exception(parnb, (*tit)->getDataSource()->getType() ,(*dit)->getType() );
            }
#else
            for (; dit != args.end(); ++dit, ++tit) {
                CommandInterface* ret = (*tit)->getDataSource()->updateCommand( dit->get() );
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
                
            // the command gets ownership of the new function :
            // this command is a DispatchInterface...
            return new CommandExecFunction( icom, fcopy, proc->programs() );
        }
}


