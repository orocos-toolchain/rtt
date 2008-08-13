/***************************************************************************
  tag: FMTC  do nov 2 13:06:07 CET 2006  OperationFactory.hpp

                        OperationFactory.hpp -  description
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


#ifndef ORO_OPERATION_FACTORY_HPP
#define ORO_OPERATION_FACTORY_HPP


#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <string>

#include "mystd.hpp"

#include "PropertyBag.hpp"
#include "Property.hpp"
#include "DataSourceAdaptor.hpp"
#include "Exceptions.hpp"
#include "DataSource.hpp"
#include "FactoryExceptions.hpp"
#include "ArgumentDescription.hpp"
#include "DispatchInterface.hpp"

/**
 * @file OperationFactory.hpp This file contains some code that is common
 * for the CommandRepository and Method Repository.
 */

namespace RTT
{
    namespace detail {

        /**
         * @internal
         * @defgroup OperationFactoryPart Base Classes for parts.
         * @brief Class keeping the information on how to generate one thing that
         * the factory can generate.
         *
         * Each name in the OperationFactory will
         * be linked with one OperationFactoryPart that knows how to produce
         * the thing that the name is used for..  Below are standard
         * implementations for functors of various signatures.
         *
         * @todo The OperationFactoryPartN classes could use
         * FunctorFactoryPartN internally for the produce method. Now
         * this is duplicate code.
         *
         *@{
         */
        template<typename ResultT>
        class OperationFactoryPart
        {
            const char* mdesc;
        public:
            OperationFactoryPart( const char* desc )
                : mdesc( desc )
            {
            }

            virtual ~OperationFactoryPart() {};

            virtual std::string description() const
            {
                return mdesc;
            }
            /**
             * Return the result (return) type of this part.
             */
            virtual std::string resultType() const = 0;

            /**
             * Get a description of the desired arguments in
             * the ArgumentDescription format.
             */
            virtual std::vector<ArgumentDescription> getArgumentList() const = 0;

            /**
             * Returns the arity (number of arguments) of this part.
             */
            virtual int arity() const = 0;

            /**
             * Create one part (function object) for a given component.
             * @param args The arguments for the target object's function.
             */
            virtual ResultT produce( const std::vector<DataSourceBase::shared_ptr>& args ) const = 0;
        };

        template<typename ResultT, typename FunctorT>
        class OperationFactoryPart0
            : public OperationFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            fun_t fun;
        public:
            template<class InitF>
            OperationFactoryPart0( InitF f, const char* desc )
                : OperationFactoryPart<ResultT>( desc ), fun( f )
            {
            }

            std::string resultType() const
            {
                return DataSource<typename FunctorT::result_type>::GetType();
            }

            int arity() const { return 0; }

            std::vector< ArgumentDescription > getArgumentList( ) const
            {
                std::vector< ArgumentDescription > mlist;
                return mlist;
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
        class OperationFactoryPart1
            : public OperationFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            fun_t fun;
            const char* arg1name;
            const char* arg1desc;
        public:
            template<class InitF>
            OperationFactoryPart1( InitF f, const char* desc,
                                   const char* a1n, const char* a1d )
                : OperationFactoryPart<ResultT>( desc ),
                  fun( f ), arg1name( a1n ), arg1desc( a1d )
            {
            }

            std::string resultType() const
            {
                return DataSource<typename FunctorT::result_type>::GetType();
            }

            std::vector< ArgumentDescription > getArgumentList( ) const
            {
                std::vector< ArgumentDescription > mlist;
                mlist.push_back( ArgumentDescription( arg1name, arg1desc, DataSource<arg1_type>::GetType() ) );
                return mlist;
            }

            int arity() const { return 1; }

            ResultT produce(
                            const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 1 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 1, args.size() ), ResultT());
                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( DataSourceTypeInfo<arg1_type>::getTypeInfo()->convert(args[0]) );
                if ( ! a )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                return fun.create( a.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class OperationFactoryPart2
            : public OperationFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            fun_t fun;
            const char* arg1name;
            const char* arg1desc;
            const char* arg2name;
            const char* arg2desc;
        public:
            template<class InitF>
            OperationFactoryPart2( InitF f, const char* desc, const char* a1n,
                                   const char* a1d, const char* a2n,
                                   const char* a2d)
                : OperationFactoryPart<ResultT>( desc ), fun( f ),
                  arg1name( a1n ), arg1desc( a1d ), arg2name( a2n ),
                  arg2desc( a2d )
            {
            }

            std::string resultType() const
            {
                return DataSource<typename FunctorT::result_type>::GetType();
            }

            std::vector< ArgumentDescription > getArgumentList( ) const
            {
                std::vector< ArgumentDescription > mlist;
                mlist.push_back( ArgumentDescription( arg1name, arg1desc, DataSource<arg1_type>::GetType() ) );
                mlist.push_back( ArgumentDescription( arg2name, arg2desc, DataSource<arg2_type>::GetType() ) );
                return mlist;
            }

            int arity() const { return 2; }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 2 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 2, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( DataSourceTypeInfo<arg1_type>::getTypeInfo()->convert(args[0]) );
                if ( !a )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( DataSourceTypeInfo<arg2_type>::getTypeInfo()->convert(args[1]) );
                if ( !b )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());

                return fun.create(a.get(), b.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class OperationFactoryPart3
            : public OperationFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;

            fun_t fun;
            const char* arg1name;
            const char* arg1desc;
            const char* arg2name;
            const char* arg2desc;
            const char* arg3name;
            const char* arg3desc;
        public:
            template<class InitF>
            OperationFactoryPart3( InitF f, const char* desc, const char* a1n,
                                   const char* a1d, const char* a2n,
                                   const char* a2d, const char* a3n,
                                   const char* a3d )
                : OperationFactoryPart<ResultT>( desc ), fun( f ),
                  arg1name( a1n ), arg1desc( a1d ),
                  arg2name( a2n ), arg2desc( a2d ),
                  arg3name( a3n ), arg3desc( a3d )
            {
            }

            std::string resultType() const
            {
                return DataSource<typename FunctorT::result_type>::GetType();
            }

            std::vector< ArgumentDescription > getArgumentList( ) const
            {
                std::vector< ArgumentDescription > mlist;
                mlist.push_back( ArgumentDescription( arg1name, arg1desc, DataSource<arg1_type>::GetType() ) );
                mlist.push_back( ArgumentDescription( arg2name, arg2desc, DataSource<arg2_type>::GetType() ) );
                mlist.push_back( ArgumentDescription( arg3name, arg3desc, DataSource<arg3_type>::GetType() ) );
                return mlist;
            }

            int arity() const { return 3; }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 3 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 3, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( DataSourceTypeInfo<arg1_type>::getTypeInfo()->convert(args[0]) );
                if ( !a )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( DataSourceTypeInfo<arg2_type>::getTypeInfo()->convert(args[1]) );
                if ( !b )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( DataSourceTypeInfo<arg3_type>::getTypeInfo()->convert(args[2]) );
                if ( !c )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get() );
            }
        };

        template<typename ResultT, typename FunctorT>
        class OperationFactoryPart4
            : public OperationFactoryPart<ResultT>
        {
            typedef FunctorT fun_t;
            typedef typename FunctorT::traits::arg1_type arg1_type;
            typedef typename FunctorT::traits::arg2_type arg2_type;
            typedef typename FunctorT::traits::arg3_type arg3_type;
            typedef typename FunctorT::traits::arg4_type arg4_type;

            fun_t fun;
            const char* arg1name;
            const char* arg1desc;
            const char* arg2name;
            const char* arg2desc;
            const char* arg3name;
            const char* arg3desc;
            const char* arg4name;
            const char* arg4desc;
        public:
            template<class InitF>
            OperationFactoryPart4( InitF f, const char* desc, const char* a1n,
                                   const char* a1d, const char* a2n,
                                   const char* a2d, const char* a3n,
                                   const char* a3d, const char* a4n,
                                   const char* a4d )
                : OperationFactoryPart<ResultT>( desc ), fun( f ),
                  arg1name( a1n ), arg1desc( a1d ),
                  arg2name( a2n ), arg2desc( a2d ),
                  arg3name( a3n ), arg3desc( a3d ),
                  arg4name( a4n ), arg4desc( a4d )
            {
            }

            std::string resultType() const
            {
                return DataSource<typename FunctorT::result_type>::GetType();
            }

            std::vector< ArgumentDescription > getArgumentList( ) const
            {
                std::vector< ArgumentDescription > mlist;
                mlist.push_back( ArgumentDescription( arg1name, arg1desc, DataSource<arg1_type>::GetType() ) );
                mlist.push_back( ArgumentDescription( arg2name, arg2desc, DataSource<arg2_type>::GetType() ) );
                mlist.push_back( ArgumentDescription( arg3name, arg3desc, DataSource<arg3_type>::GetType() ) );
                mlist.push_back( ArgumentDescription( arg4name, arg4desc, DataSource<arg4_type>::GetType() ) );
                return mlist;
            }

            int arity() const { return 4; }

            ResultT produce(const std::vector<DataSourceBase::shared_ptr>& args) const
            {
                if ( args.size() != 4 )
                    ORO_THROW_OR_RETURN(wrong_number_of_args_exception( 4, args.size() ), ResultT());

                typename DataSource<arg1_type>::shared_ptr a =
                    AdaptDataSource<arg1_type>()( DataSourceTypeInfo<arg1_type>::getTypeInfo()->convert(args[0]) );
                if ( !a )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 1, DataSource<arg1_type>::GetType(), args[0]->getType() ), ResultT());
                typename DataSource<arg2_type>::shared_ptr b =
                    AdaptDataSource<arg2_type>()( DataSourceTypeInfo<arg2_type>::getTypeInfo()->convert(args[1]) );
                if ( !b )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 2, DataSource<arg2_type>::GetType(), args[1]->getType() ), ResultT());
                typename DataSource<arg3_type>::shared_ptr c =
                    AdaptDataSource<arg3_type>()( DataSourceTypeInfo<arg3_type>::getTypeInfo()->convert(args[2]) );
                if ( !c )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 3, DataSource<arg3_type>::GetType(), args[2]->getType() ), ResultT());
                typename DataSource<arg4_type>::shared_ptr d =
                    AdaptDataSource<arg4_type>()( DataSourceTypeInfo<arg4_type>::getTypeInfo()->convert(args[3]) );
                if ( !d )
                    ORO_THROW_OR_RETURN(wrong_types_of_args_exception( 4, DataSource<arg4_type>::GetType(), args[3]->getType() ), ResultT());

                return fun.create(a.get(), b.get(), c.get(), d.get() );
            }
        };
        /**
         * @}
         */
    }

    /**
     * @brief This factory is a template for creating parts.
     */
    template<typename ResultT>
    class OperationFactory
    {
    protected:
        typedef std::map<std::string, detail::OperationFactoryPart<ResultT>* > map_t;
        map_t data;
    public:
        /**
         * The descriptions of an argumentlist.
         */
        typedef std::vector<ArgumentDescription> Descriptions;

        /**
         * The arguments for an operation.
         */
        typedef std::vector<DataSourceBase::shared_ptr> Arguments;

        /**
         * Remove and delete all added operations.
         */
        void clear() {
            for ( typename map_t::iterator i = data.begin(); i != data.end(); ++i )
                delete i->second;
            data.clear();
        }

        /**
         * Get a list of all the names of the added operations.
         */
        std::vector<std::string> getNames() const
        {
            std::vector<std::string> ret;
            std::transform( data.begin(), data.end(),
                            std::back_inserter( ret ),
                            select1st<typename map_t::value_type>() );
            return ret;
        }

        /**
         * Query if an operation is present.
         */
        bool hasMember( const std::string& name ) const
        {
            return data.find( name ) != data.end();
        }

        /**
         * Query the number of arguments of an operation
         *
         * @param name The name of the operation
         *
         * @return The arity, or -1 if \a name is not found.
         */
        int getArity( const std::string& name ) const
        {
            typename map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0 ) return -1;
            return i->second->arity();
        }
        /**
         * Produce an object that contains an operation.
         *
         * @param name The name of the operation
         * @param args The arguments filled in as properties.
         *
         * @return a new object.
         */
        ResultT produce( const std::string& name, const PropertyBag& args ) const
        {
            typename map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), ResultT());
            std::vector<DataSourceBase::shared_ptr> dsVect;
            std::transform( args.begin(), args.end(),
                            std::back_inserter( dsVect ),
                            boost::bind( &PropertyBase::getDataSource, _1));
            return i->second->produce(dsVect);
        }

        /**
         * Produce an object that contains an operation
         *
         * @param name The name of the operation
         * @param args The arguments filled in as data sources.
         *
         * @return a new object
         */
        ResultT produce( const std::string& name,
                         const std::vector<DataSourceBase::shared_ptr>& args ) const
        {
            typename map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), ResultT());
            return i->second->produce( args );
        }

        /**
         * Get the names and descriptions of all arguments of an operation.
         *
         * @param name The name of the operation
         *
         * @return A list of descriptions.
         */
        Descriptions getArgumentList( const std::string& name ) const
        {
            typename map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), Descriptions());
            return i->second->getArgumentList();
        }

        /**
         * Get the type name of the result type of an operation.
         *
         * @param name The name of the operation
         *
         * @return A name of a data type.
         */
        std::string getResultType( const std::string& name ) const
        {
            typename map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), std::string());
            return i->second->resultType();
        }

        /**
         * Get the description of an operation
         *
         * @param name The name of the operation
         *
         * @return A user readable description.
         */
        std::string getDescription( const std::string& name ) const
        {
            typename map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), std::string());
            return i->second->description();
        }

        /**
         * Add a new operation to the interface.
         *
         * @param name The name of the operation
         * @param part A part which creates the operation.
         */
        void add( const std::string& name,
                  detail::OperationFactoryPart<ResultT>* part )
        {
            typename map_t::iterator i = data.find( name );
            // XXX, wouldn't it be better to throw ?
            if ( i != data.end() )
                delete i->second;
            data[name] = part;
        }

        /**
         * Remove an added operation from the interface
         *
         * @param name The name of the operation
         */
        void remove( const std::string& name )
        {
            typename map_t::iterator i = data.find( name );
            if ( i != data.end() ) {
                delete i->second;
                data.erase(i);
            }
        }
    };

    typedef OperationFactory<DispatchInterface*> CommandFactory;
    typedef OperationFactory<DataSourceBase*> MethodFactory;
}

#endif
