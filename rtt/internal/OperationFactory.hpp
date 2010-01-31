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
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>

#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <string>

#include "mystd.hpp"

#include "../PropertyBag.hpp"
#include "../Property.hpp"
#include "DataSourceAdaptor.hpp"
#include "Exceptions.hpp"
#include "DataSource.hpp"
#include "FactoryExceptions.hpp"
#include "ArgumentDescription.hpp"
#include "../base/DispatchInterface.hpp"
#include "CreateSequence.hpp"
#include "FusedFunctorDataSource.hpp"
#include "../Operation.hpp"

/**
 * @file OperationFactory.hpp This file contains the code
 * required to register operations as scriptable entities.
 */

namespace RTT
{
    namespace internal {

        /**
         * @internal
         * @defgroup OperationFactoryPart Base Classes for scriptable parts.
         * @brief Class keeping the information on how to generate one thing that
         * the factory can generate.
         *
         * Each name in the OperationFactory will
         * be linked with one OperationFactoryPart that knows how to produce
         * the thing that the name is used for.  Below is a fused (see Boost::fusion)
         * implementation for functors of various signatures.
         *
         *@{
         */
        class OperationFactoryPart
        {
        public:
            OperationFactoryPart()
            {
            }

            virtual ~OperationFactoryPart() {};

            /**
             * Returns the description of this operation.
             * @return
             */
            virtual std::string description() const = 0;

            /**
             * Get a description of the desired arguments in
             * the ArgumentDescription format.
             */
            virtual std::vector<ArgumentDescription> getArgumentList() const = 0;

            /**
             * Return the result (return) type of this part.
             */
            virtual std::string resultType() const = 0;

            /**
             * Returns the arity (number of arguments) of this part.
             */
            virtual int arity() const = 0;

            /**
             * Create a DataSource for a given callable operation.
             * @param args The arguments for the target object's function.
             * The number of arguments must be identical to this->arity()
             * @param caller The Engine that will receive notifications when
             * the method has been executed, in case it runs in the owner's thread.
             * Normally, this is the engine of the caller's TaskContext.
             * @return a DataSource which will return the result of this operation.
             */
            virtual base::DataSourceBase* produce( const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller ) const = 0;

            /**
             * Create a DataSource for a given send operation.
             * @param args The arguments for the target object's function.
             * The number of arguments must be identical to this->arity()
             * @param caller The Engine that will receive notifications when
             * the method has been executed.
             * Normally, this is the engine of the caller's TaskContext.
             * @return a DataSource which contains the SendHandle of this operation.
             * @see produceHandle
             */
            virtual base::DataSourceBase* produceSend( const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller ) const = 0;

            /**
             * Create an empty SendHandle object for this operation.
             * @return a DataSource which contains an empty SendHandle.
             */
            virtual base::DataSourceBase* produceHandle() const = 0;

            /**
             * Create a DataSource for collecting the results of a Send.
             * @param args A vector of data sources of which the first element
             * contains a properly initialised sendhandle and the remainder of the
             * elements contains datasources for collecting the return value and
             * reference arguments.
             * @param blocking Set to true to do a blocking collect, false for a polling version.
             * @return A DataSource which collects the results when evaluated and which returns
             * the SendStatus.
             */
            virtual base::DataSourceBase* produceCollect( const std::vector<base::DataSourceBase::shared_ptr>& args, bool blocking ) const = 0;
        };

        /**
         * OperationFactoryPart implementation that uses boost::fusion
         * to produce items.
         */
        template<typename Signature>
        class OperationFactoryPartFused
            : public OperationFactoryPart
        {
            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef create_sequence<typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
            Operation<Signature>* op;
        public:
            OperationFactoryPartFused( Operation<Signature>* o)
                : op(o)
            {
            }

            virtual std::string description() const {
                return op->getDescriptions().front();
            }

            virtual std::vector<ArgumentDescription> getArgumentList() const {
                std::vector<std::string> const& descr = op->getDescriptions();
                std::vector<ArgumentDescription> ret;
                for (int i =1; i < descr.size(); i +=2 )
                    ret.push_back(ArgumentDescription(descr[i],descr[i+1], SequenceFactory::GetType((i-1)/2+1) ));
                return ret;
            }

            std::string resultType() const
            {
                return DataSource<result_type>::GetType();
            }

            int arity() const { return boost::function_traits<Signature>::arity; }

            base::DataSourceBase* produce(
                            const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const
            {
                // convert our args and signature into a boost::fusion Sequence.
                return new FusedMCallDataSource<Signature>(typename base::MethodBase<Signature>::shared_ptr(op->getMethod()->cloneI(caller)), SequenceFactory()(args) );
            }

            virtual base::DataSourceBase* produceSend( const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller ) const {
                // convert our args and signature into a boost::fusion Sequence.
                return new FusedMSendDataSource<Signature>(typename base::MethodBase<Signature>::shared_ptr(op->getMethod()->cloneI(caller)), SequenceFactory()(args) );
            }

            virtual base::DataSourceBase* produceHandle() const {
                // Because of copy/clone,program script variables ('var') must begin unbound.
                return new internal::UnboundDataSource<ValueDataSource<SendHandle<Signature> > >();
            }

            virtual base::DataSourceBase* produceCollect( const std::vector<base::DataSourceBase::shared_ptr>& args, bool blocking ) const {
                // we need to ask FusedMCollectDataSource what the arg types are, based on the collect signature.
                return new FusedMCollectDataSource<Signature>( create_sequence<typename FusedMCollectDataSource<Signature>::handle_and_arg_types >()(args), blocking );
            }
        };

            /**
             * OperationFactoryPart implementation that uses boost::fusion
             * to produce items. The methods invoked get their object pointer
             * from the first data source, which contains a shared_ptr.
             * @param Signature The full signature, with the object pointer as
             * the first argument.
             * @param ObjT The object pointer type used in Signature.
             */
            template<typename Signature,typename ObjT>
            class OperationFactoryPartFusedDS
                : public OperationFactoryPart
            {
                typedef create_sequence<typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
                typedef typename boost::function_traits<Signature>::result_type result_type;
                Operation<Signature>* op;
                // the datasource that stores a weak pointer is itself stored by a shared_ptr.
                typename DataSource<boost::shared_ptr<ObjT> >::shared_ptr mwp;
            public:
                OperationFactoryPartFusedDS( DataSource< boost::shared_ptr<ObjT> >* wp, Operation<Signature>* o)
                    : op( o ), mwp(wp)
                {
                }

                typedef std::vector<base::DataSourceBase::shared_ptr> ArgList;

                std::string resultType() const
                {
                    return DataSource<result_type>::GetType();
                }

                int arity() const { return boost::function_traits<Signature>::arity - 1;/*lie about the hidden member pointer */ }

                virtual std::string description() const {
                    return op->getDescriptions().front();
                }

                virtual std::vector<ArgumentDescription> getArgumentList() const {
                    std::vector<std::string> const& descr = op->getDescriptions();
                    std::vector<ArgumentDescription> ret;
                    for (int i =1; i < descr.size(); i +=2 )
                        ret.push_back(ArgumentDescription(descr[i],descr[i+1], SequenceFactory::GetType((i-1)/2+1)) );
                    return ret;
                }

                base::DataSourceBase* produce(ArgList const& args, ExecutionEngine* caller) const
                {
                    // the user won't give the necessary object argument, so we glue it in front.
                    ArgList a2;
                    a2.reserve(args.size()+1);
                    a2.push_back(mwp);
                    a2.insert(a2.end(), args.begin(), args.end());
                    // convert our args and signature into a boost::fusion Sequence.
                    return new FusedMCallDataSource<Signature>(typename base::MethodBase<Signature>::shared_ptr(op->getMethod()->cloneI(caller)), SequenceFactory()(a2) );
                }

                virtual base::DataSourceBase* produceSend( const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller ) const {
                    // the user won't give the necessary object argument, so we glue it in front.
                    ArgList a2;
                    a2.reserve(args.size()+1);
                    a2.push_back(mwp);
                    a2.insert(a2.end(), args.begin(), args.end());
                    // convert our args and signature into a boost::fusion Sequence.
                    return new FusedMSendDataSource<Signature>(typename base::MethodBase<Signature>::shared_ptr(op->getMethod()->cloneI(caller)), SequenceFactory()(a2) );
                }

                virtual base::DataSourceBase* produceHandle() const {
                    // Because of copy/clone,value objects must begin unbound.
                    return new internal::UnboundDataSource<ValueDataSource<SendHandle<Signature>& > >();
                }

                virtual base::DataSourceBase* produceCollect( const std::vector<base::DataSourceBase::shared_ptr>& args, bool blocking ) const {
                    // we need to ask FusedMCollectDataSource what the arg types are, based on the collect signature.
                    return new FusedMCollectDataSource<Signature>( create_sequence<typename FusedMCollectDataSource<Signature>::handle_and_arg_types >()(args), blocking );
                }
            };

        /**
         * @}
         */

    /**
     * @brief This factory is a template for creating parts.
     * @todo Rename to interface::OperationRepository.
     */
    class OperationFactory
    {
    protected:
        typedef std::map<std::string, OperationFactoryPart* > map_t;
        map_t data;
    public:
        /**
         * The arguments for an operation.
         */
        typedef std::vector<base::DataSourceBase::shared_ptr> Arguments;

        /**
         * Remove and delete all added operations.
         */
        void clear() {
            for (  map_t::iterator i = data.begin(); i != data.end(); ++i )
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
                            select1st< map_t::value_type>() );
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
             map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0 ) return -1;
            return i->second->arity();
        }

        /**
         * Produce a DataSource that call()s an operation.
         * The DataSource will return the result of call().
         *
         * @param name The name of the operation
         * @param args The arguments filled in as data sources.
         *
         * @return a new object
         */
        base::DataSourceBase* produce( const std::string& name,
                         const Arguments& args, ExecutionEngine* caller ) const
        {
            map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
            return i->second->produce( args, caller );
        }

        /**
         * Produce a DataSource that send()s an operation.
         * The DataSource will return the SendHandle of that operation.
         *
         * @param name The name of the operation
         * @param args The arguments filled in as data sources.
         *
         * @return a new object
         */
        base::DataSourceBase* produceSend( const std::string& name,
                             const Arguments& args, ExecutionEngine* caller ) const
        {
            map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
            return i->second->produceSend( args, caller );
        }

        /**
         * Produce an AssignableDataSource that contains a SendHandle,
         * fit for the operation.
         * The DataSource will return the SendHandle.
         *
         * @param name The name of the operation
         * @param args The arguments filled in as data sources.
         *
         * @return a new object
         */
        base::DataSourceBase* produceHandle( const std::string& name ) const
        {
            map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
            return i->second->produceHandle();
        }

        /**
         * Produce a DataSource that collects a sent operation,
         * The DataSource will return the SendStatus and store
         * the results in the presented arguments. Note that this
         * function takes most of the time less arguments than its companions.
         *
         * @param name The name of the operation
         * @param args The arguments filled in as data sources.
         * @param blocking Set to true to block on the result.
         *
         * @return a new object
         */
        base::DataSourceBase* produceCollect( const std::string& name,
                                const Arguments& args, bool blocking) const
        {
            map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), 0);
            return i->second->produceCollect( args, blocking );
        }

        /**
         * The descriptions of an argumentlist.
         */
        typedef std::vector<ArgumentDescription> Descriptions;

        /**
         * Get the names and descriptions of all arguments of an operation.
         *
         * @param name The name of the operation
         *
         * @return A list of descriptions.
         */
        Descriptions getArgumentList( const std::string& name ) const
        {
             map_t::const_iterator i = data.find( name );
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
             map_t::const_iterator i = data.find( name );
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
             map_t::const_iterator i = data.find( name );
            if ( i == data.end() || i->second == 0) ORO_THROW_OR_RETURN(name_not_found_exception(), std::string());
            return i->second->description();
        }

        /**
         * Add a new operation to the interface or replace an existing one.
         *
         * @param name The name of the operation
         * @param part A part which creates the operation.
         */
        void add( const std::string& name,
                  OperationFactoryPart* part )
        {
             map_t::iterator i = data.find( name );
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
            map_t::iterator i = data.find( name );
            if ( i != data.end() ) {
                delete i->second;
                data.erase(i);
            }
        }

        /**
         * Get a previously added part of this factory.
         * @param name
         * @return
         */
        OperationFactoryPart* getPart(const std::string& name) {
            map_t::iterator i = data.find( name );
            if ( i != data.end() ) {
                return i->second;
            }
            return 0;
        }
    };

    typedef OperationFactory MethodFactory;
}}

#endif
