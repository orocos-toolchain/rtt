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

namespace ORO_Execution
{
        FunctionFactory::FunctionFactory(Processor* procs) : proc(procs) {}
        FunctionFactory::~FunctionFactory() {}

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
                            mystd::select1st<map_t::value_type>() );
            return ret;
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
                mlist.push_back( ArgumentDescription( "arg", "argdesc" ) );
            return mlist;
        }

        ComCon FunctionFactory::create( const std::string& command,
                       const ORO_CoreLib::PropertyBag& args,
                       bool asyn  ) const {
            // this method will become deprecated.
            return ComCon(0,0);
        }

        ComCon FunctionFactory::create(
                      const std::string& command,
                      const std::vector<DataSourceBase*>& args,
                      bool asyn  ) const {
            // We ignore asyn, since we CommandExecFunction is always asyn itself.
            if ( !hasCommand(command) )
                throw name_not_found_exception();

            // check if correct number of args :
            FunctionGraph* orig = funcmap.find(command)->second;
            std::vector<TaskAttributeBase*> origlist = orig->getArguments();
            if ( args.size() != origlist.size() )
                throw wrong_number_of_args_exception( origlist.size(), args.size() );
            
            // make a deep copy of the function :
            std::map<const DataSourceBase*, DataSourceBase*> replacementdss;
            boost::shared_ptr<FunctionGraph> copy( orig->copy( replacementdss ) );
            // create commands that init all the args :
            CommandComposite* icom=  new CommandComposite();
            origlist = copy->getArguments(); // get the correct pointers.
            std::vector<DataSourceBase*>::const_iterator dit = args.begin();
            std::vector<TaskAttributeBase*>::const_iterator tit =  origlist.begin();
            try {
                for (; dit != args.end(); ++dit, ++tit)
                    icom->add( (*tit)->assignCommand( *dit, true ) );
            }
            catch( const bad_assignment& e) {
                delete copy;
                delete icom;
                int parnb = (dit - args.begin());
                throw wrong_types_of_args_exception(parnb);
            }
            // the command gets ownership of the new function :
            CommandExecFunction* ecom = new CommandExecFunction( copy, proc );
            ConditionInterface*  con = ecom->createCondition();
            // first init, then dispatch the function.
            // init->execute() : once(asyn), ecom->execute() : until done (syn)
            CommandInterface*    com =  new CommandBinary(icom, ecom);
            return ComCon(com, con);
        }
}


