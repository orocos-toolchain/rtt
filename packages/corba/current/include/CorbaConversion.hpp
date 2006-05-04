#ifndef ORO_CORBA_CONVERSION_HPP
#define ORO_CORBA_CONVERSION_HPP

#include <string>
#include <tao/Any.h>
#include <tao/CORBA_String.h>
#include "corba/OrocosTypesC.h"
#include "corelib/Logger.hpp"
#include "corelib/DataSourceTypeInfo.hpp"
#include "corba/AttributesC.h"

namespace ORO_CoreLib
{
    /**
     * This class converts a given application-specific type
     * to a CORBA::Any object and vice versa.
     * This class works with 'template specialisation'.
     * To create your own any conversion, specialise
     * this class for your application's C++ or IDL type.
     * Also, your specialisation must be in the ORO_CoreLib
     * namespace in order to be found by the CORBA-enabled classes
     * such as Property or the ORO_Execution::TaskContext.
     * @param Type is the Standard C++ or IDL type which
     * is used throughout your application.
     */
    template<class Type>
    struct AnyConversion
    {
        typedef CORBA::Any CorbaType;
        typedef Type StdType;

        /**
         * Updates tp with the contents of any.
         * @param any Contains possibly data of a type convertible to tp
         * @param tp A C++ or CORBA type in which the contents of the
         * any are written.
         * @return true if the any was convertible to tp.
         */
        static bool update(const CORBA::Any& any, StdType tp) {
            Logger::log() << Logger::Debug << "Failing conversion of type "<<detail::DataSourceTypeInfo<StdType>::getType()<<"." <<Logger::endl;
            return false;
        }

        /**
         * Creates an CORBA::Any object out of a C++/IDL type.
         * @param tp The value to convert to an Any.
         * @return An Any object which contains tp or an
         * empty Any object if the conversion was not possible.
         */
        static CORBA::Any_ptr createAny( StdType tp ) {
            Logger::log() << Logger::Error << "Failing Corba::Any creation of type "<<detail::DataSourceTypeInfo<StdType>::getType()<<"." <<Logger::endl;
            return new CORBA::Any();
        }
    };

    template<class Type>
    struct AnyConversionHelper
    {
        typedef Type CorbaType;
        typedef Type StdType;
        static const Type& toAny(const Type& t ) {
            //Logger::log() << Logger::Debug << "Converting type "<<detail::DataSourceTypeInfo<Type>::getType()<<" to same CORBA type." <<Logger::endl;
            return t;
        }
        static Type& fromAny( Type& t ) {
            return t;
        }
        static const Type& get(const Type& t) {
            return t;
        }

        static bool update(const CORBA::Any& any, StdType& _value) {
            if ( any >>= _value ) {
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( const Type& t ) {
            CORBA::Any_ptr ret = new CORBA::Any();
            //Logger::log() << Logger::Debug << "Creating Corba::Any from "<<detail::DataSourceTypeInfo<Type>::getType()<<"." <<Logger::endl;
            *ret <<= toAny( t );
            return ret;
        }

    };

    template<>
    struct AnyConversion<double> : public AnyConversionHelper<double> {};

    template<>
    struct AnyConversion<float> : public AnyConversionHelper<float> {};

    template<>
    struct AnyConversion<int> : public AnyConversionHelper<int> {};

    template<>
    struct AnyConversion<long> : public AnyConversionHelper<long> {};

    template<>
    struct AnyConversion<unsigned int> : public AnyConversionHelper<unsigned int> {};

    template<>
    struct AnyConversion<CORBA::Any_ptr>
    {
        typedef CORBA::Any_ptr CorbaType;
        typedef CORBA::Any_ptr StdType;

        static bool update(const CORBA::Any& any, CORBA::Any_ptr _value) {
            //Logger::log() << Logger::Debug << "Updating type CORBA::Any_ptr with CORBA::Any." <<Logger::endl;
            *_value = any;
            return true;
        }

        static CORBA::Any_ptr createAny( CORBA::Any_ptr t ) {
            //Logger::log() << Logger::Debug << "Duplicating CORBA::Any_ptr." <<Logger::endl;
            return new CORBA::Any(*t);
        }
    };

    template<>
    struct AnyConversion<CORBA::Any_var>
    {
        typedef CORBA::Any_ptr CorbaType;
        typedef CORBA::Any_var StdType;

        static bool update(const CORBA::Any& any, CORBA::Any_var _value) {
            //Logger::log() << Logger::Debug << "Updating type CORBA::Any_var with CORBA::Any." <<Logger::endl;
            *_value.out() = any;
            return true;
        }

        static CORBA::Any_ptr createAny( CORBA::Any_var t ) {
            //Logger::log() << Logger::Debug << "Duplicating CORBA::Any_var." <<Logger::endl;
            return new CORBA::Any( t.in() );
        }
    };

    template<>
    struct AnyConversion<PropertyBag>
    {
        typedef Orocos::AttributeInterface_ptr CorbaType;
        typedef PropertyBag StdType;

        static bool update(const CORBA::Any& any, StdType& _value);
        static CORBA::Any_ptr createAny( StdType t );
    };

    template<>
    struct AnyConversion<bool>
    {
        typedef CORBA::Boolean CorbaType;
        typedef bool StdType;
        static CORBA::Any::from_boolean toAny( bool t ) {
            //Logger::log() << Logger::Debug << "Converting type 'bool' to from_boolean." <<Logger::endl;
            return CORBA::Any::from_boolean(t);
        }
        static CORBA::Any::to_boolean fromAny( CORBA::Boolean& t ) {
            return CORBA::Any::to_boolean(t);
        }
        static StdType get(const CORBA::Boolean t) {
            return t;
        }

        static bool update(const CORBA::Any& any, StdType& _value) {
            CorbaType result;
            if ( any >>= AnyConversion<bool>::fromAny( result ) ) {
                _value = AnyConversion<bool>::get(result);
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( bool t ) {
            CORBA::Any_ptr ret = new CORBA::Any();
            *ret <<= toAny( t );
            return ret;
        }
    };

    template<>
    struct AnyConversion<char>
    {
        typedef CORBA::Char CorbaType;
        typedef char StdType;
        static CORBA::Any::from_char toAny( StdType t ) {
            return CORBA::Any::from_char(t);
        }
        static CORBA::Any::to_char fromAny( CorbaType& t ) {
            return CORBA::Any::to_char(t);
        }
        static StdType get(const CorbaType t) {
            return t;
        }

        static bool update(const CORBA::Any& any, StdType& _value) {
            CorbaType result;
            if ( any >>= AnyConversion<StdType>::fromAny( result ) ) {
                _value = AnyConversion<StdType>::get(result);
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( char t ) {
            CORBA::Any_ptr ret = new CORBA::Any();
            *ret <<= toAny( t );
            return ret;
        }
    };

    template<>
    struct AnyConversion<std::string>
    {
        typedef const char* CorbaType;
        typedef std::string StdType;
        static CorbaType toAny(const std::string& orig) {
            //Logger::log() << Logger::Debug << "Converting type 'string' to const char*." <<Logger::endl;
            return orig.c_str();
        }

        static StdType get(const CorbaType t) {
            return StdType( t );
        }

        static bool update(const CORBA::Any& any, StdType& _value) {
            CorbaType result;
            //Logger::log() << Logger::Debug << "Updating std::string with Any." <<Logger::endl;
            if ( any >>= result ) {
                _value = result;
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( const std::string& t ) {
            CORBA::Any_ptr ret = new CORBA::Any();
            *ret <<= toAny( t );
            return ret;
        }

    };

    template<>
    struct AnyConversion< std::vector<double> >
    {
        typedef Orocos::DoubleSequence CorbaType;
        typedef std::vector<double> StdType;
        static CorbaType* toAny(const std::vector<double>& orig) {
            //Logger::log() << Logger::Debug << "Converting type 'std::vector<double>' to sequence<CORBA::Double>." <<Logger::endl;
            CorbaType* ret = new CorbaType();
            ret->length( orig.size() );
            for( size_t i = 0; i != orig.size(); ++i)
                (*ret)[i] = orig[i];
            return ret;
        }

        static StdType get(const CorbaType* t) {
            StdType ret;
            ret.resize( t->length() );
            for( size_t i = 0; i != t->length(); ++i)
                ret[i] = (*t)[i];
            return ret;
        }

        static bool update(const CORBA::Any& any, StdType& _value) {
            CorbaType* result;
            if ( any >>= result ) {
                _value.resize( result->length() );
                for( size_t i = 0; i != result->length(); ++i)
                    _value[i] = (*result)[i];
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( const std::vector<double>& t ) {
            CORBA::Any_ptr ret = new CORBA::Any();
            *ret <<= toAny( t );
            return ret;
        }

    };
 
    
}

#endif
