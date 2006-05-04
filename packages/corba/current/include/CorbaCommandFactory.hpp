#ifndef ORO_CORBACOMMANDFACTORY_HPP
#define ORO_CORBACOMMANDFACTORY_HPP

#include <execution/CommandFactoryInterface.hpp>
#include <execution/ConditionBoolDataSource.hpp>
#include "FactoriesC.h"
#include "CommandProxy.hpp"

namespace ORO_Corba
{

    /**
     * This class mirrors a Corba Command Factory as a plain C++
     * factory.
     */
    class CorbaCommandFactory
        : public ORO_Execution::CommandFactoryInterface
    {
        std::string mobjname;
        Orocos::CommandInterface_var mfact;
    public:
        typedef std::vector< ORO_Execution::ArgumentDescription > Descriptions;
        typedef std::vector<std::string> Commands;
        typedef std::vector<ORO_CoreLib::DataSourceBase::shared_ptr> Arguments;

        CorbaCommandFactory(const std::string& obj, Orocos::CommandInterface_ptr fact) 
            : mobjname(obj), mfact( Orocos::CommandInterface::_duplicate(fact) )
        {}

        virtual ~CorbaCommandFactory() {}

        virtual bool hasCommand(const std::string& com) const 
        {
            try {
                Orocos::CommandList_var result = mfact->getCommands( mobjname.c_str() );
                for (size_t i=0; i!= result->length(); ++i)
                    if ( com == std::string(result[i]) )
                        return true;
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw ORO_Execution::name_not_found_exception( nsn.name.in() );
            }
            return false;
        }

        virtual Commands getCommandList() const 
        {
            Commands ret;
            Orocos::CommandList_var result = mfact->getCommands( mobjname.c_str() );
            ret.reserve( result->length() );
            for (size_t i=0; i!= result->length(); ++i)
                ret.push_back( std::string( result[i] ) );
            return ret;
        }

        virtual std::string getResultType( const std::string& com ) const 
        {
            return "bool";
        }

        virtual std::string getDescription( const std::string& com ) const 
        {
            try {
                CORBA::String_var result = mfact->getDescription( mobjname.c_str(), com.c_str() );
                return std::string( result.in() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw ORO_Execution::name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual ORO_CoreLib::PropertyBag
        getArgumentSpec( const std::string& command ) const {
            return ORO_CoreLib::PropertyBag();
        }

        /**
         * @brief Return the list of arguments of a certain command.
         */
        virtual Descriptions getArgumentList( const std::string& command ) const
        {
            Descriptions ret;
            try {
                Orocos::Descriptions_var result = mfact->getArguments( mobjname.c_str(), command.c_str() );
                ret.reserve( result->length() );
                for (size_t i=0; i!= result->length(); ++i)
                    ret.push_back( ORO_Execution::ArgumentDescription(std::string( result[i].name.in() ),
                                                       std::string( result[i].description.in() ),
                                                       std::string( result[i].type.in() ) ));
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw ORO_Execution::name_not_found_exception( nsn.name.in() );
            }
            return ret;
        }

        virtual ORO_Execution::ComCon create( const std::string& command,
                               const ORO_CoreLib::PropertyBag& args,
                               bool nodispatch) const
        {
            ORO_Execution::ComCon cc;
            cc.first = 0;
            cc.second = 0;
            return cc;
        }

        virtual ORO_Execution::ComCon create(
                              const std::string& command,
                              const Arguments& args,
                              bool nodispatch ) const 
        {
            Orocos::Arguments_var nargs = new Orocos::Arguments();
            nargs->length( args.size() );
            for (size_t i=0; i < args.size(); ++i )
                nargs[i] = args[i]->server();
            try {
                Orocos::Command_var result = mfact->createCommand( mobjname.c_str(), command.c_str(), nargs.in() );
                ORO_Execution::ComCon cc;
                // return a DispatchInterface object:
                cc.first = CommandProxy::Create( result.in() );
                cc.second = new ORO_Execution::ConditionBoolDataSource( ORO_Execution::DataSource<bool>::narrow( ExpressionProxy::Create( result->createCondition() ) ) );
                return cc;
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw ORO_Execution::name_not_found_exception( nsn.name.in() );
            } catch ( Orocos::WrongNumbArgException& wa ) {
                throw ORO_Execution::wrong_number_of_args_exception( wa.wanted, wa.received );
            } catch ( Orocos::WrongTypeArgException& wta ) {
                throw ORO_Execution::wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
            }
            return ORO_Execution::ComCon(); // not reached.
        }

        virtual ORO_Execution::ComCon create(
                              const std::string& command,
                              const std::vector<ORO_CoreLib::DataSourceBase*>& args,
                              bool nodispatch ) const
        {
            ORO_Execution::ComCon cc;
            cc.first = 0;
            cc.second = 0;
            return cc;
        }

    };
}

#endif
