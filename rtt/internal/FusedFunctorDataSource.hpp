/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  FusedFunctorDataSource.hpp

                        FusedFunctorDataSource.hpp -  description
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


#ifndef ORO_FUSEDFUNCTORDATASOURCE_HPP_
#define ORO_FUSEDFUNCTORDATASOURCE_HPP_

#include "DataSource.hpp"
#include "CreateSequence.hpp"
#include "../SendStatus.hpp"
#include "BindStorage.hpp"
#include "../ExecutionEngine.hpp"
#include "../os/oro_allocator.hpp"
#include "UnMember.hpp"
#include <boost/bind.hpp>
#include <boost/type_traits.hpp>
#include <boost/function.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/invoke_procedure.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/make_shared.hpp>

#include <iostream>
using namespace std;

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
                  if(ret.isError()) {
                    ff->reportError();
                    ret.checkError();
                  }
                  SequenceFactory::update(args);
                  return true;
              }

              value_t get() const
              {
                  evaluate();
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
        : public AssignableDataSource<SendHandle<Signature> >
          {
              typedef SendHandle<Signature> result_type;
              typedef result_type value_t;
              typedef typename AssignableDataSource<value_t>::param_t param_t;
              typedef typename AssignableDataSource<value_t>::const_reference_t const_reference_t;
              typedef typename AssignableDataSource<value_t>::reference_t reference_t;
              typedef create_sequence<
                      typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
              typedef typename SequenceFactory::type DataSourceSequence;
              typename base::OperationCallerBase<Signature>::shared_ptr ff;
              DataSourceSequence args;
              mutable SendHandle<Signature> sh; // mutable because of get() const
              mutable bool isqueued;
          public:
              typedef boost::intrusive_ptr<FusedMSendDataSource<Signature> >
                      shared_ptr;

              FusedMSendDataSource(typename base::OperationCallerBase<Signature>::shared_ptr g,
                                     const DataSourceSequence& s = DataSourceSequence() ) :
                  ff(g), args(s), sh(), isqueued(false)
              {
              }

              void setArguments(const DataSourceSequence& a1)
              {
                  args = a1;
              }

              virtual void set( param_t t ) {
                  sh = t;
              }

              reference_t set() {
                  return sh;
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
                  if (isqueued)
                      return sh;
                  // put the member's object as first since SequenceFactory does not know about the OperationCallerBase type.
                  sh = bf::invoke(&base::OperationCallerBase<Signature>::send, bf::cons<base::OperationCallerBase<Signature>*, typename SequenceFactory::data_type>(ff.get(), SequenceFactory::data(args)));
                  if ( sh.ready() ) // only queued if sh contains a collectable operation
                      isqueued = true;
                  return sh;
              }

              void reset() {
                  isqueued = false;
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
                  // we need copy semantics because FusedMCollectDataSource tracks us.
                  if ( alreadyCloned[this] != 0 ) {
                      assert( dynamic_cast<FusedMSendDataSource<Signature>*>( alreadyCloned[this] ) == static_cast<FusedMSendDataSource<Signature>*>( alreadyCloned[this] ) );
                      return static_cast<FusedMSendDataSource<Signature>*>( alreadyCloned[this] );
                  }
                  // Other pieces in the code rely on insertion in the map :
                  alreadyCloned[this] = new FusedMSendDataSource<Signature>(ff, SequenceFactory::copy(args, alreadyCloned));
                  // return copy
                  return static_cast<FusedMSendDataSource<Signature>*>( alreadyCloned[this] );
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
                  // we need copy semantics because CmdCollectCondition tracks us.
                  // WARNING: This is a tricky precedent... should all DataSources with state + multiple living references then implement this ? Should we assert on this ?
                  if ( alreadyCloned[this] != 0 ) {
                      assert ( dynamic_cast<FusedMCollectDataSource<Signature>*>( alreadyCloned[this] ) == static_cast<FusedMCollectDataSource<Signature>*>( alreadyCloned[this] ) );
                      return static_cast<FusedMCollectDataSource<Signature>*>( alreadyCloned[this] );
                  }
                  // Other pieces in the code rely on insertion in the map :
                  alreadyCloned[this] = new FusedMCollectDataSource<Signature>(SequenceFactory::copy(args, alreadyCloned), isblocking);
                  // return copy
                  return static_cast<FusedMCollectDataSource<Signature>*>( alreadyCloned[this] );
              }
          };

        /**
         * A Function object that reacts to a Signal by writing the arguments in
         * data sources and calling an action object.
         *
         * Implementation note: this class does not require copy/clone semantics because
         * it is re-created for each SM instantiation, so it already gets the copy/cloned
         * Data Sources in its constructor.
         */
        template<typename Signature>
        struct FusedMSignal : public base::DisposableInterface
        {
            typedef typename boost::function_traits<Signature>::result_type
                    result_type;
            typedef result_type value_t;
            typedef create_sequence<
                    typename boost::function_types::parameter_types<Signature>::type> SequenceFactory;
            typedef typename SequenceFactory::atype DataSourceSequence;
            boost::shared_ptr<base::ActionInterface> mact;
            // We need the arg_cache to store data similar to BindStorage,
            // such that we can safely access it during execute().
            typename SequenceFactory::data_store_type arg_cache;
            DataSourceSequence args;
            ExecutionEngine* subscriber;
            /**
             * Used to refcount self as long as dispose() is not called.
             * This refcount is real-time since both shared_ptr and object
             * were allocated with the rt_allocator class.
             */
            boost::shared_ptr<FusedMSignal<Signature> > self;
        public:
            typedef boost::shared_ptr<FusedMSignal<Signature> > shared_ptr;

            /**
             * Create a new object which takes ownership of the action to be executed upon signal.
             * @param act The action to be executed upon invoke()
             * @param s   The data  sources we have to fill in before executing the action
             * @param subscr The ExecutionEngine that will execute the action in it's thread.
             * If null is given, the action is executed synchronously within invoke()
             */
            FusedMSignal(base::ActionInterface* act,
                         const DataSourceSequence& s, 
                         ExecutionEngine* subscr ) :
                mact(act), args(s), subscriber(subscr), self()
            {
            }

            ~FusedMSignal() {
            }

            /**
             * A Fused function that takes the arguments of the signal,
             * puts them into the assignable data sources and
             * executes the associated action.
             */
            result_type invoke(const typename SequenceFactory::data_type& seq) const {
                if ( subscriber ) {
                    // asynchronous
                    shared_ptr sg = this->cloneRT();
                    sg->arg_cache = SequenceFactory::store(seq);
                    sg->self = sg;
                    if ( subscriber->process( sg.get() ) ) {
                        // all ok
                    } else {
                        sg->dispose();
                    }
                } else {
                    // synchronous
                    SequenceFactory::set( seq, args );
                    mact->execute();
                }

                return NA<result_type>::na();
            }

            void executeAndDispose() {
                SequenceFactory::load( this->arg_cache, this->args );
                mact->execute();
                dispose();
            }

            /**
             * As long as dispose (or executeAndDispose() ) is
             * not called, this object will not be destroyed.
             */
            void dispose() {
                self.reset();
            }


            void setArguments(const DataSourceSequence& a1)
            {
                args = a1;
            }

            typename FusedMSignal<Signature>::shared_ptr cloneRT() const
            {
                // returns identical copy of this;
                return boost::allocate_shared<FusedMSignal<Signature> >(os::rt_allocator<FusedMSignal<Signature> >(), *this);
            }
        };

    }
}

#endif /* ORO_FUSEDFUNCTORDATASOURCE_HPP_ */
