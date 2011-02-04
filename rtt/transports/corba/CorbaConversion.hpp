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


#ifndef ORO_CORBA_CONVERSION_HPP
#define ORO_CORBA_CONVERSION_HPP

#include <string>
#include <vector>
#include <map>

#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/Version.h>
#if TAO_MAJOR_VERSION == 1 && TAO_MINOR_VERSION <= 4
#include <tao/Any.h>
#else // TAO 1.5 and higher
#include <tao/AnyTypeCode/Any.h>
#endif
#include <tao/CORBA_String.h>
#else
#include "corba.h"
#include <omniORB4/stringtypes.h>
#endif

#include "OrocosTypesC.h"
#include "../../Logger.hpp"
#include "../../internal/DataSourceTypeInfo.hpp"


namespace RTT { 
  namespace corba {

    /**
    * This class converts a given application-specific type
    * to a CORBA::Any object and vice versa.
    * This class works with 'template specialisation'.
    * To create your own any conversion, specialise
    * this class for your application's C++ or IDL type.
    * Also, your specialisation must be in the RTT
    * namespace in order to be found by the CORBA-enabled classes
    * such as Property or the TaskContext.
    * @param Type is the Standard C++ or IDL type which
    * is used throughout your application.
    */
    template<class Type>
    struct AnyConversion
    {
      /**      
      * corba type
      */
      typedef CORBA::Any CorbaType;
      /**      
      * C++ type
      */
      typedef Type StdType;
      /**      
      * sequence of corba type (used to convert stl container)
      */
      typedef RTT::corba::CAnySequence sequence;

      /**
      * Updates tp with the contents of cb.
      * @param tp A C++ or CORBA type in which the contents of the
      * cb are written.
      * @param cb Contains possibly data of a type convertible to tp
      * @return true if the cb was convertible to tp.
      */
      static bool toStdType(StdType& tp, const CorbaType& cb) {
        Logger::log() << Logger::Error << "Failing conversion of CorbaType to type "<<internal::DataSourceTypeInfo<StdType>::getType()<<"." <<Logger::endl;
        return false;
      }

      /**
      * Updates cb with the contents of tp.
      * @param cb A CORBA type in which the contents of the
      * tp are written.
      * @param tp Contains possibly data of a type convertible to cb
      * @return true if the tp was convertible to cb.
      */
      static bool toCorbaType(CorbaType& cb, const StdType& tp) {
        Logger::log() << Logger::Error << "Failing conversion of  type "<<internal::DataSourceTypeInfo<StdType>::getType()<<"to a CorbaType." <<Logger::endl;
        return false;
      }

      /**
      * Updates tp with the contents of any.
      * @param any Contains possibly data of a type convertible to tp
      * @param tp A C++ or CORBA type in which the contents of the
      * any are written.
      * @return true if the any was convertible to tp.
      */
      static bool update(const CORBA::Any& any, StdType tp) {
        Logger::log() << Logger::Error << "Failing conversion of type "<<internal::DataSourceTypeInfo<StdType>::getType()<<"." <<Logger::endl;
        return false;
      }

      /**
      * Creates an CORBA::Any object out of a C++/IDL type.
      * @param tp The value to convert to an Any.
      * @return An Any object which contains tp or an
      * empty Any object if the conversion was not possible.
      */
      static CORBA::Any_ptr createAny( StdType tp ) {
        Logger::log() << Logger::Error << "Failing corba::Any creation of type "<<internal::DataSourceTypeInfo<StdType>::getType()<<"." <<Logger::endl;
        return new CORBA::Any();
      }

      /**
      * Updates an CORBA::Any object from of a C++/IDL type.
      * @param tp The value to convert to the Any.
      * @param  An Any object which will contain tp or an
      * empty Any object if the conversion was not possible.
      * @return false if no conversion could be made
      */
      static bool updateAny( StdType tp, CORBA::Any& any ) {
        Logger::log() << Logger::Error << "Failing corba::Any updating of type "<<internal::DataSourceTypeInfo<StdType>::getType()<<"." <<Logger::endl;
        return false;
      }
    };

    /**
    * Used for the conversion of types that are binary
    * compatible between CORBA type and std C++ type.
    */
    template<class Type, class _CorbaType = Type>
    struct AnyConversionHelper
    {
      typedef _CorbaType CorbaType;
      typedef Type StdType;
      static CorbaType toAny( Type t ) {
        //Logger::log() << Logger::Debug << "Converting type "<<internal::DataSourceTypeInfo<Type>::getType()<<" to same CORBA type." <<Logger::endl;
        return t;
      }

      static Type& fromAny( Type& t ) {
        return t;
      }

      static const Type& get(const Type& t) {
        return t;
      }

      static bool toStdType(StdType& tp, const CorbaType& cb) {
        tp = cb;
        return true;
      }

      static bool toCorbaType(CorbaType& cb, const StdType& tp) {
        cb = tp;
        return true;
      }

      static bool update(const CORBA::Any& any, StdType& _value) {
        CorbaType temp;
        if ( any >>= temp ) {
          _value = temp;
          return true;
        }
        return false;
      }

      static CORBA::Any_ptr createAny( const Type& t ) {
        CORBA::Any_ptr ret = new CORBA::Any();
        //Logger::log() << Logger::Debug << "Creating corba::Any from "<<internal::DataSourceTypeInfo<Type>::getType()<<"." <<Logger::endl;
        *ret <<= toAny( static_cast<CorbaType>(t) );
        return ret;
      }

      static bool updateAny( const Type& t, CORBA::Any& any ) {
        //Logger::log() << Logger::Debug << "Updating corba::Any from "<<internal::DataSourceTypeInfo<Type>::getType()<<"." <<Logger::endl;
        any <<= toAny( static_cast<CorbaType>(t) );
        return true;
      }
    };

    /**
    * Use for the conversion of a std::vector of C++ type
    * to a Corba sequence, given the conversion between
    * the C++ type and the Corba type
    */
    template<class T>
    struct AnyConversion< std::vector<T> >
    {
      typedef RTT::corba::CAnySequence sequence;

      typedef typename AnyConversion<T>::sequence CorbaType;
      typedef std::vector<T> StdType;

      static bool toStdType(StdType& tp, const CorbaType& cb) {
        bool res = true;
        tp.resize( cb.length() );
        for (size_t i = 0; i != cb.length(); ++i) {
          res = res && AnyConversion<T>::toStdType(tp[i], cb[(CORBA::ULong)(i)]);
        }
        return res;
      }

      static bool toCorbaType(CorbaType& cb, const StdType& tp) {
        bool res = true;
        cb.length( (CORBA::ULong)(tp.size()) );
        for( size_t i = 0; i != tp.size(); ++i) 
          res = res && AnyConversion<T>::toCorbaType(cb[(CORBA::ULong)(i)], tp[i]);
        return res;
      }

      /**
      * if T is a std::vector or a std::map the associated type is CORBA::Any
      */
      static bool toStdType(StdType& tp, const CORBA::Any& any) {
        return update(any, tp);
      }
      static bool toCorbaType(CORBA::Any& any, const StdType& tp) {
        return updateAny(tp, any);
      }

      static CorbaType* toAny(const StdType& tp) {
        CorbaType* cb = new CorbaType();
        toCorbaType(*cb, tp);
        return cb;
      }

      static bool update(const CORBA::Any& any, StdType& _value) {
        CorbaType* result;
        if ( any >>= result ) {
          return toStdType(_value, *result);
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

    /**
    * Use for the conversion of a std::pair of C++ types
    * to a two Corba sequences, given the conversions between
    * the C++ types and the Corba types
    */
    template<class T1, class T2>
    struct AnyConversion<std::pair<T1, T2> > {
      typedef RTT::corba::PairSeq sequence;

      typedef RTT::corba::Pair CorbaType;
      typedef std::pair<T1, T2> StdType;

      static bool toStdType(StdType& tp, const CorbaType& cb) {
        return AnyConversion<T1>::update(cb.t1, tp.first) && AnyConversion<T2>::update(cb.t2, tp.second);
      }

      static bool toCorbaType(CorbaType& cb, const StdType& tp) {
        return AnyConversion<T1>::updateAny(tp.first, cb.t1) && AnyConversion<T2>::updateAny(tp.second, cb.t2);
      }

      static CorbaType* toAny(const StdType& tp) {
        CorbaType* cb = new CorbaType();
        toCorbaType(*cb, tp);
        return cb;
      }

      static StdType get(const CorbaType* cb) {
        StdType tp;
        toStdType(tp, *cb);
        return tp;
      }

      static bool update(const CORBA::Any& any, StdType& _value) {
        CorbaType* result;
        if ( any >>= result ) {
          return toStdType(_value, *result);
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

    /**
    * Use for the conversion of a std::map of C++ types
    * to a two Corba sequences, given the conversions between
    * the C++ types and the Corba types
    */
    template<class T1, class T2>
    struct AnyConversion<std::map<T1, T2> > {
      typedef RTT::corba::CAnySequence sequence;

      typedef RTT::corba::PairSeq CorbaType;
      typedef std::map<T1, T2> StdType;

      static bool toStdType(StdType& tp, const CorbaType& cb) {
        bool res = true;
        tp.clear();

        for (size_t i = 0; i != cb.length(); ++i) {
          std::pair<T1, T2> p;
          res = res && AnyConversion<std::pair<T1, T2> >::toStdType(p, cb[(CORBA::ULong)(i)]);
          tp.insert(p);
        }
        return res;
      }

      static bool toCorbaType(CorbaType& cb, const StdType& tp) {
        bool res = true;
        cb.length(tp.size());
        StdType::const_iterator it = tp.begin();

        for(size_t i = 0; i != tp.size(); i++) {
          res = res &&AnyConversion<std::pair<T1, T2> >::toCorbaType(cb[(CORBA::ULong)(i)], *it);
          it++;
        }
        return res;
      }

      /**
      * if T is a std::vector or a std::map the associated type is CORBA::Any
      */
      static bool toStdType(StdType& tp, const CORBA::Any& any) {
        return update(any, tp);
      }
      static bool toCorbaType(CORBA::Any& any, const StdType& tp) {
        return updateAny(tp, any);
      }

      static CorbaType* toAny(const StdType& tp) {
        CorbaType* cb = new CorbaType();
        toCorbaType(*cb, tp);
        return cb;
      }

      static StdType get(const CorbaType* cb) {
        StdType cb;
        toStdType(cb, *tp);
        return cb;
      }

      static bool update(const CORBA::Any& any, StdType& _value) {
        CorbaType* result;
        if ( any >>= result ) {
          return toStdType(_value, *result);
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
  }
}

#endif
