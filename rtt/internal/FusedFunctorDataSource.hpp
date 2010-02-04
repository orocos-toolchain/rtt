#ifndef ORO_FUSEDFUNCTORDATASOURCE_HPP_
#define ORO_FUSEDFUNCTORDATASOURCE_HPP_

#include "DataSource.hpp"
#include "../base/MethodBase.hpp"
#include "CreateSequence.hpp"
#include "BindStorage.hpp"
#include "UnMember.hpp"
#include <boost/bind.hpp>
#include <boost/type_traits.hpp>
#include <boost/function.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/invoke_procedure.hpp>

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
        template<typename Signature>
        struct FusedFunctorDataSource
        : public DataSource<
                  typename boost::function_traits<Signature>::result_type>
          {
              typedef typename boost::function_traits<Signature>::result_type
                      result_type;
              typedef result_type value_t;
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

              value_t get() const
              {
                  // forward invoke to ret object, which stores return value.
                  // this foo pointer dance is because older compilers don't handle using
                  // &bf::invoke<call_type,arg_type> directly.
                  typedef typename bf::result_of::invoke<call_type,arg_type>::type iret;
                  typedef iret(*IType)(call_type, arg_type const&);
                  IType foo = &bf::invoke<call_type,arg_type>;
                  ret.exec( boost::bind(foo, boost::ref(ff), SequenceFactory::data(args)));
                  SequenceFactory::update(args);
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

        /**
         * A DataSource that calls a method which gets its arguments from other
         * data sources. The result type of this data source is the result type
         * of the called function.
         */
        template<typename Signature>
        struct FusedMCallDataSource
        : public DataSource<
                  typename boost::function_traits<Signature>::result_type>
          {
              typedef typename boost::function_traits<Signature>::result_type
                      result_type;
              typedef result_type value_t;
              typedef create_sequence<
                      typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              typename base::MethodBase<Signature>::shared_ptr ff;
              DataSourceSequence args;
              mutable RStore<result_type> ret;
          public:
              typedef boost::intrusive_ptr<FusedMCallDataSource<Signature> >
                      shared_ptr;

              FusedMCallDataSource(typename base::MethodBase<Signature>::shared_ptr g,
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

              value_t get() const
              {
                  // put the member's object as first since SequenceFactory does not know about the MethodBase type.
                  typedef bf::cons<base::MethodBase<Signature>*, typename SequenceFactory::data_type> arg_type;
                  typedef typename AddMember<Signature,base::MethodBase<Signature>* >::type call_type;
                  // this foo pointer dance is because older compilers don't handle using
                  // &bf::invoke<call_type,arg_type> directly.
                  typedef typename bf::result_of::invoke<call_type,arg_type>::type iret;
                  typedef iret(*IType)(call_type, arg_type const&);
                  IType foo = &bf::invoke<call_type,arg_type>;
                  // we need to store the ret value ourselves.
                  ret.exec( boost::bind(foo, &base::MethodBase<Signature>::call, arg_type(ff.get(), SequenceFactory::data(args))) );
                  SequenceFactory::update(args);
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
              typedef create_sequence<
                      typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              typename base::MethodBase<Signature>::shared_ptr ff;
              DataSourceSequence args;
              mutable SendHandle<Signature> sh; // mutable because of get() const
          public:
              typedef boost::intrusive_ptr<FusedMSendDataSource<Signature> >
                      shared_ptr;

              FusedMSendDataSource(typename base::MethodBase<Signature>::shared_ptr g,
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

              value_t get() const
              {
                  // put the member's object as first since SequenceFactory does not know about the MethodBase type.
                  sh = bf::invoke(&base::MethodBase<Signature>::send, bf::cons<base::MethodBase<Signature>*, typename SequenceFactory::data_type>(ff.get(), SequenceFactory::data(args)));
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
              // push the SendHandle pointer in front.
              typedef typename CollectType<Signature>::type CollectSignature;
              typedef typename boost::function_types::parameter_types<CollectSignature>::type arg_types;
              typedef typename mpl::push_front<arg_types,typename boost::add_reference<SendHandle<Signature> >::type >::type handle_and_arg_types;
              typedef create_sequence< handle_and_arg_types// @todo XXX use &SendHandle to avoid a copy ???
                      > SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              DataSourceSequence args;
              bool isblocking;
              mutable SendStatus ss; // because of get() const
          public:
              typedef boost::intrusive_ptr<FusedMCollectDataSource<Signature> >
                      shared_ptr;

              FusedMCollectDataSource(
                                     const DataSourceSequence& s = DataSourceSequence(), bool blocking = true ) :
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

              value_t get() const
              {
                  // put the member's object as first since SequenceFactory does not know about the MethodBase type.
                  if (isblocking)
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
    }
}

#endif /* ORO_FUSEDFUNCTORDATASOURCE_HPP_ */
