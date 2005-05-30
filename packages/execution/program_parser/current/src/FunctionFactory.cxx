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
 
 
#include <execution/FunctionFactory.hpp>

#include "execution/CommandComposite.hpp"
#include "execution/CommandBinary.hpp"
#include "execution/CommandExecFunction.hpp"
#include "execution/CommandDispatch.hpp"
#include "execution/ConditionComposite.hpp"
#include "execution/TryCommand.hpp"
#include <sstream>
#include <map>
#include <string>
#include "execution/mystd.hpp"
#include <corelib/PropertyBag.hpp>
#include <corelib/Property.hpp>
#include "execution/TaskAttribute.hpp"
#include "execution/parse_exception.hpp"
#include <boost/bind.hpp>

namespace ORO_Execution
{
    using namespace boost;

        FunctionFactory::FunctionFactory(Processor* procs) : proc(procs) {}
        FunctionFactory::~FunctionFactory() {
            while( !funcmap.empty() ) {
                delete funcmap.begin()->second;
                funcmap.erase(funcmap.begin());
            }
        }

        void FunctionFactory::addFunction(const std::string& name, FunctionGraph* f ) 
        {
            funcmap[name] = f;
        }

        bool FunctionFactory::hasCommand(const std::string& com) const {
            return funcmap.count(com);
        }

        std::vector<std::string> FunctionFactory::getCommandList() const {
            std::vector<std::string> ret;
            std::transform( funcmap.begin(), funcmap.end(),
                            std::back_inserter( ret ),
                            ORO_std::select1st<map_t::value_type>() );
            return ret;
        }

        std::string FunctionFactory::getResultType( const std::string& com ) const {
            if ( hasCommand(com) )
                return std::string("bool");
            throw name_not_found_exception();
        }

        std::string FunctionFactory::getDescription( const std::string& com ) const {
            if ( hasCommand(com) )
                return std::string("A function with name ") + com;
            throw name_not_found_exception();
        }

        ORO_CoreLib::PropertyBag
        FunctionFactory::getArgumentSpec( const std::string& command ) const
        {
            if ( !hasCommand(command) )
                throw name_not_found_exception();
            FunctionGraph* orig = funcmap.find(command)->second;
            std::vector<TaskAttributeBase*> origlist = orig->getArguments();

            PropertyBag ret;
            for ( std::vector<TaskAttributeBase*>::const_iterator it = origlist.begin();
                  it != origlist.end(); ++it)
                ret.add( new Property<std::string>("na", "Type Not available") );
            return ret;
        }

        std::vector< ArgumentDescription > FunctionFactory::getArgumentList( const std::string& command ) const
        {
            if ( !hasCommand(command) )
                throw name_not_found_exception();

            FunctionGraph* orig = funcmap.find(command)->second;
            std::vector<TaskAttributeBase*> origlist = orig->getArguments();
            std::vector< ArgumentDescription > mlist;
            for ( std::vector<TaskAttributeBase*>::const_iterator it = origlist.begin();
                  it != origlist.end(); ++it)
                mlist.push_back( ArgumentDescription( "arg", "Function Argument", (*it)->toDataSource()->getType() ) );
            return mlist;
        }

        ComCon FunctionFactory::create( const std::string& command,
                       const ORO_CoreLib::PropertyBag& args,
                       bool ) const {
            std::vector<DataSourceBase::shared_ptr> dsVect;
            std::transform( args.begin(), args.end(),
                            std::back_inserter( dsVect ),
                            bind( &ORO_CoreLib::PropertyBase::createDataSource, _1));
            return this->create( command, dsVect );
        }

        ComCon FunctionFactory::create(
                      const std::string& command,
                      const std::vector<DataSourceBase*>& args,
                      bool ) const {
            std::vector<DataSourceBase::shared_ptr> dsVect;
            for( std::vector<DataSourceBase*>::const_iterator i = args.begin(); i != args.end(); ++i )
                dsVect.push_back( DataSourceBase::shared_ptr( *i ));
            return this->create( command, dsVect );
        }

        ComCon FunctionFactory::create(
                      const std::string& command,
                      const std::vector<DataSourceBase::shared_ptr>& args,
                      bool ) const {
            // We ignore asyn, since we CommandExecFunction is always asyn itself.
            if ( !hasCommand(command) )
                throw name_not_found_exception();

            // check if correct number of args :
            FunctionGraph* orig = funcmap.find(command)->second;
            std::vector<TaskAttributeBase*> origlist = orig->getArguments();
            if ( args.size() != origlist.size() )
                throw wrong_number_of_args_exception( origlist.size(), args.size() );

            // make a semi-deep copy of the function :
            // copy the local variables, but clone() the remote datasources.
            std::map<const DataSourceBase*, DataSourceBase*> replacementdss;
            assert( orig );
            boost::shared_ptr<FunctionGraph> fcopy( orig->copy( replacementdss ) );
            assert( fcopy );
            // create commands that init all the args :
            CommandComposite* icom=  new CommandComposite();

            // get the correct pointers.
            origlist = fcopy->getArguments(); 
            std::vector<DataSourceBase::shared_ptr>::const_iterator dit = args.begin();
            std::vector<TaskAttributeBase*>::const_iterator tit =  origlist.begin();
            try {
                for (; dit != args.end(); ++dit, ++tit)
                    icom->add( (*tit)->assignCommand( *dit, true ) );
            }
            catch( const bad_assignment& e) {
                delete icom;
                int parnb = (dit - args.begin());
                throw wrong_types_of_args_exception(parnb, (*tit)->toDataSource()->getType() ,(*dit)->getType() );
            }

            // the args of the copy can now safely be removed (saves memory):
            fcopy->clearArguments();
                
            // the command gets ownership of the new function :
            CommandExecFunction* ecom = new CommandExecFunction( fcopy, proc );
            ConditionInterface*  con = ecom->createCondition();
            // first init, then dispatch the function.
            // init->execute() : once(asyn), ecom->execute() : until done (syn)
            CommandInterface*    com =  new CommandBinary(icom, ecom);
            return ComCon(com, con);
        }
}


