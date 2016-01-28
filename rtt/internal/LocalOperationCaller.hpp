/***************************************************************************
  tag: FMTC  do nov 2 13:06:05 CET 2006  LocalOperationCaller.hpp

                        LocalOperationCaller.hpp -  description
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


#ifndef ORO_LOCAL_METHOD_HPP
#define ORO_LOCAL_METHOD_HPP

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>
#include "Invoker.hpp"
#include "../base/OperationCallerBase.hpp"
#include "../base/OperationBase.hpp"
#include "BindStorage.hpp"
#include "../SendStatus.hpp"
#include "../SendHandle.hpp"
#include "../ExecutionEngine.hpp"
#include "OperationCallerBinder.hpp"
#include <boost/fusion/include/vector_tie.hpp>
#include "../os/oro_allocator.hpp"

#include <iostream>
// For doing I/O
#include <boost/fusion/sequence/io.hpp>

namespace RTT
{
    namespace internal
    {
        /**
         * Implements call, send, collect, collectIfDone for all function arities.
         *
         * You can pass arguments by (const) value and by (const) reference. Passing
         * pointers is 'accidentally' supported in-process, it will not work with out-of-process
         * calls. The send takes all arguments (C++ doesn't know pure out args), the collect()
         * only takes the return value and the 'reference' arguments (if any)
         *
         * If this object is disposed, the collection of results is no longer possible
         * and this will generate a SendFailure at caller side.
         */
        template<class FunctionT>
        class LocalOperationCallerImpl
            : public base::OperationCallerBase<FunctionT>,
              public internal::CollectBase<FunctionT>,
              protected BindStorage<FunctionT>
        {
        public:
            LocalOperationCallerImpl() {}
            typedef FunctionT Signature;
            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef typename boost::function_traits<Signature>::result_type result_reference;
            typedef boost::function_traits<Signature> traits;

            typedef boost::shared_ptr<LocalOperationCallerImpl> shared_ptr;

            virtual bool ready() const {
                return true;
            }

            virtual bool isError() const {
              return this->retv.isError();
            }

            void executeAndDispose() {
                if (!this->retv.isExecuted()) {
                    this->exec(); // calls BindStorage.
                    //cout << "executed method"<<endl;
                    if(this->retv.isError())
                        this->reportError();
                    bool result = false;
                    if ( this->caller){
                        result = this->caller->process(this);
                    }
                    if (!result)
                        dispose();
                } else {
                    //cout << "received method done msg."<<endl;
                    // Already executed, are in caller.
                    // nop, we will check ret in collect()
                    // This is the place to call call-back functions,
                    // since we're in the caller's (or proxy's) EE.
                    dispose();
                }
                return;
            }

            /**
             * As long as dispose (or executeAndDispose() ) is
             * not called, this object will not be destroyed.
             */
            void dispose() {
                self.reset();
            }

            SendHandle<Signature> do_send(shared_ptr cl) {
                //std::cout << "Sending clone..."<<std::endl;
                ExecutionEngine* receiver = this->getMessageProcessor();
                cl->self = cl;
                if ( receiver && receiver->process( cl.get() ) ) {
                    return SendHandle<Signature>( cl );
                } else {
                    cl->dispose();
                    // cleanup. Done by shared_ptr.
                    return SendHandle<Signature>();
                }
            }
            // We need a handle object !
            SendHandle<Signature> send_impl() {
                return do_send( this->cloneRT() );
            }

            template<class T1>
            SendHandle<Signature> send_impl( T1 a1 ) {
                // bind types from Storage<Function>
                shared_ptr cl = this->cloneRT();
                cl->store( a1 );
                return do_send(cl);
            }

            template<class T1, class T2>
            SendHandle<Signature> send_impl( T1 a1, T2 a2 ) {
                // bind types from Storage<Function>
                shared_ptr cl = this->cloneRT();
                cl->store( a1,a2 );
                return do_send(cl);
            }

            template<class T1, class T2, class T3>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3 ) {
                // bind types from Storage<Function>
                shared_ptr cl = this->cloneRT();
                cl->store( a1,a2,a3 );
                return do_send(cl);
            }

            template<class T1, class T2, class T3, class T4>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                // bind types from Storage<Function>
                shared_ptr cl = this->cloneRT();
                cl->store( a1,a2,a3,a4 );
                return do_send(cl);
            }

            template<class T1, class T2, class T3, class T4, class T5>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5 ) {
                // bind types from Storage<Function>
                shared_ptr cl = this->cloneRT();
                cl->store( a1,a2,a3,a4,a5 );
                return do_send(cl);
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6 ) {
                // bind types from Storage<Function>
                shared_ptr cl = this->cloneRT();
                cl->store( a1,a2,a3,a4,a5,a6 );
                return do_send(cl);
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7 ) {
                // bind types from Storage<Function>
                shared_ptr cl = this->cloneRT();
                cl->store( a1,a2,a3,a4,a5,a6,a7 );
                return do_send(cl);
            }


            SendStatus collectIfDone_impl() const {
                if ( this->retv.isExecuted()) {
                    this->retv.checkError();
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            // collect_impl belongs in LocalOperationCallerImpl because it would need
            // to be repeated in each BindStorage spec.
            template<class T1>
            SendStatus collectIfDone_impl( T1& a1 ) const {
                if ( this->retv.isExecuted()) {
                    this->retv.checkError();
                    bf::vector_tie(a1) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2>
            SendStatus collectIfDone_impl( T1& a1, T2& a2 ) const {
                if ( this->retv.isExecuted()) {
                    this->retv.checkError();
                    bf::vector_tie(a1,a2) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                }
                return SendNotReady;
            }

            template<class T1, class T2, class T3>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3 ) const {
                if ( this->retv.isExecuted()) {
                    this->retv.checkError();
                    bf::vector_tie(a1,a2,a3) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4 ) const {
                if ( this->retv.isExecuted()) {
                    this->retv.checkError();
                    bf::vector_tie(a1,a2,a3,a4) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5 ) const {
                if ( this->retv.isExecuted()) {
                    this->retv.checkError();
                    bf::vector_tie(a1,a2,a3,a4,a5) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5, T6& a6 ) const {
                if ( this->retv.isExecuted()) {
                    this->retv.checkError();
                    bf::vector_tie(a1,a2,a3,a4,a5,a6) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5, T6& a6, T7& a7 ) const {
                if ( this->retv.isExecuted()) {
                    this->retv.checkError();
                    bf::vector_tie(a1,a2,a3,a4,a5,a6,a7) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            bool checkCaller() const {
                if (!this->caller) {
                    log(Error) << "You're using call() an OwnThread operation or collect() on a sent operation without setting a caller in the OperationCaller. This often causes deadlocks." <<endlog();
                    log(Error) << "Use this->engine() in a component or GlobalEngine::Instance() in a non-component function. Returning a CollectFailure." <<endlog();
                    assert( false && "You forgot to use setCaller(). See Orocos LOG messages for explanation.");
                    return false;
                }
                return true;
            }

            SendStatus collect_impl() const {
                if (!checkCaller()) return CollectFailure;
                this->caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retv)) );
                return this->collectIfDone_impl();
            }
            template<class T1>
            SendStatus collect_impl( T1& a1 ) const {
                if (!checkCaller()) return CollectFailure;
                this->caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retv)) );
                return this->collectIfDone_impl(a1);
            }

            template<class T1, class T2>
            SendStatus collect_impl( T1& a1, T2& a2 ) const {
                if (!checkCaller()) return CollectFailure;
                this->caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retv)) );
                return this->collectIfDone_impl(a1,a2);
            }

            template<class T1, class T2, class T3>
            SendStatus collect_impl( T1& a1, T2& a2, T3& a3 ) const {
                if (!checkCaller()) return CollectFailure;
                this->caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retv)) );
                return this->collectIfDone_impl(a1,a2,a3);
            }

	    template<class T1, class T2, class T3, class T4>
            SendStatus collect_impl( T1& a1, T2& a2, T3& a3, T4& a4) const {
                if (!checkCaller()) return CollectFailure;
                this->caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retv)) );
                return this->collectIfDone_impl(a1,a2,a3,a4);
            }

	    template<class T1, class T2, class T3, class T4, class T5>
	    SendStatus collect_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5) const {
                if (!checkCaller()) return CollectFailure;
                this->caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retv)) );
                return this->collectIfDone_impl(a1,a2,a3,a4, a5);
            }

	    template<class T1, class T2, class T3, class T4, class T5, class T6>
	    SendStatus collect_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5, T6& a6) const {
                if (!checkCaller()) return CollectFailure;
                this->caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retv)) );
                return this->collectIfDone_impl(a1,a2,a3,a4,a5,a6);
            }

	    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	    SendStatus collect_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5, T6& a6, T7& a7) const {
                if (!checkCaller()) return CollectFailure;
                this->caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retv)) );
                return this->collectIfDone_impl(a1,a2,a3,a4,a5,a6,a7);
            }

            /**
             * Invoke this operator if the method has no arguments.
             */
            template<class Xignored>
            result_type call_impl()
            {
                if ( this->isSend() ) {
                    SendHandle<Signature> h = send_impl();
                    if ( h.collect() == SendSuccess )
                        return h.ret();
                    else
                        throw SendFailure;
                } else {
#ifdef ORO_SIGNALLING_OPERATIONS
                    if (this->msig) this->msig->emit();
#endif
                    if ( this->mmeth )
                        return this->mmeth(); // ClientThread
                    else
                        return NA<result_type>::na();
                }
            }


            /**
             * Invoke this operator if the method has one argument.
             */
            template<class T1>
            result_type call_impl(T1 a1)
            {
                SendHandle<Signature> h;
                if ( this->isSend() ) {
                    h = send_impl<T1>(a1);
                    // collect_impl may take diff number of arguments than
                    // call_impl/ret_impl(), so we use generic collect() + ret_impl()
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1);
                    else
                        throw SendFailure;
                } else{
#ifdef ORO_SIGNALLING_OPERATIONS
                    if (this->msig) this->msig->emit(a1);
#endif
                    if ( this->mmeth )
                        return this->mmeth(a1);
                    else
                        return NA<result_type>::na();
                }
                return NA<result_type>::na();
            }

            template<class T1, class T2>
            result_type call_impl(T1 a1, T2 a2)
            {
                SendHandle<Signature> h;
                if ( this->isSend() ) {
                    h = send_impl<T1,T2>(a1,a2);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2);
                    else
                        throw SendFailure;
                } else {
#ifdef ORO_SIGNALLING_OPERATIONS
                    if (this->msig) this->msig->emit(a1,a2);
#endif
                    if ( this->mmeth )
                        return this->mmeth(a1,a2);
                    else
                        return NA<result_type>::na();
                }
                return NA<result_type>::na();
            }

            template<class T1, class T2, class T3>
            result_type call_impl(T1 a1, T2 a2, T3 a3)
            {
                SendHandle<Signature> h;
                if ( this->isSend() ) {
                    h = send_impl<T1,T2,T3>(a1,a2,a3);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3);
                    else
                        throw SendFailure;
                } else {
#ifdef ORO_SIGNALLING_OPERATIONS
                    if (this->msig) this->msig->emit(a1,a2,a3);
#endif
                    if ( this->mmeth )
                        return this->mmeth(a1,a2,a3);
                    else
                        return NA<result_type>::na();
                }
                return NA<result_type>::na();
            }

            template<class T1, class T2, class T3, class T4>
            result_type call_impl(T1 a1, T2 a2, T3 a3, T4 a4)
            {
                SendHandle<Signature> h;
                if ( this->isSend() ) {
                    h = send_impl<T1,T2,T3,T4>(a1,a2,a3,a4);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3,a4);
                    else
                        throw SendFailure;
                } else {
#ifdef ORO_SIGNALLING_OPERATIONS
                    if (this->msig) this->msig->emit(a1,a2,a3,a4);
#endif
                    if ( this->mmeth )
                        return this->mmeth(a1,a2,a3,a4);
                    else
                        return NA<result_type>::na();
                }
                return NA<result_type>::na();
            }

            template<class T1, class T2, class T3, class T4, class T5>
            result_type call_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
            {
                SendHandle<Signature> h;
                if (this->isSend()) {
                    h = send_impl<T1,T2,T3,T4,T5>(a1,a2,a3,a4,a5);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3,a4,a5);
                    else
                        throw SendFailure;
                } else {
#ifdef ORO_SIGNALLING_OPERATIONS
                    if (this->msig) this->msig->emit(a1,a2,a3,a4,a5);
#endif
                    if ( this->mmeth )
                        return this->mmeth(a1,a2,a3,a4,a5);
                    else
                        return NA<result_type>::na();
                }
                return NA<result_type>::na();
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            result_type call_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)
            {
                SendHandle<Signature> h;
                if (this->isSend()) {
                    h = send_impl<T1,T2,T3,T4,T5,T6>(a1,a2,a3,a4,a5,a6);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3,a4,a5,a6);
                    else
                        throw SendFailure;
                } else {
#ifdef ORO_SIGNALLING_OPERATIONS
                    if (this->msig) this->msig->emit(a1,a2,a3,a4,a5,a6);
#endif
                    if ( this->mmeth )
                        return this->mmeth(a1,a2,a3,a4,a5,a6);
                    else
                        return NA<result_type>::na();
                }
                return NA<result_type>::na();
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            result_type call_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)
            {
                SendHandle<Signature> h;
                if (this->isSend()) {
                    h = send_impl<T1,T2,T3,T4,T5,T6,T7>(a1,a2,a3,a4,a5,a6,a7);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3,a4,a5,a6,a7);
                    else
                        throw SendFailure;
                } else {
#ifdef ORO_SIGNALLING_OPERATIONS
                    if (this->msig) this->msig->emit(a1,a2,a3,a4,a5,a6,a7);
#endif
                    if ( this->mmeth )
                        return this->mmeth(a1,a2,a3,a4,a5,a6,a7);
                    else
                        return NA<result_type>::na();
                }
                return NA<result_type>::na();
            }

            result_type ret_impl() const
            {
                this->retv.checkError();
                return this->retv.result(); // may return void.
            }

            /**
             * This function has the same signature of call() and
             * returns the stored return value, and tries to return
             * all arguments.
             */
            template<class T1>
            result_type ret_impl(T1 a1) const
            {
                this->retv.checkError();
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1> vArgs( boost::ref(a1) );
                if ( this->retv.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retv.result(); // may return void.
            }

            template<class T1,class T2>
            result_type ret_impl(T1 a1, T2 a2) const
            {
                this->retv.checkError();
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2> vArgs( boost::ref(a1), boost::ref(a2) );
                if ( this->retv.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg< boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retv.result(); // may return void.
            }

            template<class T1,class T2, class T3>
            result_type ret_impl(T1 a1, T2 a2, T3 a3) const
            {
                this->retv.checkError();
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3) );
                if ( this->retv.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retv.result(); // may return void.
            }

            template<class T1,class T2, class T3, class T4>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4) const
            {
                this->retv.checkError();
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4) );
                if ( this->retv.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retv.result(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) const
            {
                this->retv.checkError();
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4,T5> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4), boost::ref(a5) );
                if ( this->retv.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retv.result(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5, class T6>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) const
            {
                this->retv.checkError();
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4,T5,T6> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4), boost::ref(a5), boost::ref(a6) );
                if ( this->retv.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retv.result(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5, class T6, class T7>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) const
            {
                this->retv.checkError();
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4,T5,T6,T7> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4), boost::ref(a5), boost::ref(a6), boost::ref(a7) );
                if ( this->retv.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retv.result(); // may return void.
            }

            virtual shared_ptr cloneRT() const = 0;
        protected:
            typedef BindStorage<FunctionT> Store;
            /**
             * Used to refcount self as long as dispose() is not called.
             * This refcount is real-time since both shared_ptr and object
             * were allocated with the rt_allocator class.
             */
            typename base::OperationCallerBase<FunctionT>::shared_ptr self;
        };

        /**
         * A method which executes a local function.
         *
         * Usage:
         @code
         LocalOperationCaller<double(int, double)> mymeth( &Class::foo, &c);
         double result = mymeth( 3, 1.9);
         @endcode
        */
        template<class FunctionT>
        struct LocalOperationCaller
            : public Invoker<FunctionT,LocalOperationCallerImpl<FunctionT> >
        {
            typedef FunctionT Signature;
            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef boost::function_traits<Signature> traits;

            typedef boost::shared_ptr<LocalOperationCaller> shared_ptr;

            /**
             * Create an empty LocalOperationCaller object.
             * Use assignment to initialise it.
             * @see method
             */
            LocalOperationCaller()
            {}

            /**
             * Construct a LocalOperationCaller from a class member pointer and an
             * object of that class.
             *
             * @param meth A pointer to a class member function
             * @param object An object of the class which has \a meth as member function.
             * @param ee The executing engine. This is the owner engine or the GlobalEngine. If null, will be the GlobalEngine.
             * @param caller The caller engine. From which component we call this Operation.
             */
            template<class M, class ObjectType>
            LocalOperationCaller(M meth, ObjectType object, ExecutionEngine* ee, ExecutionEngine* caller, ExecutionThread et = ClientThread)
            {
                this->setCaller( caller );
                this->setOwner(ee );
                this->setThread( et, ee );
                this->mmeth = OperationCallerBinder<Signature>()(meth, object);
            }

            /**
             * Construct a LocalOperationCaller from a function pointer or function object.
             *
             * @param meth an pointer to a function or function object.
             */
            template<class M>
            LocalOperationCaller(M meth, ExecutionEngine* ee, ExecutionEngine* caller, ExecutionThread et = ClientThread )
            {
                this->setCaller( caller );
                this->setOwner( ee );
                this->setThread( et, ee );
                this->mmeth = meth;
            }

            boost::function<Signature> getOperationCallerFunction() const
            {
                return this->mmeth;
            }

#ifdef ORO_SIGNALLING_OPERATIONS
            void setSignal(typename Signal<Signature>::shared_ptr sig) {
                this->msig = sig;
            }
#endif
            base::OperationCallerBase<Signature>* cloneI(ExecutionEngine* caller) const
            {
                LocalOperationCaller<Signature>* ret = new LocalOperationCaller<Signature>(*this);
                ret->setCaller( caller ); // mandatory !
                return ret;
            }

            typename LocalOperationCallerImpl<Signature>::shared_ptr cloneRT() const
            {
                // returns identical copy of this;
                return boost::allocate_shared<LocalOperationCaller<Signature> >(os::rt_allocator<LocalOperationCaller<Signature> >(), *this);
            }
        };
    }
}

#endif
