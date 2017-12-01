/***************************************************************************
  tag: FMTC  do nov 2 13:06:09 CET 2006  RemoteOperationCaller.hpp

                        RemoteOperationCaller.hpp -  description
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


#ifndef ORO_REMOTE_METHOD_HPP
#define ORO_REMOTE_METHOD_HPP

#include <boost/function.hpp>
#include <string>
#include "../base/OperationCallerBase.hpp"
#include "OperationCallerC.hpp"
#include "DataSourceStorage.hpp"
#include "Invoker.hpp"

#include <boost/fusion/include/vector_tie.hpp>
#include <boost/fusion/include/filter_if.hpp>
#include <boost/fusion/include/as_vector.hpp>



namespace RTT
{
    namespace internal
    {
        namespace bf = ::boost::fusion;

        /**
         * A OperationCaller implementation which executes a remote function
         * which is encapsulated in a OperationCallerC and SendHandleC object.
         * Used as implementation class for RemoteOperationCaller.
         *
         * @param OperationCallerT The function signature of the method. For example,
         * bool( int, Frame, double)
         *
         */
        template<class OperationCallerT>
        class RemoteOperationCallerImpl
            : public base::OperationCallerBase<OperationCallerT>,
              public internal::CollectBase<OperationCallerT>
        {
        protected:
            OperationCallerC mmeth;
            SendHandleC mhandle;
            DataSourceStorage<OperationCallerT> sendargs;
            DataSourceStorage<typename CollectType<OperationCallerT>::type > collectargs;
        public:
            typedef OperationCallerT Signature;
            typedef typename boost::function_traits<OperationCallerT>::result_type result_type;

            /**
             * The defaults are reset by the constructor.
             */
            RemoteOperationCallerImpl()
                : mmeth(), mhandle()
            {}

            /**
             * Create from a sendhandle.
             */
            RemoteOperationCallerImpl(SendHandleC handle)
                : mmeth(), mhandle(handle)
            {}

            virtual void executeAndDispose() { assert(false); }
            virtual bool isError() const { return false; }
            virtual void dispose() { assert(false); }

            /**
             * Call this operator if the RemoteOperationCaller takes no arguments.
             *
             * @return true if ready and succesfully sent.
             */
            template<class Xignored>
            result_type call_impl() {
                mmeth.call();
                return sendargs.getResult();
            }

            template<class T1>
            result_type call_impl( T1 a1 ) {
                sendargs.store( a1 );
                mmeth.call();
                return sendargs.getResult();
            }

            template<class T1, class T2>
            result_type call_impl( T1 a1, T2 a2 ) {
                sendargs.store( a1, a2 );
                mmeth.call();
                return sendargs.getResult();
            }

            template<class T1, class T2, class T3>
            result_type call_impl( T1 a1, T2 a2, T3 a3 ) {
                sendargs.store( a1, a2, a3 );
                mmeth.call();
                return sendargs.getResult();
            }

            template<class T1, class T2, class T3, class T4>
            result_type call_impl( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                sendargs.store( a1, a2, a3, a4 );
                mmeth.call();
                return sendargs.getResult();
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            result_type call_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6 ) {
                sendargs.store( a1, a2, a3, a4, a5, a6 );
                mmeth.call();
                return sendargs.getResult();
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            result_type call_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7 ) {
                sendargs.store( a1, a2, a3, a4, a5, a6, a7 );
                mmeth.call();
                return sendargs.getResult();
            }

            template<class T1, class T2, class T3, class T4, class T5>
            result_type call_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5 ) {
                sendargs.store( a1, a2, a3, a4, a5 );
                mmeth.call();
                return sendargs.getResult();
            }

            SendHandle<Signature> send_impl() {
                mhandle = mmeth.send();
                // @todo: get remote collect from rt allocation.
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1>
            SendHandle<Signature> send_impl( T1 a1 ) {
                sendargs.store( a1 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2>
            SendHandle<Signature> send_impl( T1 a1, T2 a2 ) {
                sendargs.store( a1, a2 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3 ) {
                sendargs.store( a1, a2, a3 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3, class T4>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                sendargs.store( a1, a2, a3, a4 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3, class T4, class T5>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5 ) {
                sendargs.store( a1, a2, a3, a4, a5 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6 ) {
                sendargs.store( a1, a2, a3, a4, a5, a6 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7 ) {
                sendargs.store( a1, a2, a3, a4, a5, a6, a7 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            SendStatus collectIfDone_impl() {
                if ( mhandle.collectIfDone() == SendSuccess ) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1>
            SendStatus collectIfDone_impl( T1& a1 ) {
                collectargs.store( a1 );
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2>
            SendStatus collectIfDone_impl( T1& a1, T2& a2 ) {
                collectargs.store( a1, a2);
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    return SendSuccess;
                }
                return SendNotReady;
            }

            template<class T1, class T2, class T3>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3 ) {
                collectargs.store( a1, a2, a3);
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4 ) {
                collectargs.store( a1, a2, a3, a4);
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5, T6& a6 ) {
                collectargs.store( a1, a2, a3, a4, a5, a6);
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5, T6& a6, T7& a7 ) {
                collectargs.store( a1, a2, a3, a4, a5, a6, a7 );
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5 ) {
                collectargs.store( a1, a2, a3, a4, a5);
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            SendStatus collect_impl() {
                return mhandle.collect();
            }
            template<class T1>
            SendStatus collect_impl( T1& a1 ) {
                collectargs.store( a1 );
                return  mhandle.collect();
            }

            template<class T1, class T2>
            SendStatus collect_impl( T1& a1, T2& a2 ) {
                collectargs.store( a1, a2);
                return mhandle.collect();
            }

            template<class T1, class T2, class T3>
            SendStatus collect_impl( T1& a1, T2& a2, T3& a3 ) {
                collectargs.store( a1, a2, a3);
                return mhandle.collect();
            }

            template<class T1, class T2, class T3, class T4>
            SendStatus collect_impl( T1& a1, T2& a2, T3& a3, T4& a4 ) {
                collectargs.store( a1, a2, a3, a4);
                return mhandle.collect();
            }

            template<class T1, class T2, class T3, class T4, class T5>
            SendStatus collect_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5 ) {
                collectargs.store( a1, a2, a3, a4, a5);
                return mhandle.collect();
            }

            result_type ret_impl()
            {
                return sendargs.getResult(); // may return void.
            }

            /**
             * This function has the same signature of call() and
             * returns the stored return value, and tries to return
             * all arguments.
             */
            template<class T1>
            result_type ret_impl(T1 a1)
            {
                sendargs.store( a1 );
                mhandle.collectIfDone();
                return sendargs.getResult(); // may return void.
            }

            template<class T1,class T2>
            result_type ret_impl(T1 a1, T2 a2)
            {
                sendargs.store( a1, a2 );
                mhandle.collectIfDone();
                return sendargs.getResult(); // may return void.
            }

            template<class T1,class T2, class T3>
            result_type ret_impl(T1 a1, T2 a2, T3 a3)
            {
                sendargs.store( a1, a2, a3 );
                mhandle.collectIfDone();
                return sendargs.getResult(); // may return void.
            }

            template<class T1,class T2, class T3, class T4>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4)
            {
                sendargs.store( a1, a2, a3, a4 );
                mhandle.collectIfDone();
                return sendargs.getResult(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5, class T6>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)
            {
                sendargs.store( a1, a2, a3, a4, a5, a6 );
                mhandle.collectIfDone();
                return sendargs.getResult(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5, class T6, class T7>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)
            {
                sendargs.store( a1, a2, a3, a4, a5, a6, a7 );
                mhandle.collectIfDone();
                return sendargs.getResult(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
            {
                sendargs.store( a1, a2, a3, a4, a5);
                mhandle.collectIfDone();
                return sendargs.getResult(); // may return void.
            }
        };


        /**
         * A OperationCaller implementation which delegates C++ to datasource
         * conversions when C++ code tries to call remote operations, only
         * available in an OperationInterfacePart, and not through an
         * OperationBase. The datasources created as such are not
         * subject to copy/clone semantics since they only serve as
         * proxies to C++ arguments on the stack, when the OperationCaller
         * object is invoked. A OperationCallerC object is used to keep track
         * of all involved objects, and a SendHandleC object is used
         * to track the results of asynchronous calls.
         *
         * @param OperationCallerT The function signature of the remote operation. For example,
         * bool( int, Frame, double)
         *
         */
        template<class OperationCallerT>
        class RemoteOperationCaller
            : public Invoker<OperationCallerT,RemoteOperationCallerImpl<OperationCallerT> >
        {
        public:
            typedef OperationCallerT Signature;

            /**
             * Create a RemoteOperationCaller object which executes a remote method
             *
             * @param of The OperationFactory for methods.
             * @param name The name of this method.
             * @param caller The caller's engine of this operation. Will be reset if this RemoteOperationCaller is
             *               assigned to an OperationCaller.
             */
            RemoteOperationCaller(OperationInterfacePart* of, std::string name, ExecutionEngine* caller = 0)
            {
                // create the method.
                this->mmeth = OperationCallerC(of, name, caller);
                // add the arguments to the method.
                this->sendargs.initArgs( this->mmeth );
                this->sendargs.initRet(  this->mmeth );
                this->base::OperationCallerInterface::setCaller(caller);
            }

            RemoteOperationCaller(const SendHandleC& sh )
            {
                this->mhandle = sh;
                this->collectargs.initArgs( this->mhandle );
                // no need to collect on remote operations.
                this->mhandle.setAutoCollect(false);
            }

            virtual bool ready() const {
                return this->mmeth.ready();
            }

            virtual void setCaller(ExecutionEngine* caller) {
                // re-create the method.
                this->mmeth = OperationCallerC(this->mmeth, caller);
                // add the arguments to the method.
                this->sendargs.initArgs( this->mmeth );
                this->sendargs.initRet(  this->mmeth );
                this->base::OperationCallerInterface::setCaller(caller);
            }

            virtual base::OperationCallerBase<OperationCallerT>* cloneI(ExecutionEngine* caller) const {
                RemoteOperationCaller<OperationCallerT>* rm = new RemoteOperationCaller<OperationCallerT>( this->mmeth.getOrp(), this->mmeth.getName(), caller);
                return rm;
            }
        };
    }
}
#endif
