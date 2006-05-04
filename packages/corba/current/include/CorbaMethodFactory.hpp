
#ifndef ORO_CORBAMETHODFACTORY_HPP
#define ORO_CORBAMETHODFACTORY_HPP
 
#include <execution/MemberFactoryInterface.hpp>
#include <execution/FactoryExceptions.hpp>
#include "FactoriesC.h"

namespace ORO_Corba
{

    /**
     * A local factory for creating remote Corba methods.
     * It connects to an Orocos::MethodFactory and translates
     * Corba objects to plain C++ objects.
     */
    class CorbaMethodFactory
        : public ORO_Execution::MemberFactoryInterface
    {
        Orocos::MethodInterface_var mfact;
        std::string mobjname;
    public:
        typedef std::vector<ORO_CoreLib::DataSourceBase::shared_ptr> Arguments;
        typedef std::vector<std::string> Members;
        typedef std::vector< ORO_Execution::ArgumentDescription > Descriptions;

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
                throw ORO_Execution::name_not_found_exception( nsn.name.in() );
            }
            return false;
        }

        virtual std::string getResultType( const std::string& method ) const {
            try {
                CORBA::String_var result = mfact->getResultType( mobjname.c_str(), method.c_str() );
                return std::string( result.in() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw ORO_Execution::name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual std::string getDescription( const std::string& method ) const {
            try {
                CORBA::String_var result = mfact->getDescription( mobjname.c_str(), method.c_str() );
                return std::string( result.in() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw ORO_Execution::name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual std::vector< ORO_Execution::ArgumentDescription > getArgumentList( const std::string& method ) const {
            Descriptions ret;
            try {
                Orocos::Descriptions_var result = mfact->getArguments( mobjname.c_str(), method.c_str() );
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

        virtual ORO_CoreLib::PropertyBag
        getArgumentSpec( const std::string& method ) const {
            return ORO_CoreLib::PropertyBag();
        }

        virtual ORO_CoreLib::DataSourceBase* create(
                                                    const std::string& name,
                                                    const ORO_CoreLib::PropertyBag& args ) const {
            return 0;
        }

        virtual ORO_CoreLib::DataSourceBase* create(
                                                    const std::string& name,
                                                    const Arguments& args ) const {
            Orocos::Arguments_var nargs = new Orocos::Arguments();
            nargs->length( args.size() );
            for (size_t i=0; i < args.size(); ++i )
                nargs[i] = args[i]->server();
            try {
                Orocos::Expression_var result = mfact->createMethod( mobjname.c_str(), name.c_str(), nargs.in() );
                return ExpressionProxy::Create( result._retn() );
            } catch ( Orocos::NoSuchNameException& nsn ) {
                throw ORO_Execution::name_not_found_exception( nsn.name.in() );
            } catch ( Orocos::WrongNumbArgException& wa ) {
                throw ORO_Execution::wrong_number_of_args_exception( wa.wanted, wa.received );
            } catch ( Orocos::WrongTypeArgException& wta ) {
                throw ORO_Execution::wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
            }
            return 0; // not reached.
        }

        virtual ORO_CoreLib::DataSourceBase* create(
                                                    const std::string& name,
                                                    const std::vector<ORO_CoreLib::DataSourceBase*>& args ) const {
            return 0;
        }
    };

}

#endif
