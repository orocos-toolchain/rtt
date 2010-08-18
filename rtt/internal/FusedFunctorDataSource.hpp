#ifndef ORO_FUSEDFUNCTORDATASOURCE_HPP_
#define ORO_FUSEDFUNCTORDATASOURCE_HPP_

#include "DataSource.hpp"
#include "CreateSequence.hpp"
#include "../SendStatus.hpp"
#include "BindStorage.hpp"
#include <boost/bind.hpp>
#include <boost/type_traits.hpp>
#include <boost/function.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/invoke_procedure.hpp>
#include <boost/mpl/bool.hpp>

namespace RTT
{
    namespace internal
    {
        namespace bf = boost::fusion;

        /**
         * A DataSource that calls a functor of signature \a Signature which gets its arguments from other
         * data sources. The result type of this data source is the result type
         * of the called function.
         */
        template<typename Signature, class Enable=void>
        struct FusedFunctorDataSource
        : public DataSource<
                  typename remove_cr<typename boost::function_traits<Signature>::result_type>::type >
          {
              //BOOST_STATIC_ASSERT( boost::mpl::false_::value );
              typedef typename boost::function_traits<Signature>::result_type
                      result_type;
              typedef typename remove_cr<result_type>::type value_t;
              typedef typename DataSource<value_t>::const_reference_t const_reference_t;
              typedef create_sequence<
                      typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              typedef boost::function<Signature> call_type;
              typedef typename SequenceFactory::data_type arg_type;
              boost::function<Signature> ff;
              DataSourceSequence args;
              mutable RStore<result_type> ret;
          public:
              typedef boost::intrusive_ptr<FusedFunctorDataSource<Signature> >
                      shared_ptr;

              template<class Func>
              FusedFunctorDataSource(Func g,
                                     const DataSourceSequence& s = DataSourceSequence() ) :
                  ff(g), args(s)
              {
              }

              void setArguments(const DataSourceSequence& a1)
              {
                  args = a1;
              }

              value_t value() const
              {
                  return ret.result();
              }

              const_reference_t rvalue() const
              {
                  return ret.result();
              }

              bool evaluate() const {
                  // forward invoke to ret object, which stores return value.
                  // this foo pointer dance is because older compilers don't handle using
                  // &bf::invoke<call_type,arg_type> directly.
                  typedef typename bf::result_of::invoke<call_type,arg_type>::type iret;
                  typedef iret(*IType)(call_type, arg_type const&);
                  IType foo = &bf::invoke<call_type,arg_type>;
                  ret.exec( boost::bind(foo, boost::ref(ff), SequenceFactory::data(args)));
                  SequenceFactory::update(args);
                  return true;
              }

              value_t get() const
              {
                  FusedFunctorDataSource<Signature>::evaluate();
                  return ret.result();
              }

              virtual FusedFunctorDataSource<Signature>* clone() const
              {
                  return new FusedFunctorDataSource<Signature> (ff, args);
              }
              virtual FusedFunctorDataSource<Signature>* copy(
                                                          std::map<
                                                                  const base::DataSourceBase*,
                                                                  base::DataSourceBase*>& alreadyCloned) const
              {
                  return new FusedFunctorDataSource<Signature> (ff, SequenceFactory::copy(args, alreadyCloned));
              }
          };

        template<typename Signature>
        struct FusedFunctorDataSource<Signature, typename boost::enable_if< is_pure_reference<typename boost::function_traits<Signature>::result_type> >::type >
        : public AssignableDataSource<
                  typename remove_cr<typename boost::function_traits<Signature>::result_type>::type >
          {
            //BOOST_STATIC_ASSERT( boost::mpl::false_::value );
              typedef typename boost::function_traits<Signature>::result_type
                      result_type;
              typedef typename remove_cr<result_type>::type value_t;
              typedef typename DataSource<value_t>::const_reference_t const_reference_t;
              typedef typename AssignableDataSource<value_t>::reference_t reference_t;
              typedef create_sequence<
                      typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              typedef boost::function<Signature> call_type;
              typedef typename SequenceFactory::data_type arg_type;
              boost::function<Signature> ff;
              DataSourceSequence args;
              mutable RStore<result_type> ret;
          public:
              typedef boost::intrusive_ptr<FusedFunctorDataSource<Signature> >
                      shared_ptr;

              template<class Func>
              FusedFunctorDataSource(Func g,
                                     const DataSourceSequence& s = DataSourceSequence() ) :
                  ff(g), args(s)
              {
              }

              void setArguments(const DataSourceSequence& a1)
              {
                  args = a1;
              }

              value_t value() const
              {
                  return ret.result();
              }

              const_reference_t rvalue() const
              {
                  return ret.result();
              }

              bool evaluate() const {
                  // forward invoke to ret object, which stores return value.
                  // this foo pointer dance is because older compilers don't handle using
                  // &bf::invoke<call_type,arg_type> directly.
                  typedef typename bf::result_of::invoke<call_type,arg_type>::type iret;
                  typedef iret(*IType)(call_type, arg_type const&);
                  IType foo = &bf::invoke<call_type,arg_type>;
                  ret.exec( boost::bind(foo, boost::ref(ff), SequenceFactory::data(args)));
                  SequenceFactory::update(args);
                  return true;
              }
              value_t get() const
              {
                  FusedFunctorDataSource<Signature>::evaluate();
                  return ret.result();
              }

              void set( typename AssignableDataSource<value_t>::param_t arg) {
                  // we need to get the new reference before we set the arg.
                  get(); ret.result() = arg;
              }

              reference_t set() {
                  get(); return ret.result();
              }

              virtual FusedFunctorDataSource<Signature>* clone() const
              {
                  return new FusedFunctorDataSource<Signature> (ff, args);
              }
              virtual FusedFunctorDataSource<Signature>* copy(
                                                          std::map<
                                                                  const base::DataSourceBase*,
                                                                  base::DataSourceBase*>& alreadyCloned) const
              {
                  return new FusedFunctorDataSource<Signature> (ff, SequenceFactory::copy(args, alreadyCloned));
              }
          };
        /**
         * Creates a data source that returns the result of a given function.
         * Use GenerateDataSource to create the arguments args from given data sources.
         * @param f The function to embed in a data source. The return type of this function
         * will be the return type of the created data source.
         * @param args The arguments given to the function \a f
         * @return A new data source.
         * @throw wrong_number_of_args_exception when args.size() is not equal to the arity of \a f.
         * @throw wrong_types_of_args_exception when one or more types in args do not match the expected types in f.
         */
        template<class Function>
        base::DataSourceBase* newFunctorDataSource(Function f, const std::vector<base::DataSourceBase::shared_ptr>& args)
        {
            typedef typename boost::function_types::function_type<Function>::type Signature;
            typedef internal::create_sequence<typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
            if ( args.size() != boost::function_traits<Signature>::arity )
                throw wrong_number_of_args_exception(boost::function_traits<Signature>::arity, args.size() );
            return new FusedFunctorDataSource<Signature>(f, SequenceFactory::sources(args.begin()));
        }

        /**
         * A DataSource that calls a method which gets its arguments from other
         * data sources. The result type of this data source is the result type
         * of the called function.
         */
        template<typename Signature>
        struct FusedMCallDataSource
        : public DataSource<
              typename remove_cr<typename boost::function_traits<Signature>::result_type>::type >
        {
              typedef typename boost::function_traits<Signature>::result_type
                      result_type;
              typedef typename remove_cr<result_type>::type value_t;
              typedef typename DataSource<value_t>::const_reference_t const_reference_t;
              typedef create_sequence<
                      typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              typename base::OperationCallerBase<Signature>::shared_ptr ff;
              DataSourceSequence args;
              mutable RStore<result_type> ret;
          public:
              typedef boost::intrusive_ptr<FusedMCallDataSource<Signature> >
                      shared_ptr;

              FusedMCallDataSource(typename base::OperationCallerBase<Signature>::shared_ptr g,
                                     const DataSourceSequence& s = DataSourceSequence() ) :
                  ff(g), args(s)
              {
              }

              void setArguments(const DataSourceSequence& a1)
              {
                  args = a1;
              }

              value_t value() const
              {
                  return ret.result();
              }

              const_reference_t rvalue() const
              {
                  return ret.result();
              }

              bool evaluate() const {
                  // put the member's object as first since SequenceFactory does not know about the OperationCallerBase type.
                  typedef bf::cons<base::OperationCallerBase<Signature>*, typename SequenceFactory::data_type> arg_type;
                  typedef typename AddMember<Signature,base::OperationCallerBase<Signature>* >::type call_type;
                  // this foo pointer dance is because older compilers don't handle using
                  // &bf::invoke<call_type,arg_type> directly.
                  typedef typename bf::result_of::invoke<call_type,arg_type>::type iret;
                  typedef iret(*IType)(call_type, arg_type const&);
                  IType foo = &bf::invoke<call_type,arg_type>;
                  // we need to store the ret value ourselves.
                  ret.exec( boost::bind(foo, &base::OperationCallerBase<Signature>::call, arg_type(ff.get(), SequenceFactory::data(args))) );
                  SequenceFactory::update(args);
                  return true;
              }

              value_t get() const
              {
                  FusedMCallDataSource<Signature>::evaluate();
                  return ret.result();
              }

              virtual FusedMCallDataSource<Signature>* clone() const
              {
                  return new FusedMCallDataSource<Signature> (ff, args);
              }
              virtual FusedMCallDataSource<Signature>* copy(
                                                          std::map<
                                                                  const base::DataSourceBase*,
                                                                  base::DataSourceBase*>& alreadyCloned) const
              {
                  return new FusedMCallDataSource<Signature> (ff, SequenceFactory::copy(args, alreadyCloned));
              }
          };

        /**
         * A DataSource that sends a method which gets its arguments from other
         * data sources. The result type of this data source is a SendHandle.
         */
        template<typename Signature>
        struct FusedMSendDataSource
        : public DataSource<SendHandle<Signature> >
          {
              typedef SendHandle<Signature> result_type;
              typedef result_type value_t;
              typedef typename DataSource<value_t>::const_reference_t const_reference_t;
              typedef create_sequence<
                      typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              typename base::OperationCallerBase<Signature>::shared_ptr ff;
              DataSourceSequence args;
              mutable SendHandle<Signature> sh; // mutable because of get() const
          public:
              typedef boost::intrusive_ptr<FusedMSendDataSource<Signature> >
                      shared_ptr;

              FusedMSendDataSource(typename base::OperationCallerBase<Signature>::shared_ptr g,
                                     const DataSourceSequence& s = DataSourceSequence() ) :
                  ff(g), args(s)
              {
              }

              void setArguments(const DataSourceSequence& a1)
              {
                  args = a1;
              }

              value_t value() const
              {
                  return sh;
              }

              const_reference_t rvalue() const
              {
                  return sh;
              }

              value_t get() const
              {
                  // put the member's object as first since SequenceFactory does not know about the OperationCallerBase type.
                  sh = bf::invoke(&base::OperationCallerBase<Signature>::send, bf::cons<base::OperationCallerBase<Signature>*, typename SequenceFactory::data_type>(ff.get(), SequenceFactory::data(args)));
                  return sh;
              }

              virtual FusedMSendDataSource<Signature>* clone() const
              {
                  return new FusedMSendDataSource<Signature> (ff, args);
              }
              virtual FusedMSendDataSource<Signature>* copy(
                                                          std::map<
                                                                  const base::DataSourceBase*,
                                                                  base::DataSourceBase*>& alreadyCloned) const
              {
                  return new FusedMSendDataSource<Signature> (ff, SequenceFactory::copy(args, alreadyCloned));
              }
          };

        /**
         * A DataSource that collects the result of an asynchronous method which store its results in other
         * data sources. The result type of this data source is a SendStatus.
         * @param Signature is the signature of the collect function, not of the
         * original send function.
         */
        template<typename Signature>
        struct FusedMCollectDataSource
        : public DataSource<SendStatus>
          {
              typedef SendStatus result_type;
              typedef result_type value_t;
              typedef DataSource<SendStatus>::const_reference_t const_reference_t;
              // push the SendHandle pointer in front.
              typedef typename CollectType<Signature>::type CollectSignature;
              typedef typename boost::function_types::parameter_types<CollectSignature>::type arg_types;
              typedef typename mpl::push_front<arg_types, SendHandle<Signature>& >::type handle_and_arg_types;
              typedef create_sequence< handle_and_arg_types
                      > SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              DataSourceSequence args;
              DataSource<bool>::shared_ptr isblocking;
              mutable SendStatus ss; // because of get() const
          public:
              typedef boost::intrusive_ptr<FusedMCollectDataSource<Signature> >
                      shared_ptr;

              FusedMCollectDataSource(
                                     const DataSourceSequence& s, DataSource<bool>::shared_ptr blocking ) :
                  args(s), isblocking(blocking), ss(SendFailure)
              {
              }

              void setArguments(const DataSourceSequence& a1)
              {
                  args = a1;
              }

              value_t value() const
              {
                  return ss;
              }

              const_reference_t rvalue() const
              {
                  return ss;
              }

              value_t get() const
              {
                  // put the member's object as first since SequenceFactory does not know about the OperationCallerBase type.
                  if (isblocking->get())
                      ss = bf::invoke(&SendHandle<Signature>::CBase::collect, SequenceFactory::data(args));
                  else
                      ss = bf::invoke(&SendHandle<Signature>::CBase::collectIfDone, SequenceFactory::data(args));
                  SequenceFactory::update(args);
                  return ss;
              }

              virtual FusedMCollectDataSource<Signature>* clone() const
              {
                  return new FusedMCollectDataSource<Signature> ( args, isblocking);
              }
              virtual FusedMCollectDataSource<Signature>* copy(
                                                          std::map<
                                                                  const base::DataSourceBase*,
                                                                  base::DataSourceBase*>& alreadyCloned) const
              {
                  return new FusedMCollectDataSource<Signature> ( SequenceFactory::copy(args, alreadyCloned), isblocking);
              }
          };

        /**
         * A Factory that reacts to a Signal by writing the arguments in
         * data sources and calling an action.
         */
        template<typename Signature>
        struct FusedMSignal
        {
            typedef typename boost::function_traits<Signature>::result_type
                    result_type;
            typedef result_type value_t;
            typedef create_sequence<
                    typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
            typedef typename SequenceFactory::atype DataSourceSequence;
            base::ActionInterface* mact;
            DataSourceSequence args;
        public:
              typedef boost::shared_ptr<FusedMSignal<Signature> >
                      shared_ptr;

              FusedMSignal(base::ActionInterface* act,
                           const DataSourceSequence& s = DataSourceSequence() ) :
                  mact(act), args(s)
              {
              }

              /**
               * A Fused function that takes the arguments of the signal,
               * puts them into the assignable data sources and
               * executes the associated action.
               */
              result_type invoke(typename SequenceFactory::data_type seq) {
                  SequenceFactory::set( seq, args );
                  mact->execute();
                  return NA<result_type>::na();
              }

              void setArguments(const DataSourceSequence& a1)
              {
                  args = a1;
              }
          };

    }
}

#endif /* ORO_FUSEDFUNCTORDATASOURCE_HPP_ */
