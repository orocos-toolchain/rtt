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

namespace RTT
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

    }

    /**
     * helper function to create a new UnaryOperator
     */
    template<typename function>
    detail::UnaryOperator<function>*
    newUnaryOperator( const char* op, function f )
    {
        return new detail::UnaryOperator<function>( op, f );
    }

    /**
     * helper function to create a new BinaryOperator
     */
    template<typename function>
    detail::BinaryOperator<function>*
    newBinaryOperator( const char* op, function f )
    {
        return new detail::BinaryOperator<function>( op, f );
    }

    /**
     * helper function to create a new DotOperator
     */
    template<typename function>
    detail::DotOperator<function>*
    newDotOperator( const char* member, function f )
    {
        return new detail::DotOperator<function>( member, f );
    }
}
#endif
