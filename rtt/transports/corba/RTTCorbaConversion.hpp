/***************************************************************************
tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  CorbaConversion.hpp

CorbaConversion.hpp -  description
-------------------
begin                : Mon June 26 2006
copyright            : (C) 2006 Peter Soetens
email                : peter.soetens@fmtc.be

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


#ifndef ORO_RTT_CORBA_CONVERSION_HPP
#define ORO_RTT_CORBA_CONVERSION_HPP


#include "CorbaConversion.hpp"
#include "OrocosTypesC.h"
#include "TaskContextC.h"
#include "TaskContextServer.hpp"
#include "TaskContextProxy.hpp"
#include "CorbaConnPolicy.hpp"
#ifdef OS_RT_MALLOC
#include <rtt/rt_string.hpp>
#endif

namespace RTT { 
  namespace corba {

    template<>
    struct AnyConversion<double> : public AnyConversionHelper<double> {
      typedef CORBA::DoubleSeq sequence;
    };

    template<>
    struct AnyConversion<float> : public AnyConversionHelper<float> {
      typedef CORBA::FloatSeq sequence;
    };

    template<>
    struct AnyConversion<int> : public AnyConversionHelper<int, CORBA::Long> {
      typedef CORBA::LongSeq sequence;
    };

    //template<>
    //struct AnyConversion<long> : public AnyConversionHelper<long> {};

    template<>
    struct AnyConversion<unsigned int> : public AnyConversionHelper<unsigned int, CORBA::ULong> {
      typedef CORBA::ULongSeq sequence;
    };

    template<>
    struct AnyConversion<long long> : public AnyConversionHelper<long long, CORBA::LongLong> {
      typedef CORBA::LongLongSeq sequence;
    };

    template<>
    struct AnyConversion<unsigned long long> : public AnyConversionHelper<unsigned long long, CORBA::ULongLong> {
      typedef CORBA::ULongLongSeq sequence;
    };

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

      static bool updateAny( const StdType& t, CORBA::Any& any ) {
        //Logger::log() << Logger::Debug << "Duplicating CORBA::Any_ptr." <<Logger::endl;
        any <<= CORBA::Any(*t);
        return true;
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

      static bool updateAny( const StdType& t, CORBA::Any& any ) {
        //Logger::log() << Logger::Debug << "Duplicating CORBA::Any_var." <<Logger::endl;
        any <<= CORBA::Any( t.in() );
        return true;
      }
    };

    template<>
    struct AnyConversion<bool>
    {
      typedef CORBA::Boolean CorbaType;
      typedef bool StdType;

      typedef CORBA::BooleanSeq sequence;

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

      static bool toStdType(StdType& tp, const CorbaType& cb) {
        tp = get(cb);
        return true;
      }
      static bool toCorbaType(CorbaType& cb, const StdType& tp) {
        cb = tp;
        return true;
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

      static bool updateAny( bool t, CORBA::Any& any ) {
        any <<= toAny( t );
        return true;
      }
    };

    template<>
    struct AnyConversion<char>
    {
      typedef CORBA::Char CorbaType;
      typedef char StdType;

      typedef CORBA::CharSeq sequence;

      static CORBA::Any::from_char toAny( StdType t ) {
        return CORBA::Any::from_char(t);
      }

      static CORBA::Any::to_char fromAny( CorbaType& t ) {
        return CORBA::Any::to_char(t);
      }

      static StdType get(const CorbaType t) {
        return t;
      }

      static bool toStdType(StdType& tp, const CorbaType& cb) {
        tp = get(cb);
        return true;
      }
      static bool toCorbaType(CorbaType& cb, const StdType& tp) {
        cb = tp;
        return true;
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

      static bool updateAny( char t, CORBA::Any& any ) {
        any <<= toAny( t );
        return true;
      }
    };

    template<>
    struct AnyConversion<std::string>
    {
      typedef CORBA::StringSeq sequence;

      typedef const char* CorbaType;
      typedef std::string StdType;

      static CorbaType toAny(const std::string& orig) {
        //Logger::log() << Logger::Debug << "Converting type 'string' to const char*." <<Logger::endl;
        return orig.c_str();
      }

      static StdType get(const CorbaType t) {
        return StdType( t );
      }

      static bool toStdType(StdType& dest, const CorbaType src) {
        dest = get(src);
        return true;
      }

      /**
      * Use for the conversion from a stl container to a sequence<string>
      *
      * Note: T needs to have reference semantics, but we cannot explicitly use a reference argument
      * because it would break TAO. See https://github.com/orocos-toolchain/rtt/commit/7cd62561b72467a9dd79757ae22714931d0fa1c9.
      */
      template<class T>
      static bool toCorbaType(T dest, const StdType& src) {
        dest = src.c_str();
        return true;
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

      static bool updateAny( StdType const& t, CORBA::Any& any ) {
        any <<= toAny( t );
        return true;
      }
    };

    /**
    * This specialisation uses the toRTT() and toCORBA functions
    * for converting between RTT::ConnPolicy and RTT::corba::CConPolicy.
    */
    template<>
    struct AnyConversion<ConnPolicy>
    {
      typedef RTT::corba::CConnPolicy CorbaType;
      typedef RTT::ConnPolicy StdType;
      static CorbaType toAny(const StdType& orig) {
        //Logger::log() << Logger::Debug << "Converting type 'string' to const char*." <<Logger::endl;
        return toCORBA(orig);
      }

      static StdType get(const CorbaType t) {
        return toRTT( t );
      }

      static bool update(const CORBA::Any& any, StdType& _value) {
        CorbaType* result;
        //Logger::log() << Logger::Debug << "Updating std::string with Any." <<Logger::endl;
        if ( any >>= result ) {
          _value = toRTT(*result);
          return true;
        }
        return false;
      }

      static CORBA::Any_ptr createAny( const StdType& t ) {
        CORBA::Any_ptr ret = new CORBA::Any();
        *ret <<= toAny( t );
        return ret;
      }

      static bool updateAny( StdType const& t, CORBA::Any& any ) {
        any <<= toAny( t );
        return true;
      }
    };

    template<>
    struct AnyConversion<RTT::TaskContext*>
    {
      typedef RTT::corba::CTaskContext_ptr CorbaType;
      typedef RTT::TaskContext* StdType;

      static bool update(const CORBA::Any& any, StdType& _value) {
        RTT::corba::CTaskContext_ptr task;
        if ( any >>= task ) {
          // read-only insertion, we duplicate the _ptr in Create:
          _value = TaskContextProxy::Create( task );
          return true;
        }
        return true;
      }

      static CORBA::Any_ptr createAny( const StdType& t ) {
        CORBA::Any_ptr ret = new CORBA::Any();
        // copying insertion:
        *ret <<= TaskContextServer::CreateServer(t,false,false);
        return ret;
      }

      static bool updateAny( const StdType& t, CORBA::Any& any ) {
        if (t) {
          // copying insertion:
          any <<= TaskContextServer::CreateServer(t, false, false );
        } else {
          // leave any.
        }
        return true;
      }
    };

#ifdef OS_RT_MALLOC
    template<>
    struct AnyConversion< rt_string >
    {
      typedef const char*     CorbaType;
      typedef rt_string     StdType;

      static bool toCorbaType(CorbaType& cb,const StdType& orig) {
        cb = orig.c_str();
        return true;
      }

      static CorbaType toAny(const StdType& orig) {
        return orig.c_str();
      }

      static bool toStdType(StdType& dest, const CorbaType orig) {
        dest = StdType(orig);
        return true;
      }

      static bool update(const CORBA::Any& any, StdType& ret) {
        CorbaType orig;
        if ( any >>= orig )
          {
            ret = orig;
            return true;
          }
        return false;
      }

      static CORBA::Any_ptr createAny( const StdType& t ) {
        CORBA::Any_ptr ret = new CORBA::Any();
        *ret <<= toAny( t );
        return ret;
      }

      static bool updateAny( StdType const& t, CORBA::Any& any ) {
        any <<= toAny( t );
        return true;
      }

    };
#endif
  }
}

#endif
