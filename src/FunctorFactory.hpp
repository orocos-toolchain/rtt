/***************************************************************************
  tag: FMTC  do nov 2 13:05:59 CET 2006  FunctorFactory.hpp 

                        FunctorFactory.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
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
 
 
#ifndef ORO_RTT_FUNCTOR_FACTORY_HPP
#define ORO_RTT_FUNCTOR_FACTORY_HPP

#include "DataSource.hpp"
#include "FactoryExceptions.hpp"
#include "Exceptions.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * @internal
         * Minimal implementation to convert a function object to a 
         * DataSource accepting functor.
         * @todo Use this code as a base class for OperationFactoryParts.
         */
        template<typename ResultT>
        class FunctorFactoryPart
        {
        public:
            virtual ~FunctorFactoryPart() {};

            /**
             * Create one part (function object) for a given component.
             * @param args The arguments for the target object's function.
             */
            virtual ResultT produce( const std::vector<DataSourceBase::shared_ptr>& args ) const = 0;
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart0
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart0( FInit f )
                : fun( f )
            {
            }

            ResultT produce(
                            const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( ! args.empty() )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 0, args.size() ), ResultT());
                return fun.create();
            }
        };

        template<typename ResultT, typename FunctorT, typename arg1_type = typename FunctorT::traits::arg1_type>
        class FunctorFactoryPart1
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart1( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 1 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 1, args.size() ), ResultT());
                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( ! a )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                return fun.create( a.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart2
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart2( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 2 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 2, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());

                return fun.create(a.get(), b.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart3
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;

            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart3( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 3 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 3, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( args[2] );
                if ( !c ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart4
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;
            typedef typename FunctorT::traits::arg4_type arg4_type;

            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart4( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 4 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 4, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( args[2] );
                if ( !c ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());
                typename DataSource<arg4_type>::shared_ptr d =
                    AdaptDataSource<arg4_type>()( args[3] );
                if ( !d )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 4, DataSource<arg4_type>::GetType(), args[3]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get(), d.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart5
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;
            typedef typename FunctorT::traits::arg4_type arg4_type;
            typedef typename FunctorT::traits::arg5_type arg5_type;

            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart5( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 5 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 5, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( args[2] );
                if ( !c ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());
                typename DataSource<arg4_type>::shared_ptr d =
                    AdaptDataSource<arg4_type>()( args[3] );
                if ( !d )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 4, DataSource<arg4_type>::GetType(), args[3]->getType() ), ResultT());

                typename DataSource<arg5_type>::shared_ptr e =
                    AdaptDataSource<arg5_type>()( args[4] );
                if ( !e )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 5, DataSource<arg5_type>::GetType(), args[4]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get(), d.get(), e.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart6
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;
            typedef typename FunctorT::traits::arg4_type arg4_type;
            typedef typename FunctorT::traits::arg5_type arg5_type;
            typedef typename FunctorT::traits::arg6_type arg6_type;

            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart6( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 6 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 6, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( args[2] );
                if ( !c ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());
                typename DataSource<arg4_type>::shared_ptr d =
                    AdaptDataSource<arg4_type>()( args[3] );
                if ( !d )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 4, DataSource<arg4_type>::GetType(), args[3]->getType() ), ResultT());

                typename DataSource<arg5_type>::shared_ptr e =
                    AdaptDataSource<arg5_type>()( args[4] );
                if ( !e )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 5, DataSource<arg5_type>::GetType(), args[4]->getType() ), ResultT());

                typename DataSource<arg6_type>::shared_ptr f =
                    AdaptDataSource<arg6_type>()( args[5] );
                if ( !f )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 6, DataSource<arg6_type>::GetType(), args[5]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get(), d.get(), e.get(), f.get() );
            }
        };

    }
}

#endif
#ifndef ORO_RTT_FUNCTOR_FACTORY_HPP
#define ORO_RTT_FUNCTOR_FACTORY_HPP

#include "DataSource.hpp"
#include "FactoryExceptions.hpp"
#include "Exceptions.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * @internal
         * Minimal implementation to convert a function object to a 
         * DataSource accepting functor.
         * @todo Use this code as a base class for OperationFactoryParts.
         */
        template<typename ResultT>
        class FunctorFactoryPart
        {
        public:
            virtual ~FunctorFactoryPart() {};

            /**
             * Create one part (function object) for a given component.
             * @param args The arguments for the target object's function.
             */
            virtual ResultT produce( const std::vector<DataSourceBase::shared_ptr>& args ) const = 0;
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart0
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart0( FInit f )
                : fun( f )
            {
            }

            ResultT produce(
                            const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( ! args.empty() )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 0, args.size() ), ResultT());
                return fun.create();
            }
        };

        template<typename ResultT, typename FunctorT, typename arg1_type = typename FunctorT::traits::arg1_type>
        class FunctorFactoryPart1
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart1( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 1 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 1, args.size() ), ResultT());
                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( ! a )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                return fun.create( a.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart2
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart2( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 2 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 2, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());

                return fun.create(a.get(), b.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart3
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;

            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart3( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 3 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 3, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( args[2] );
                if ( !c ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart4
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;
            typedef typename FunctorT::traits::arg4_type arg4_type;

            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart4( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 4 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 4, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( args[2] );
                if ( !c ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());
                typename DataSource<arg4_type>::shared_ptr d =
                    AdaptDataSource<arg4_type>()( args[3] );
                if ( !d )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 4, DataSource<arg4_type>::GetType(), args[3]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get(), d.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart5
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;
            typedef typename FunctorT::traits::arg4_type arg4_type;
            typedef typename FunctorT::traits::arg5_type arg5_type;

            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart5( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 5 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 5, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( args[2] );
                if ( !c ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());
                typename DataSource<arg4_type>::shared_ptr d =
                    AdaptDataSource<arg4_type>()( args[3] );
                if ( !d )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 4, DataSource<arg4_type>::GetType(), args[3]->getType() ), ResultT());

                typename DataSource<arg5_type>::shared_ptr e =
                    AdaptDataSource<arg5_type>()( args[4] );
                if ( !e )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 5, DataSource<arg5_type>::GetType(), args[4]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get(), d.get(), e.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart6
            : public FunctorFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;
            typedef typename FunctorT::traits::arg4_type arg4_type;
            typedef typename FunctorT::traits::arg5_type arg5_type;
            typedef typename FunctorT::traits::arg6_type arg6_type;

            fun_t fun;
        public:
            template<class FInit>
            FunctorFactoryPart6( FInit f)
                : fun( f )
            {
            }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 6 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 6, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( args[0] );
                if ( !a ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( args[1] );
                if ( !b ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( args[2] );
                if ( !c ) 
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());
                typename DataSource<arg4_type>::shared_ptr d =
                    AdaptDataSource<arg4_type>()( args[3] );
                if ( !d )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 4, DataSource<arg4_type>::GetType(), args[3]->getType() ), ResultT());

                typename DataSource<arg5_type>::shared_ptr e =
                    AdaptDataSource<arg5_type>()( args[4] );
                if ( !e )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 5, DataSource<arg5_type>::GetType(), args[4]->getType() ), ResultT());

                typename DataSource<arg6_type>::shared_ptr f =
                    AdaptDataSource<arg6_type>()( args[5] );
                if ( !f )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 6, DataSource<arg6_type>::GetType(), args[5]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get(), d.get(), e.get(), f.get() );
            }
        };

    }
}

#endif
