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
                    AdaptDataSource<arg_t>()( a ); // do not call convert(a) here ! Would always succeed.
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
                // operation (+,-,...) and first argument type must match.
                if ( op != mop || a->getTypeInfo() != DataSourceTypeInfo<arg1_t>::getTypeInfo() ) return 0;
                //         Logger::log() << Logger::Debug << "BinaryOperator: "<< op << Logger::nl;
                typename DataSource<arg1_t>::shared_ptr arg1 =
                    AdaptDataSource<arg1_t>()( a ); // first argument must be exact match.
                typename DataSource<arg2_t>::shared_ptr arg2 =
                    AdaptDataSource<arg2_t>()( DataSourceTypeInfo<arg2_t>::getTypeInfo()->convert(b) );
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
