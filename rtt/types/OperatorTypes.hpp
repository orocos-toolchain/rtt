/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  OperatorTypes.hpp

                        OperatorTypes.hpp -  description
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


#ifndef ORO_CORELIB_OPERATOP_TYPES_HPP
#define ORO_CORELIB_OPERATOP_TYPES_HPP

#include "Operators.hpp"
#include "../internal/DataSources.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{
    namespace types {

        /**
         * An operator which reads a single internal::DataSource
         * and returns a modified result.
         */
        template<typename function>
        class UnaryOperator
            : public UnaryOp
        {
            typedef typename internal::remove_cr<typename function::argument_type>::type arg_t;
            typedef typename internal::remove_cr<typename function::result_type>::type result_t;
            const char* mop;
            function fun;
        public:
            UnaryOperator( const char* op, function f )
                : mop( op ), fun( f )
            {
            }
            internal::DataSource<result_t>* build( const std::string& op, base::DataSourceBase* a )
            {
                if ( op != mop ) return 0;
                base::DataSourceBase::shared_ptr dsb = a;
                typename internal::DataSource<arg_t>::shared_ptr arg =
                    boost::dynamic_pointer_cast< internal::DataSource<arg_t> >( dsb ); // do not call convert(a) here ! Would always succeed.
                if ( ! arg ) return 0;
                return new internal::UnaryDataSource<function>( arg, fun );
            }
        };


        /**
         * An operator which reads a two DataSources
         * and returns a modified result.
         */
        template<typename function>
        class BinaryOperator
            : public BinaryOp
        {
            typedef typename internal::remove_cr<typename function::first_argument_type>::type arg1_t;
            typedef typename internal::remove_cr<typename function::second_argument_type>::type arg2_t;
            typedef typename internal::remove_cr<typename function::result_type>::type result_t;
            const char* mop;
            function fun;
        public:
            BinaryOperator( const char* op, function f )
                : mop( op ), fun( f )
            {
            }
            internal::DataSource<result_t>* build( const std::string& op, base::DataSourceBase* a,
                                         base::DataSourceBase* b )
            {
                // operation (+,-,...) and first argument type must match.
                if ( op != mop || a->getTypeInfo() != internal::DataSourceTypeInfo<arg1_t>::getTypeInfo() ) return 0;
                //         Logger::log() << Logger::Debug << "BinaryOperator: "<< op << Logger::nl;
                base::DataSourceBase::shared_ptr dsb = a;
                typename internal::DataSource<arg1_t>::shared_ptr arg1 =
                    boost::dynamic_pointer_cast< internal::DataSource<arg1_t> >( dsb ); // first argument must be exact match.
                typename internal::DataSource<arg2_t>::shared_ptr arg2 =
                    boost::dynamic_pointer_cast< internal::DataSource<arg2_t> >( internal::DataSourceTypeInfo<arg2_t>::getTypeInfo()->convert(b) );
                //         Logger::log() << "arg1 : "<< arg1 <<" second arg: "<<arg2<<"..." << Logger::endl;
                //         Logger::log() << "arg1 was: "<< typeid(arg1).name()  <<" a was: "<<typeid(a).name()<<"..." << Logger::endl;
                if ( !arg1 || ! arg2 ) return 0;
                //         Logger::log() << "success !"<< Logger::endl;
                return new internal::BinaryDataSource<function>( arg1, arg2, fun );
            }
        };

        /**
         *  Dot : '.' for member access of composite values
         */
        template<typename function>
        class DotOperator
            : public DotOp
        {
            typedef typename internal::remove_cr<typename function::argument_type>::type arg1_t;
            typedef typename internal::remove_cr<typename function::result_type>::type result_t;
            const char* memb;
            function fun;
        public:
            /**
             * Create a Dot '.' accessor to a composite parser type.
             */
            DotOperator( const char* m, function f )
                : memb( m ), fun( f )
            {
            }
            internal::DataSource<result_t>* build( const std::string& member, base::DataSourceBase* a)
            {
                if ( member != memb ) return 0;
                //         Logger::log() << Logger::Debug << "DotOperator: "<< op << Logger::nl;
                base::DataSourceBase::shared_ptr dsb = a;
                typename internal::DataSource<arg1_t>::shared_ptr arg1 =
                    boost::dynamic_pointer_cast< internal::DataSource<arg1_t> >( dsb );
                if ( !arg1 ) return 0;
                //         Logger::log() << "success !"<< Logger::endl;
                return new internal::UnaryDataSource<function>( arg1, fun );
            }
        };

    /**
     * helper function to create a new UnaryOperator
     */
    template<typename function>
    UnaryOperator<function>*
    newUnaryOperator( const char* op, function f )
    {
        return new UnaryOperator<function>( op, f );
    }

    /**
     * helper function to create a new BinaryOperator
     */
    template<typename function>
    BinaryOperator<function>*
    newBinaryOperator( const char* op, function f )
    {
        return new BinaryOperator<function>( op, f );
    }

    /**
     * helper function to create a new DotOperator
     */
    template<typename function>
    DotOperator<function>*
    newDotOperator( const char* member, function f )
    {
        return new DotOperator<function>( member, f );
    }
}}
#endif
