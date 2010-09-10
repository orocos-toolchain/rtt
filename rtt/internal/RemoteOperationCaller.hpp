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
              public internal::CollectBase<OperationCallerT>,
              protected DataSourceStorage<OperationCallerT>
        {
        protected:
            OperationCallerC mmeth;
            SendHandleC mhandle;
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
            virtual void dispose() { assert(false); }

            virtual void setExecutor(ExecutionEngine* ee) {
            }

            virtual void setCaller(ExecutionEngine* ee) {
            }


            /**
             * Call this operator if the RemoteOperationCaller takes no arguments.
             *
             * @return true if ready and succesfully sent.
             */
            result_type call_impl() {
                mmeth.call();
                return this->DataSourceStorage<OperationCallerT>::getResult();
            }

            template<class T1>
            result_type call_impl( T1 a1 ) {
                this->store( a1 );
                mmeth.call();
                return this->DataSourceStorage<OperationCallerT>::getResult();
            }

            template<class T1, class T2>
            result_type call_impl( T1 a1, T2 a2 ) {
                this->store( a1, a2 );
                mmeth.call();
                return this->DataSourceStorage<OperationCallerT>::getResult();
            }

            template<class T1, class T2, class T3>
            result_type call_impl( T1 a1, T2 a2, T3 a3 ) {
                this->store( a1, a2, a3 );
                mmeth.call();
                return this->DataSourceStorage<OperationCallerT>::getResult();
            }

            template<class T1, class T2, class T3, class T4>
            result_type call_impl( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                this->store( a1, a2, a3, a4 );
                mmeth.call();
                return this->DataSourceStorage<OperationCallerT>::getResult();
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            result_type call_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6 ) {
                this->store( a1, a2, a3, a4, a5, a6 );
                mmeth.call();
                return this->DataSourceStorage<OperationCallerT>::getResult();
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            result_type call_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7 ) {
                this->store( a1, a2, a3, a4, a5, a6, a7 );
                mmeth.call();
                return this->DataSourceStorage<OperationCallerT>::getResult();
            }

            template<class T1, class T2, class T3, class T4, class T5>
            result_type call_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5 ) {
                this->store( a1, a2, a3, a4, a5 );
                mmeth.call();
                return this->DataSourceStorage<OperationCallerT>::getResult();
            }

            SendHandle<Signature> send_impl() {
                mhandle = mmeth.send();
                // @todo: get remote collect from rt allocation.
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1>
            SendHandle<Signature> send_impl( T1 a1 ) {
                this->store( a1 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2>
            SendHandle<Signature> send_impl( T1 a1, T2 a2 ) {
                this->store( a1, a2 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3 ) {
                this->store( a1, a2, a3 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3, class T4>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                this->store( a1, a2, a3, a4 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3, class T4, class T5>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5 ) {
                this->store( a1, a2, a3, a4, a5 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6 ) {
                this->store( a1, a2, a3, a4, a5, a6 );
                mhandle = mmeth.send();
                return SendHandle<Signature>( boost::make_shared< RemoteOperationCaller<OperationCallerT> >( mhandle ) );
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7 ) {
                this->store( a1, a2, a3, a4, a5, a6, a7 );
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
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    bf::vector_tie(a1) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2>
            SendStatus collectIfDone_impl( T1& a1, T2& a2 ) {
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    bf::vector_tie(a1,a2) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                }
                return SendNotReady;
            }

            template<class T1, class T2, class T3>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3 ) {
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    bf::vector_tie(a1,a2,a3) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4 ) {
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    bf::vector_tie(a1,a2,a3,a4) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5, T6& a6 ) {
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    bf::vector_tie(a1,a2,a3,a4,a5,a6) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5, T6& a6, T7& a7 ) {
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    bf::vector_tie(a1,a2,a3,a4,a5,a6,a7) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4, class T5>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4, T5& a5 ) {
                if (  mhandle.collectIfDone() == SendSuccess ) {
                    bf::vector_tie(a1,a2,a3,a4,a5) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            SendStatus collect_impl() {
                return mhandle.collect();
            }
            template<class T1>
            SendStatus collect_impl( T1& a1 ) {
                mhandle.collect();
                return this->collectIfDone_impl(a1);
            }

            template<class T1, class T2>
            SendStatus collect_impl( T1& a1, T2& a2 ) {
                mhandle.collect();
                return this->collectIfDone_impl(a1,a2);
            }

            template<class T1, class T2, class T3>
            SendStatus collect_impl( T1& a1, T2& a2, T3& a3 ) {
                mhandle.collect();
                return this->collectIfDone_impl(a1,a2,a3);
            }


            result_type ret_impl()
            {
                return this->getResult(); // may return void.
            }

            /**
             * This function has the same signature of call() and
             * returns the stored return value, and tries to return
             * all arguments.
             */
            template<class T1>
            result_type ret_impl(T1 a1)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1> vArgs( boost::ref(a1) );
                if ( mhandle.collectIfDone() == SendSuccess )
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->getResult(); // may return void.
            }

            template<class T1,class T2>
            result_type ret_impl(T1 a1, T2 a2)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2> vArgs( boost::ref(a1), boost::ref(a2) );
                if ( mhandle.collectIfDone() == SendSuccess )
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg< boost::remove_reference<mpl::_> > >(this->vStore);
                return this->getResult(); // may return void.
            }

            template<class T1,class T2, class T3>
            result_type ret_impl(T1 a1, T2 a2, T3 a3)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3) );
                if ( mhandle.collectIfDone() == SendSuccess )
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->getResult(); // may return void.
            }

            template<class T1,class T2, class T3, class T4>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4) );
                if ( mhandle.collectIfDone() == SendSuccess )
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->getResult(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5, class T6>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4,T5,T6> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4), boost::ref(a5), boost::ref(a6) );
                if ( mhandle.collectIfDone() == SendSuccess )
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->getResult(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5, class T6, class T7>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4,T5,T6,T7> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4), boost::ref(a5), boost::ref(a6), boost::ref(a7) );
                if ( mhandle.collectIfDone() == SendSuccess )
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->getResult(); // may return void.
            }

            template<class T1,class T2, class T3, class T4, class T5>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4,T5> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4), boost::ref(a5) );
                if ( mhandle.collectIfDone() == SendSuccess )
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->getResult(); // may return void.
            }

        };


        /**
         * A OperationCaller implementation which delegates C++ to datasource
         * conversions when C++ code tries to call remote methods, only
         * available in an OperationInterfacePart, and not through an
         * OperationBase. The datasources created as such are not
         * subject to copy/clone semantics since they only serve as
         * proxies to C++ arguments on the stack, when the OperationCaller
         * object is invoked. A OperationCallerC object is used to keep track
         * of all involved objects, and a SendHandleC object is used
         * to track the results of asynchronous calls.
         *
         * @param OperationCallerT The function signature of the method. For example,
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
             * @param name The name of this method.
             * @param com The OperationFactory for methods.
             */
            RemoteOperationCaller(OperationInterfacePart* of, std::string name, ExecutionEngine* caller)
            {
                // create the method.
                this->mmeth = OperationCallerC(of, name, caller);
                // add the arguments to the method.
                this->initArgs( this->mmeth );
                this->initRet(  this->mmeth );
            }

            RemoteOperationCaller(const SendHandleC& sh )
            {
                this->mhandle = sh;
            }

            virtual void readArguments() {}

            virtual bool ready() const {
                return this->mmeth.ready();
            }

            virtual bool execute() {
                return this->mmeth.call();
            }

            virtual base::OperationCallerBase<OperationCallerT>* cloneI(ExecutionEngine* caller) const {
                RemoteOperationCaller<OperationCallerT>* rm = new RemoteOperationCaller<OperationCallerT>( this->mmeth.getOrp(), this->mmeth.getName(), caller);
                return rm;
            }
        };
    }
}
#endif
