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
 
#include "MemberFactoryInterface.hpp"
#include "FactoryExceptions.hpp"
#include "FactoriesC.h"

namespace Corba
{

    /**
     * A local factory for creating remote Corba methods.
     * It connects to an Orocos::MethodFactory and translates
     * Corba objects to plain C++ objects.
     */
    class CorbaMethodFactory
        : public MemberFactoryInterface
    {
        Orocos::MethodInterface_var mfact;
        std::string mobjname;
    public:
        typedef std::vector<DataSourceBase::shared_ptr> Arguments;
        typedef std::vector<std::string> Members;
        typedef std::vector< ArgumentDescription > Descriptions;

        CorbaMethodFactory( std::string objname, Orocos::MethodInterface_ptr fact )
            : mfact(Orocos::MethodInterface::_duplicate(fact) ), mobjname(objname)
        {}

        virtual ~CorbaMethodFactory() {}

        virtual Members getNames() const {
            Members ret;
            Orocos::MethodList_var result = mfact->getMethods( mobjname.c_str() );
            ret.reserve( result->length() );
            for (size_t i=0; i!= result->length(); ++i)
                ret.push_back( std::string( result[i] ) );
            return ret;
        }

        virtual bool hasMember( const std::string& s ) const {
            try {
                Orocos::MethodList_var result = mfact->getMethods( mobjname.c_str() );
                for (size_t i=0; i!= result->length(); ++i)
                    if ( s == std::string(result[i]) )
                        return true;
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            }
            return false;
        }

        virtual int getArity( const std::string& method )  const {
            if (this->hasMember(method) == false)
                return -1;
            return this->getArgumentList(method).size();
        }

        virtual std::string getResultType( const std::string& method ) const {
            try {
                CORBA::String_var result = mfact->getResultType( mobjname.c_str(), method.c_str() );
                return std::string( result.in() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual std::string getDescription( const std::string& method ) const {
            try {
                CORBA::String_var result = mfact->getDescription( mobjname.c_str(), method.c_str() );
                return std::string( result.in() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual std::vector< ArgumentDescription > getArgumentList( const std::string& method ) const {
            Descriptions ret;
            try {
                Orocos::Descriptions_var result = mfact->getArguments( mobjname.c_str(), method.c_str() );
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
        getArgumentSpec( const std::string& method ) const {
            return PropertyBag();
        }

        virtual DataSourceBase* create(
                                                    const std::string& name,
                                                    const PropertyBag& args ) const {
            return 0;
        }

        virtual DataSourceBase* create(
                                                    const std::string& name,
                                                    const Arguments& args ) const {
            Orocos::Arguments_var nargs = new Orocos::Arguments();
            nargs->length( args.size() );
            for (size_t i=0; i < args.size(); ++i )
                nargs[i] = args[i]->server();
            try {
                Orocos::Expression_var result = mfact->createMethod( mobjname.c_str(), name.c_str(), nargs.in() );
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

        virtual DataSourceBase* create(
                                                    const std::string& name,
                                                    const std::vector<DataSourceBase*>& args ) const {
            return 0;
        }
    };

}

#endif
