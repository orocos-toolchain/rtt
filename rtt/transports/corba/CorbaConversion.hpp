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

#include "../../Logger.hpp"
#include "../../internal/DataSourceTypeInfo.hpp"


namespace RTT
{ namespace corba {

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

}}

#endif
