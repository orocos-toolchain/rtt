/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  OperationInterfacePartFused.hpp

                        OperationInterfacePartFused.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_OPERATION_INTERFACE_PART_FUSED_HPP
#define ORO_OPERATION_INTERFACE_PART_FUSED_HPP


#include <boost/shared_ptr.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
// Fusion behaviour changed at Boost 1.42
#include <boost/version.hpp>
#if BOOST_VERSION >= 104100
// our code goes up to 7 FUSION_MAX_VECTOR_SIZE defaults to 10
#ifndef BOOST_FUSION_UNFUSED_MAX_ARITY
#define BOOST_FUSION_UNFUSED_MAX_ARITY 7
#endif
#include <boost/functional/forward_adapter.hpp>
#include <boost/fusion/functional/generation/make_unfused.hpp>
#else
// our code goes up to 7 FUSION_MAX_VECTOR_SIZE defaults to 10
#ifndef BOOST_FUSION_UNFUSED_GENERIC_MAX_ARITY
#define BOOST_FUSION_UNFUSED_GENERIC_MAX_ARITY 7
#endif
#include <boost/fusion/include/make_unfused_generic.hpp>
#endif

#include <vector>
#include <string>

#include "rtt/internal/DataSource.hpp"
#include "rtt/internal/CreateSequence.hpp"
#include "rtt/internal/FusedFunctorDataSource.hpp"
#include "rtt/OperationInterfacePart.hpp"
#include "rtt/FactoryExceptions.hpp"
#include "rtt/Operation.hpp"
#include "rtt/base/OperationCallerBase.hpp"

/**
 * @file OperationInterfacePart.hpp This file contains the code
 * required to register operations as scriptable entities.
 */

namespace RTT
{
    namespace internal {

        class RTT_API OperationInterfacePartHelper {
        public:
            static std::string description(base::OperationBase* ob);
            static std::vector<ArgumentDescription> getArgumentList(base::OperationBase* ob, const int arity, std::vector<std::string> const& types);
        };

        /**
         * OperationInterfacePart implementation that uses boost::fusion
         * to produce items.
         */
        template<typename Signature>
        class OperationInterfacePartFused
            : public OperationInterfacePart
        {
        protected:
            typedef typename boost::function_traits<Signature>::result_type result_type;
            //! The factory for converting data sources to C++ types in call()
            typedef create_sequence<typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
            //! The factory for converting data sources to C++ types in collect(). This includes SendHandle.
            typedef create_sequence<typename boost::function_types::parameter_types<typename CollectType<Signature>::type>::type > CollectSequenceFactory;
            Operation<Signature>* op;
        public:
            OperationInterfacePartFused( Operation<Signature>* o)
                : op(o)
            {
            }

            virtual std::string getName() const {
                return op->getName();
            }

            virtual std::string description() const {
                return OperationInterfacePartHelper::description( op );
            }

            virtual std::vector<ArgumentDescription> getArgumentList() const {
                std::vector<std::string> types;
                for (unsigned int i = 1; i <= OperationInterfacePartFused::arity(); ++i )
                    types.push_back( SequenceFactory::GetType(i) );
                return OperationInterfacePartHelper::getArgumentList( op, OperationInterfacePartFused::arity(), types );
            }

            std::string resultType() const
            {
                return DataSourceTypeInfo<result_type>::getType() + DataSourceTypeInfo<result_type>::getQualifier();
            }

            unsigned int arity() const { return boost::function_traits<Signature>::arity; }

            const types::TypeInfo* getArgumentType(unsigned int arg) const
            {
                if (arg == 0 )
                    return internal::DataSourceTypeInfo<result_type>::getTypeInfo();
                return SequenceFactory::GetTypeInfo(arg);
            }

            unsigned int collectArity() const { return boost::function_traits< typename CollectType<Signature>::type >::arity; }

            const types::TypeInfo* getCollectType(unsigned int arg) const
            {
                return CollectSequenceFactory::GetTypeInfo(arg);
            }

            base::DataSourceBase::shared_ptr produce(
                            const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const
            {
                // convert our args and signature into a boost::fusion Sequence.
                if ( args.size() != OperationInterfacePartFused::arity() )
                    throw wrong_number_of_args_exception(OperationInterfacePartFused::arity(), args.size() );
                return new FusedMCallDataSource<Signature>(typename base::OperationCallerBase<Signature>::shared_ptr(op->getOperationCaller()->cloneI(caller)), SequenceFactory::sources(args.begin()) );
            }

            virtual base::DataSourceBase::shared_ptr produceSend( const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller ) const {
                // convert our args and signature into a boost::fusion Sequence.
                if ( args.size() != OperationInterfacePartFused::arity() ) throw wrong_number_of_args_exception(OperationInterfacePartFused::arity(), args.size() );
                return new FusedMSendDataSource<Signature>(typename base::OperationCallerBase<Signature>::shared_ptr(op->getOperationCaller()->cloneI(caller)), SequenceFactory::sources(args.begin()) );
            }

            virtual base::DataSourceBase::shared_ptr produceHandle() const {
                // Because of copy/clone,program script variables ('var') must begin unbound.
                return new internal::UnboundDataSource<ValueDataSource<SendHandle<Signature> > >();
            }

            virtual base::DataSourceBase::shared_ptr produceCollect( const std::vector<base::DataSourceBase::shared_ptr>& args, DataSource<bool>::shared_ptr blocking ) const {
                const unsigned int carity = boost::mpl::size<typename FusedMCollectDataSource<Signature>::handle_and_arg_types>::value;
                assert( carity == collectArity() + 1 ); // check for arity functions. (this is actually a compile time assert).
                if ( args.size() != carity ) throw wrong_number_of_args_exception(carity, args.size() );
                // we need to ask FusedMCollectDataSource what the arg types are, based on the collect signature.
                return new FusedMCollectDataSource<Signature>( create_sequence<typename FusedMCollectDataSource<Signature>::handle_and_arg_types >::assignable(args.begin()), blocking );
            }

#ifdef ORO_SIGNALLING_OPERATIONS
            virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* subscriber) const
            {
                throw no_asynchronous_operation_exception("cannot use produceSignal on non-signalling operations");
            }
#endif
            boost::shared_ptr<base::DisposableInterface> getLocalOperation() const {
                return op->getImplementation();
            }
        };


#ifdef ORO_SIGNALLING_OPERATIONS
        /**
         * OperationInterfacePart implementation for signalling operations that uses boost::fusion
         * to produce items.
         */
        template<typename Signature>
        class EventOperationInterfacePartFused
            : public OperationInterfacePartFused<Signature>
        {
            typedef typename boost::function_traits<Signature>::result_type result_type;
            //! The factory for converting data sources to C++ types in call()
            typedef create_sequence<typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
            //! The factory for converting data sources to C++ types in collect(). This includes SendHandle.
            typedef create_sequence<typename boost::function_types::parameter_types<typename CollectType<Signature>::type>::type > CollectSequenceFactory;

        public:
            EventOperationInterfacePartFused( Operation<Signature>* o)
                : OperationInterfacePartFused<Signature>(o)
            {
            }


            virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* subscriber) const {
                base::OperationCallerInterface::shared_ptr impl = this->op->getOperationCaller();
                if ( subscriber == impl->getMessageProcessor() )
                    subscriber = 0; // clear out to avoid dispatching
                // convert our args and signature into a boost::fusion Sequence.
                if ( args.size() != OperationInterfacePartFused<Signature>::arity() ) throw wrong_number_of_args_exception(OperationInterfacePartFused<Signature>::arity(), args.size() );
                // note: in boost 1.41.0+ the function make_unfused() is available.
#if BOOST_VERSION >= 104100
#if __cplusplus > 199711L
                auto invoke_fused = boost::bind(&FusedMSignal<Signature>::invoke,
                                        boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin()), subscriber),
                                        _1
                                    );
                typedef typename boost::fusion::result_of::make_unfused< decltype(invoke_fused) >::type unfused_type;
                return this->op->signals(boost::forward_adapter<unfused_type>(boost::fusion::make_unfused(invoke_fused)));
#else // __cplusplus > 199711L
                return this->op->signals(
                            boost::fusion::make_unfused(boost::bind(&FusedMSignal<Signature>::invoke,
                                                                    boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin()), subscriber),
                                                                    _1
                                                                    )
                                                        )
                            );
#endif // __cplusplus > 199711L
#else // BOOST_VERSION >= 104100
                return this->op->signals(
                            boost::fusion::make_unfused_generic(boost::bind(&FusedMSignal<Signature>::invoke,
                                                                    boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin()), subscriber),
                                                                    _1
                                                                    )
                                                        )
                            );
#endif // BOOST_VERSION >= 104100
            }
        };
#endif // ORO_SIGNALLING_OPERATIONS

        /**
         * OperationInterfacePart implementation that only provides synchronous
         * access to an operation. Only produce() can be called, the others will
         * throw a no_asynchronous_operation_exception.
         */
        template<typename Signature>
        class SynchronousOperationInterfacePartFused
            : public OperationInterfacePart
        {
            typedef typename boost::function_traits<Signature>::result_type result_type;
            //! The factory for converting data sources to C++ types in call()
            typedef create_sequence<typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
            //! The factory for converting data sources to C++ types in collect(). This includes SendHandle.
            typedef create_sequence<typename boost::function_types::parameter_types<typename CollectType<Signature>::type>::type > CollectSequenceFactory;
            Operation<Signature>* op;
        public:
            SynchronousOperationInterfacePartFused( Operation<Signature>* o)
                : op(o)
            {
            }

            virtual base::DataSourceBase::shared_ptr produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const
            { throw no_asynchronous_operation_exception("cannot use produceSend on synchronous operations"); }
            virtual base::DataSourceBase::shared_ptr produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking) const
            { throw no_asynchronous_operation_exception("cannot use produceCollect on synchronous operations"); }
#ifdef ORO_SIGNALLING_OPERATIONS
            virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* subscriber) const
            { throw no_asynchronous_operation_exception("cannot use produceSignal on synchronous operations"); }
#endif
            virtual base::DataSourceBase::shared_ptr produceHandle() const
            { throw no_asynchronous_operation_exception("cannot use produceHandle on synchronous operations"); }

            virtual std::string getName() const {
                return op->getName();
            }
            virtual std::string description() const {
                return OperationInterfacePartHelper::description( op );
            }

            virtual std::vector<ArgumentDescription> getArgumentList() const {
                std::vector<std::string> types;
                for (unsigned int i = 1; i <= SynchronousOperationInterfacePartFused::arity(); ++i )
                    types.push_back( SequenceFactory::GetType(i) );
                return OperationInterfacePartHelper::getArgumentList( op, SynchronousOperationInterfacePartFused::arity(), types );
            }

            std::string resultType() const
            {
                return DataSourceTypeInfo<result_type>::getType() + DataSourceTypeInfo<result_type>::getQualifier();
            }

            unsigned int arity() const { return boost::function_traits<Signature>::arity; }

            const types::TypeInfo* getArgumentType(unsigned int arg) const
            {
                if (arg == 0 )
                    return internal::DataSourceTypeInfo<result_type>::getTypeInfo();
                return SequenceFactory::GetTypeInfo(arg);
            }

            unsigned int collectArity() const { return boost::function_traits< typename CollectType<Signature>::type >::arity; }

            const types::TypeInfo* getCollectType(unsigned int arg) const
            {
                return CollectSequenceFactory::GetTypeInfo(arg);
            }

            base::DataSourceBase::shared_ptr produce(
                            const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const
            {
                // convert our args and signature into a boost::fusion Sequence.
                if ( args.size() != SynchronousOperationInterfacePartFused::arity() )
                    throw wrong_number_of_args_exception(SynchronousOperationInterfacePartFused::arity(), args.size() );
                return new FusedMCallDataSource<Signature>(typename base::OperationCallerBase<Signature>::shared_ptr(op->getOperationCaller()->cloneI(caller)), SequenceFactory::sources(args.begin()) );
            }

            boost::shared_ptr<base::DisposableInterface> getLocalOperation() const {
                return op->getImplementation();
            }
        };

            /**
             * OperationInterfacePart implementation that uses boost::fusion
             * to produce items. The methods invoked get their object pointer
             * from the first data source, which contains a shared_ptr.
             * @param Signature The full signature, with the object pointer as
             * the first argument.
             * @param ObjT The object pointer type used in Signature.
             */
            template<typename Signature,typename ObjT>
            class OperationInterfacePartFusedDS
                : public OperationInterfacePart
            {
                typedef create_sequence<typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
                typedef create_sequence<typename boost::function_types::parameter_types<typename CollectType<Signature>::type>::type > CollectSequenceFactory;
                typedef typename boost::function_traits<Signature>::result_type result_type;
                Operation<Signature>* op;
                // the datasource that stores a weak pointer is itself stored by a shared_ptr.
                typename DataSource<boost::shared_ptr<ObjT> >::shared_ptr mwp;
            public:
                OperationInterfacePartFusedDS( DataSource< boost::shared_ptr<ObjT> >* wp, Operation<Signature>* o)
                    : op( o ), mwp(wp)
                {
                }

                typedef std::vector<base::DataSourceBase::shared_ptr> ArgList;

                std::string resultType() const
                {
                    return DataSourceTypeInfo<result_type>::getType() + DataSourceTypeInfo<result_type>::getQualifier();
                }

                unsigned int arity() const { return boost::function_traits<Signature>::arity - 1;/*lie about the hidden member pointer */ }

                const types::TypeInfo* getArgumentType(unsigned int arg) const
                {
                    if (arg == 0 )
                        return internal::DataSourceTypeInfo<result_type>::getTypeInfo();
                    return SequenceFactory::GetTypeInfo(arg);
                }

                unsigned int collectArity() const { return boost::function_traits< typename CollectType<Signature>::type >::arity; }

                const types::TypeInfo* getCollectType(unsigned int arg) const
                {
                    return CollectSequenceFactory::GetTypeInfo(arg);
                }

                virtual std::string getName() const {
                    return op->getName();
                }

                virtual std::string description() const {
                    return op->getDescriptions().front();
                }

                virtual std::vector<ArgumentDescription> getArgumentList() const {
                    std::vector<std::string> descr = op->getDescriptions();
                    std::vector<ArgumentDescription> ret;
                    descr.resize( 1 + arity() * 2 );
                    for (unsigned int i =1; i < descr.size(); i +=2 )
                        ret.push_back(ArgumentDescription(descr[i],descr[i+1], SequenceFactory::GetType((i-1)/2+2)) );
                    return ret;
                }

                base::DataSourceBase::shared_ptr produce(ArgList const& args, ExecutionEngine* caller) const
                {
                    if ( args.size() != arity() ) throw wrong_number_of_args_exception(arity(), args.size() );
                    // the user won't give the necessary object argument, so we glue it in front.
                    ArgList a2;
                    a2.reserve(args.size()+1);
                    a2.push_back(mwp);
                    a2.insert(a2.end(), args.begin(), args.end());
                    // convert our args and signature into a boost::fusion Sequence.
                    return new FusedMCallDataSource<Signature>(typename base::OperationCallerBase<Signature>::shared_ptr(op->getOperationCaller()->cloneI(caller)), SequenceFactory::sources(a2.begin()) );
                }

                virtual base::DataSourceBase::shared_ptr produceSend( const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller ) const {
                    if ( args.size() != arity() ) throw wrong_number_of_args_exception(arity(), args.size() );
                    // the user won't give the necessary object argument, so we glue it in front.
                    ArgList a2;
                    a2.reserve(args.size()+1);
                    a2.push_back(mwp);
                    a2.insert(a2.end(), args.begin(), args.end());
                    // convert our args and signature into a boost::fusion Sequence.
                    return new FusedMSendDataSource<Signature>(typename base::OperationCallerBase<Signature>::shared_ptr(op->getOperationCaller()->cloneI(caller)), SequenceFactory::sources(a2.begin()) );
                }

                virtual base::DataSourceBase::shared_ptr produceHandle() const {
                    // Because of copy/clone,value objects must begin unbound.
                    return new internal::UnboundDataSource<ValueDataSource<SendHandle<Signature> > >();
                }

                virtual base::DataSourceBase::shared_ptr produceCollect( const std::vector<base::DataSourceBase::shared_ptr>& args, DataSource<bool>::shared_ptr blocking ) const {
                    const unsigned int carity = boost::mpl::size<typename FusedMCollectDataSource<Signature>::handle_and_arg_types>::value;
                    assert( carity == collectArity() + 1 ); // check for arity functions. (this is actually a compile time assert).
                    if ( args.size() != carity ) throw wrong_number_of_args_exception(carity, args.size() );
                    // we need to ask FusedMCollectDataSource what the arg types are, based on the collect signature.
                    return new FusedMCollectDataSource<Signature>( create_sequence<typename FusedMCollectDataSource<Signature>::handle_and_arg_types >::assignable(args.begin()), blocking );
                }
#ifdef ORO_SIGNALLING_OPERATIONS
                virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* subscriber) const {
                    if ( args.size() != arity() ) throw wrong_number_of_args_exception(arity(), args.size() );
                    base::OperationCallerInterface::shared_ptr impl = op->getOperationCaller();
                    if ( subscriber == impl->getMessageProcessor() )
                        subscriber = 0; // clear out to avoid dispatching
                    // the user won't give the necessary object argument, so we glue it in front.
                    ArgList a2;
                    a2.reserve(args.size()+1);
                    a2.push_back(mwp);
                    a2.insert(a2.end(), args.begin(), args.end());
                    // note: in boost 1.41.0+ the function make_unfused() is available.
#if BOOST_VERSION >= 104100
#if __cplusplus > 199711L
                    auto invoke_fused = boost::bind(&FusedMSignal<Signature>::invoke,
                                            boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin()), subscriber),
                                            _1
                                        );
                    typedef typename boost::fusion::result_of::make_unfused< decltype(invoke_fused) >::type unfused_type;
                    return this->op->signals(boost::forward_adapter<unfused_type>(boost::fusion::make_unfused(invoke_fused)));
#else // __cplusplus > 199711L
                    return this->op->signals(
                                boost::fusion::make_unfused(boost::bind(&FusedMSignal<Signature>::invoke,
                                                                        boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin()), subscriber),
                                                                        _1
                                                                        )
                                                            )
                                );
#endif // __cplusplus > 199711L
#else // BOOST_VERSION >= 104100
                    return this->op->signals(
                                boost::fusion::make_unfused_generic(boost::bind(&FusedMSignal<Signature>::invoke,
                                                                        boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin()), subscriber),
                                                                        _1
                                                                        )
                                                            )
                                );
#endif // BOOST_VERSION >= 104100
                }
#endif // ORO_SIGNALLING_OPERATIONS

                boost::shared_ptr<base::DisposableInterface> getLocalOperation() const {
                    return this->op->getImplementation();
                }
            };
    }
}

#endif
