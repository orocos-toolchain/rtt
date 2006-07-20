/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  CorbaCommandFactory.hpp 

                        CorbaCommandFactory.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_CORBACOMMANDFACTORY_HPP
#define ORO_CORBACOMMANDFACTORY_HPP

#include "CommandFactoryInterface.hpp"
#include "ConditionBoolDataSource.hpp"
#include "FactoriesC.h"
#include "CommandProxy.hpp"

namespace Corba
{

    /**
     * This class mirrors a Corba Command Factory as a plain C++
     * factory.
     */
    class CorbaCommandFactory
        : public CommandFactoryInterface
    {
        std::string mobjname;
        Orocos::CommandInterface_var mfact;
    public:
        typedef std::vector< ArgumentDescription > Descriptions;
        typedef std::vector<std::string> Commands;
        typedef std::vector<DataSourceBase::shared_ptr> Arguments;

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
                throw name_not_found_exception( nsn.name.in() );
            }
            return false;
        }

        virtual int getArity( const std::string& com )  const {
            if (this->hasCommand(com) == false)
                return -1;
            return this->getArgumentList(com).size();
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
                throw name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual PropertyBag
        getArgumentSpec( const std::string& command ) const {
            return PropertyBag();
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
                    ret.push_back( ArgumentDescription(std::string( result[i].name.in() ),
                                                       std::string( result[i].description.in() ),
                                                       std::string( result[i].type.in() ) ));
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            }
            return ret;
        }

        virtual ComCon create( const std::string& command,
                               const PropertyBag& args,
                               bool nodispatch) const
        {
            ComCon cc;
            cc.first = 0;
            cc.second = 0;
            return cc;
        }

        virtual ComCon create(
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
                ComCon cc;
                // return a DispatchInterface object:
                cc.first = CommandProxy::Create( result.in() );
                cc.second = new ConditionBoolDataSource( DataSource<bool>::narrow( ExpressionProxy::Create( result->createCondition() ) ) );
                return cc;
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            } catch ( Orocos::WrongNumbArgException& wa ) {
                throw wrong_number_of_args_exception( wa.wanted, wa.received );
            } catch ( Orocos::WrongTypeArgException& wta ) {
                throw wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
            }
            return ComCon(); // not reached.
        }

        virtual ComCon create(
                              const std::string& command,
                              const std::vector<DataSourceBase*>& args,
                              bool nodispatch ) const
        {
            ComCon cc;
            cc.first = 0;
            cc.second = 0;
            return cc;
        }

    };
}

#endif
