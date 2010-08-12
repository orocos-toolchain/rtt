#ifndef ORO_OPERATION_REPOSITORY_PART_FUSED_HPP
#define ORO_OPERATION_REPOSITORY_PART_FUSED_HPP


#include <boost/shared_ptr.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
// Fusion behaviour changed at Boost 1.42
#include <boost/version.hpp>
#if BOOST_VERSION >= 104100
#include <boost/fusion/functional/generation/make_unfused.hpp>
#else
#include <boost/fusion/include/make_unfused_generic.hpp>
#endif

#include <vector>
#include <string>

#include "DataSource.hpp"
#include "CreateSequence.hpp"
#include "FusedFunctorDataSource.hpp"
#include "../interface/OperationInterfacePart.hpp"
#include "../interface/FactoryExceptions.hpp"
#include "../Operation.hpp"
#include "../base/OperationCallerBase.hpp"

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
            static std::vector<interface::ArgumentDescription> getArgumentList(base::OperationBase* ob, const int arity, std::vector<std::string> const& types);
        };

        /**
         * OperationInterfacePart implementation that uses boost::fusion
         * to produce items.
         */
        template<typename Signature>
        class OperationInterfacePartFused
            : public interface::OperationInterfacePart
        {
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

            virtual std::string description() const {
                return OperationInterfacePartHelper::description( op );
            }

            virtual std::vector<interface::ArgumentDescription> getArgumentList() const {
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
                    throw interface::wrong_number_of_args_exception(OperationInterfacePartFused::arity(), args.size() );
                return new FusedMCallDataSource<Signature>(typename base::OperationCallerBase<Signature>::shared_ptr(op->getOperationCaller()->cloneI(caller)), SequenceFactory::sources(args.begin()) );
            }

            virtual base::DataSourceBase::shared_ptr produceSend( const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller ) const {
                // convert our args and signature into a boost::fusion Sequence.
                if ( args.size() != OperationInterfacePartFused::arity() ) throw interface::wrong_number_of_args_exception(OperationInterfacePartFused::arity(), args.size() );
                return new FusedMSendDataSource<Signature>(typename base::OperationCallerBase<Signature>::shared_ptr(op->getOperationCaller()->cloneI(caller)), SequenceFactory::sources(args.begin()) );
            }

            virtual base::DataSourceBase::shared_ptr produceHandle() const {
                // Because of copy/clone,program script variables ('var') must begin unbound.
                return new internal::UnboundDataSource<ValueDataSource<SendHandle<Signature> > >();
            }

            virtual base::DataSourceBase::shared_ptr produceCollect( const std::vector<base::DataSourceBase::shared_ptr>& args, DataSource<bool>::shared_ptr blocking ) const {
                const unsigned int carity = boost::mpl::size<typename FusedMCollectDataSource<Signature>::handle_and_arg_types>::value;
                assert( carity == collectArity() + 1 ); // check for arity functions. (this is actually a compile time assert).
                if ( args.size() != carity ) throw interface::wrong_number_of_args_exception(carity, args.size() );
                // we need to ask FusedMCollectDataSource what the arg types are, based on the collect signature.
                return new FusedMCollectDataSource<Signature>( create_sequence<typename FusedMCollectDataSource<Signature>::handle_and_arg_types >::sources(args.begin()), blocking );
            }

            virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args) const {
                // convert our args and signature into a boost::fusion Sequence.
                if ( args.size() != OperationInterfacePartFused::arity() ) throw interface::wrong_number_of_args_exception(OperationInterfacePartFused::arity(), args.size() );
                // note: in boost 1.41.0+ the function make_unfused() is available.
#if BOOST_VERSION >= 104100
                return op->signals( boost::fusion::make_unfused(boost::bind(&FusedMSignal<Signature>::invoke,
                                                                            boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin())),
                                                                            boost::lambda::_1
                                                                            )
                                                                )
                                   );
#else
                return op->signals( boost::fusion::make_unfused_generic(boost::bind(&FusedMSignal<Signature>::invoke,
                                                                            boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin())),
                                                                            boost::lambda::_1
                                                                            )
                                                                )
                                   );
#endif
            }

            boost::shared_ptr<base::DisposableInterface> getLocalOperation() const {
                return op->getImplementation();
            }
        };

        /**
         * OperationInterfacePart implementation that only provides synchronous
         * access to an operation. Only produce() can be called, the others will
         * throw a no_asynchronous_operation_exception.
         */
        template<typename Signature>
        class SynchronousOperationInterfacePartFused
            : public interface::OperationInterfacePart
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
            { throw interface::no_asynchronous_operation_exception("cannot use produceSend on synchronous operations"); }
            virtual base::DataSourceBase::shared_ptr produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking) const
            { throw interface::no_asynchronous_operation_exception("cannot use produceCollect on synchronous operations"); }
            virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args) const
            { throw interface::no_asynchronous_operation_exception("cannot use produceSignal on synchronous operations"); }
            virtual base::DataSourceBase::shared_ptr produceHandle() const
            { throw interface::no_asynchronous_operation_exception("cannot use produceHandle on synchronous operations"); }

            virtual std::string description() const {
                return OperationInterfacePartHelper::description( op );
            }

            virtual std::vector<interface::ArgumentDescription> getArgumentList() const {
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
                    throw interface::wrong_number_of_args_exception(SynchronousOperationInterfacePartFused::arity(), args.size() );
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
                : public interface::OperationInterfacePart
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

                virtual std::string description() const {
                    return op->getDescriptions().front();
                }

                virtual std::vector<interface::ArgumentDescription> getArgumentList() const {
                    std::vector<std::string> const& descr = op->getDescriptions();
                    std::vector<interface::ArgumentDescription> ret;
                    for (unsigned int i =1; i < descr.size(); i +=2 )
                        ret.push_back(interface::ArgumentDescription(descr[i],descr[i+1], SequenceFactory::GetType((i-1)/2+1)) );
                    return ret;
                }

                base::DataSourceBase::shared_ptr produce(ArgList const& args, ExecutionEngine* caller) const
                {
                    if ( args.size() != arity() ) throw interface::wrong_number_of_args_exception(arity(), args.size() );
                    // the user won't give the necessary object argument, so we glue it in front.
                    ArgList a2;
                    a2.reserve(args.size()+1);
                    a2.push_back(mwp);
                    a2.insert(a2.end(), args.begin(), args.end());
                    // convert our args and signature into a boost::fusion Sequence.
                    return new FusedMCallDataSource<Signature>(typename base::OperationCallerBase<Signature>::shared_ptr(op->getOperationCaller()->cloneI(caller)), SequenceFactory::sources(a2.begin()) );
                }

                virtual base::DataSourceBase::shared_ptr produceSend( const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller ) const {
                    if ( args.size() != arity() ) throw interface::wrong_number_of_args_exception(arity(), args.size() );
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
                    if ( args.size() != carity ) throw interface::wrong_number_of_args_exception(carity, args.size() );
                    // we need to ask FusedMCollectDataSource what the arg types are, based on the collect signature.
                    return new FusedMCollectDataSource<Signature>( create_sequence<typename FusedMCollectDataSource<Signature>::handle_and_arg_types >::sources(args.begin()), blocking );
                }

                virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args) const {
                    if ( args.size() != arity() ) throw interface::wrong_number_of_args_exception(arity(), args.size() );
                    // the user won't give the necessary object argument, so we glue it in front.
                    ArgList a2;
                    a2.reserve(args.size()+1);
                    a2.push_back(mwp);
                    a2.insert(a2.end(), args.begin(), args.end());
                    // note: in boost 1.41.0+ the function make_unfused() is available.
    #if BOOST_VERSION >= 104100
                    return op->signals( boost::fusion::make_unfused(boost::bind(&FusedMSignal<Signature>::invoke,
                                                                                boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin())),
                                                                                _1
                                                                                )
                                                                    )
                                       );
    #else
                    return op->signals( boost::fusion::make_unfused_generic(boost::bind(&FusedMSignal<Signature>::invoke,
                                                                                boost::make_shared<FusedMSignal<Signature> >(func, SequenceFactory::assignable(args.begin())),
                                                                                _1
                                                                                )
                                                                    )
                                       );
    #endif
                }

                boost::shared_ptr<base::DisposableInterface> getLocalOperation() const {
                    return op->getImplementation();
                }
            };
    }
}

#endif
