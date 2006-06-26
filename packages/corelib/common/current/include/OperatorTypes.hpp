/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  OperatorTypes.hpp 

                        OperatorTypes.hpp -  description
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
 
 
#ifndef ORO_CORELIB_OPERATOP_TYPES_HPP
#define ORO_CORELIB_OPERATOP_TYPES_HPP

#include "Operators.hpp"
#include "DataSources.hpp"
#include "DataSourceAdaptor.hpp"

namespace ORO_CoreLib
{
    namespace detail {

        /**
         * An operator which reads a single DataSource
         * and returns a modified result.
         */
        template<typename function>
        class UnaryOperator
            : public UnaryOp
        {
            typedef typename function::argument_type arg_t;
            typedef typename function::result_type result_t;
            const char* mop;
            function fun;
        public:
            UnaryOperator( const char* op, function f )
                : mop( op ), fun( f )
            {
            }
            DataSource<result_t>* build( const std::string& op, DataSourceBase* a )
            {
                if ( op != mop ) return 0;
                typename DataSource<arg_t>::shared_ptr arg =
                    AdaptDataSource<arg_t>()( a );
                if ( ! arg ) return 0;
                return new UnaryDataSource<function>( arg, fun );
            }
        };

        /**
         * helper function for the use of UnaryOperator
         */
        template<typename function>
        UnaryOperator<function>*
        newUnaryOperator( const char* op, function f )
        {
            return new UnaryOperator<function>( op, f );
        }


        /**
         * An operator which reads a two DataSources
         * and returns a modified result.
         */
        template<typename function>
        class BinaryOperator
            : public BinaryOp
        {
            typedef typename function::first_argument_type arg1_t;
            typedef typename function::second_argument_type arg2_t;
            typedef typename function::result_type result_t;
            const char* mop;
            function fun;
        public:
            BinaryOperator( const char* op, function f )
                : mop( op ), fun( f )
            {
            }
            DataSource<result_t>* build( const std::string& op, DataSourceBase* a,
                                         DataSourceBase* b )
            {
                if ( op != mop ) return 0;
                //         Logger::log() << Logger::Debug << "BinaryOperator: "<< op << Logger::nl;
                typename DataSource<arg1_t>::shared_ptr arg1 =
                    AdaptDataSource<arg1_t>()( a );
                typename DataSource<arg2_t>::shared_ptr arg2 =
                    AdaptDataSource<arg2_t>()( b );
                //         Logger::log() << "arg1 : "<< arg1 <<" second arg: "<<arg2<<"..." << Logger::endl;
                //         Logger::log() << "arg1 was: "<< typeid(arg1).name()  <<" a was: "<<typeid(a).name()<<"..." << Logger::endl;
                if ( !arg1 || ! arg2 ) return 0;
                //         Logger::log() << "success !"<< Logger::endl;
                return new BinaryDataSource<function>( arg1, arg2, fun );
            }
        };

        template<typename function>
        BinaryOperator<function>*
        newBinaryOperator( const char* op, function f )
        {
            return new BinaryOperator<function>( op, f );
        }


        /**
         * An operator which reads a three DataSources
         * and returns a modified result.
         */
        template<typename function>
        class TernaryOperator
            : public TernaryOp
        {
            typedef typename function::first_argument_type arg1_t;
            typedef typename function::second_argument_type arg2_t;
            typedef typename function::third_argument_type arg3_t;
            typedef typename function::result_type result_t;
            const char* mop;
            function fun;
        public:
            TernaryOperator( const char* op, function f )
                : mop( op ), fun( f )
            {
            }
            DataSource<result_t>* build( const std::string& op, DataSourceBase* a,
                                         DataSourceBase* b, DataSourceBase* c )
            {
                if ( op != mop ) return 0;
                typename DataSource<arg1_t>::shared_ptr arg1 =
                    AdaptDataSource<arg1_t>()( a );
                typename DataSource<arg2_t>::shared_ptr arg2 =
                    AdaptDataSource<arg2_t>()( b );
                typename DataSource<arg3_t>::shared_ptr arg3 =
                    AdaptDataSource<arg3_t>()( c );
                if ( !arg1 || ! arg2 || !arg3 ) return 0;
                return new TernaryDataSource<function>( arg1, arg2, arg3, fun );
            }
        };

        template<typename function>
        TernaryOperator<function>*
        newTernaryOperator( const char* op, function f )
        {
            return new TernaryOperator<function>( op, f );
        }


        /**
         * An operator which reads a six DataSources
         * and returns a modified result.
         */
        template<typename function>
        class SixaryOperator
            : public SixaryOp
        {
            typedef typename function::first_argument_type arg1_t;
            typedef typename function::second_argument_type arg2_t;
            typedef typename function::third_argument_type arg3_t;
            typedef typename function::fourth_argument_type arg4_t;
            typedef typename function::fifth_argument_type arg5_t;
            typedef typename function::sixth_argument_type arg6_t;
            typedef typename function::result_type result_t;
            const char* mop;
            function fun;
        public:
            SixaryOperator( const char* op, function f )
                : mop( op ), fun( f )
            {
            }
            DataSource<result_t>* build( const std::string& op,
                                         DataSourceBase* a, DataSourceBase* b, DataSourceBase* c,
                                         DataSourceBase* d, DataSourceBase* e, DataSourceBase* f )
            {
                if ( op != mop ) return 0;
                typename DataSource<arg1_t>::shared_ptr arg1 =
                    AdaptDataSource<arg1_t>()( a );
                typename DataSource<arg2_t>::shared_ptr arg2 =
                    AdaptDataSource<arg2_t>()( b );
                typename DataSource<arg3_t>::shared_ptr arg3 =
                    AdaptDataSource<arg3_t>()( c );
                typename DataSource<arg4_t>::shared_ptr arg4 =
                    AdaptDataSource<arg4_t>()( d );
                typename DataSource<arg5_t>::shared_ptr arg5 =
                    AdaptDataSource<arg5_t>()( e );
                typename DataSource<arg6_t>::shared_ptr arg6 =
                    AdaptDataSource<arg6_t>()( f );
                if ( !arg1 || ! arg2 || !arg3 || !arg4 || !arg5 || !arg6 ) return 0;
                return new SixaryDataSource<function>( arg1, arg2, arg3, arg4, arg5, arg6, fun );
            }
        };

        template<typename function>
        SixaryOperator<function>*
        newSixaryOperator( const char* op, function f )
        {
            return new SixaryOperator<function>( op, f );
        }

        /**
         *  Dot : '.' for member access of composite values
         */
        template<typename function>
        class DotOperator
            : public DotOp
        {
            typedef typename function::argument_type arg1_t;
            typedef typename function::result_type result_t;
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
            DataSource<result_t>* build( const std::string& member, DataSourceBase* a)
            {
                if ( member != memb ) return 0;
                //         Logger::log() << Logger::Debug << "DotOperator: "<< op << Logger::nl;
                typename DataSource<arg1_t>::shared_ptr arg1 =
                    AdaptDataSource<arg1_t>()( a );
                if ( !arg1 ) return 0;
                //         Logger::log() << "success !"<< Logger::endl;
                return new UnaryDataSource<function>( arg1, fun );
            }
        };

        template<typename function>
        DotOperator<function>*
        newDotOperator( const char* member, function f )
        {
            return new DotOperator<function>( member, f );
        }

    }
}
#endif
