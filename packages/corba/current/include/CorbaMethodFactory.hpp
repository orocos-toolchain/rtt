/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  CorbaMethodFactory.hpp 

                        CorbaMethodFactory.hpp -  description
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
 
 

#ifndef ORO_CORBAMETHODFACTORY_HPP
#define ORO_CORBAMETHODFACTORY_HPP
 
#include "../OperationInterface.hpp"
#include "../FactoryExceptions.hpp"
#include "ExpressionProxy.hpp"
#include "FactoriesC.h"

namespace RTT
{namespace Corba
{

    /**
     * A local factory for creating remote Corba methods.
     * It connects to an Orocos::MethodFactory and translates
     * Corba objects to plain C++ objects.
     */
    class CorbaMethodFactory
        : public RTT::detail::OperationFactoryPart<DataSourceBase*>
    {
        Orocos::MethodInterface_var mfact;
        std::string method;
    public:
        typedef std::vector<DataSourceBase::shared_ptr> Arguments;
        typedef std::vector<std::string> Members;
        typedef std::vector< ArgumentDescription > Descriptions;

        CorbaMethodFactory( const std::string& method_name, Orocos::MethodInterface_ptr fact )
            : RTT::detail::OperationFactoryPart<DataSourceBase*>("Corba Method"),
              mfact(Orocos::MethodInterface::_duplicate(fact) ), method(method_name)
        {}

        virtual ~CorbaMethodFactory() {}

        virtual int arity()  const {
            return this->getArgumentList().size();
        }

        virtual std::string resultType() const {
            try {
                CORBA::String_var result = mfact->getResultType( method.c_str() );
                return std::string( result.in() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual std::string description() const {
            try {
                CORBA::String_var result = mfact->getDescription( method.c_str() );
                return std::string( result.in() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual std::vector< ArgumentDescription > getArgumentList() const {
            Descriptions ret;
            try {
                Orocos::Descriptions_var result = mfact->getArguments( method.c_str() );
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

        virtual PropertyBag
        getArgumentSpec( ) const {
            return PropertyBag();
        }

        virtual DataSourceBase* produce( const Arguments& args ) const {
            Orocos::Arguments_var nargs = new Orocos::Arguments();
            nargs->length( args.size() );
            for (size_t i=0; i < args.size(); ++i )
                nargs[i] = args[i]->server();
            try {
                Orocos::Expression_var result = mfact->createMethod( method.c_str(), nargs.in() );
                return ExpressionProxy::CreateDataSource( result._retn() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            } catch ( Orocos::WrongNumbArgException& wa ) {
                throw wrong_number_of_args_exception( wa.wanted, wa.received );
            } catch ( Orocos::WrongTypeArgException& wta ) {
                throw wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
            }
            return 0; // not reached.
        }
    };

}}

#endif
