/***************************************************************************
  tag: FMTC  do nov 2 13:06:09 CET 2006  RemoteMethod.hpp

                        RemoteMethod.hpp -  description
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
#include "../base/MethodBase.hpp"
#include "MethodC.hpp"
#include "DataSourceStorage.hpp"
#include "Invoker.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * A Method which executes a remote function directly.
         * @param MethodT The function signature of the method. For example,
         * bool( int, Frame, double)
         *
         */
        template<class MethodT>
        class RemoteMethodImpl
            : public base::MethodBase<MethodT>,
              protected DataSourceStorage<MethodT>
        {
        protected:
            MethodC mmeth;
        public:
            typedef typename boost::function_traits<MethodT>::result_type result_type;

            /**
             * The defaults are reset by the constructor.
             */
            RemoteMethodImpl()
                : mmeth()
            {}

            /**
             * Call this operator if the RemoteMethod takes no arguments.
             *
             * @return true if ready and succesfully sent.
             */
            result_type invoke() {
                mmeth.call();
                return this->DataSourceStorage<MethodT>::getResult();
            }

            template<class T1>
            result_type invoke( T1 a1 ) {
                this->store( a1 );
                mmeth.call();
                return this->DataSourceStorage<MethodT>::getResult();
            }

            template<class T1, class T2>
            result_type invoke( T1 a1, T2 a2 ) {
                this->store( a1, a2 );
                mmeth.call();
                return this->DataSourceStorage<MethodT>::getResult();
            }

            template<class T1, class T2, class T3>
            result_type invoke( T1 a1, T2 a2, T3 a3 ) {
                this->store( a1, a2, a3 );
                mmeth.call();
                return this->DataSourceStorage<MethodT>::getResult();
            }

            template<class T1, class T2, class T3, class T4>
            result_type invoke( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                this->store( a1, a2, a3, a4 );
                mmeth.call();
                return this->DataSourceStorage<MethodT>::getResult();
            }

            SendHandle<Signature> send_impl() {
                mmeth.send();
                return this->DataSourceStorage<MethodT>::sendHandle();
            }

            template<class T1>
            SendHandle<Signature> send_impl( T1 a1 ) {
                this->store( a1 );
                mmeth.send();
                return this->DataSourceStorage<MethodT>::sendHandle();
            }

            template<class T1, class T2>
            SendHandle<Signature> send_impl( T1 a1, T2 a2 ) {
                this->store( a1, a2 );
                mmeth.send();
                return this->DataSourceStorage<MethodT>::sendHandle();
            }

            template<class T1, class T2, class T3>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3 ) {
                this->store( a1, a2, a3 );
                mmeth.send();
                return this->DataSourceStorage<MethodT>::sendHandle();
            }

            template<class T1, class T2, class T3, class T4>
            SendHandle<Signature> send_impl( T1 a1, T2 a2, T3 a3, T4 a4 ) {
                this->store( a1, a2, a3, a4 );
                mmeth.send();
                return this->DataSourceStorage<MethodT>::sendHandle();
            }

            SendStatus collectIfDone_impl() {
                if ( this->DataSourceStorage<MethodT>::isExecuted()) {
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1>
            SendStatus collectIfDone_impl( T1& a1 ) {
                if ( this->DataSourceStorage<MethodT>::isExecuted() ) {
                    bf::vector_tie(a1) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2>
            SendStatus collectIfDone_impl( T1& a1, T2& a2 ) {
                if ( this->retn.isExecuted()) {
                    bf::vector_tie(a1,a2) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                }
                return SendNotReady;
            }

            template<class T1, class T2, class T3>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3 ) {
                if ( this->retn.isExecuted()) {
                    bf::vector_tie(a1,a2,a3) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            template<class T1, class T2, class T3, class T4>
            SendStatus collectIfDone_impl( T1& a1, T2& a2, T3& a3, T4& a4 ) {
                if ( this->retn.isExecuted()) {
                    bf::vector_tie(a1,a2,a3,a4) = bf::filter_if< is_arg_return<boost::remove_reference<mpl::_> > >(this->vStore);
                    return SendSuccess;
                } else
                    return SendNotReady;
            }

            SendStatus collect_impl() {
                caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retn)) );
                return this->collectIfDone_impl();
            }
            template<class T1>
            SendStatus collect_impl( T1& a1 ) {
                caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retn)) );
                return this->collectIfDone_impl(a1);
            }

            template<class T1, class T2>
            SendStatus collect_impl( T1& a1, T2& a2 ) {
                caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retn)) );
                return this->collectIfDone_impl(a1,a2);
            }

            template<class T1, class T2, class T3>
            SendStatus collect_impl( T1& a1, T2& a2, T3& a3 ) {
                caller->waitForMessages( boost::bind(&Store::RStoreType::isExecuted,boost::ref(this->retn)) );
                return this->collectIfDone_impl(a1,a2,a3);
            }

            /**
             * Invoke this operator if the method has no arguments.
             */
            result_type call_impl()
            {

                if (met == OwnThread && myengine != caller) {
                    SendHandle<Signature> h = send_impl();
                    if ( h.collect() == SendSuccess )
                        return h.ret();
                    else
                        throw SendFailure;
                } else
                    return this->invoke(); // ClientThread
            }


            /**
             * Invoke this operator if the method has one argument.
             */
            template<class T1>
            result_type call_impl(T1 a1)
            {
                SendHandle<Signature> h;
                if (met == OwnThread && myengine != caller) {
                    h = send_impl(a1);
                    // collect_impl may take diff number of arguments than
                    // call_impl/ret_impl(), so we use generic collect() + ret_impl()
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1);
                    else
                        throw SendFailure;
                } else
                    return this->invoke(a1);
                return NA<result_type>::na();
            }

            template<class T1, class T2>
            result_type call_impl(T1 a1, T2 a2)
            {
                SendHandle<Signature> h;
                if (met == OwnThread && myengine != caller) {
                    h = send_impl(a1,a2);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2);
                    else
                        throw SendFailure;
                } else
                    return this->invoke(a1,a2);
                return NA<result_type>::na();
            }

            template<class T1, class T2, class T3>
            result_type call_impl(T1 a1, T2 a2, T3 a3)
            {
                SendHandle<Signature> h;
                if (met == OwnThread && myengine != caller) {
                    h = send_impl(a1,a2,a3);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3);
                    else
                        throw SendFailure;
                } else
                    return this->invoke(a1,a2,a3);
                return NA<result_type>::na();
            }

            template<class T1, class T2, class T3, class T4>
            result_type call_impl(T1 a1, T2 a2, T3 a3, T4 a4)
            {
                SendHandle<Signature> h;
                if (met == OwnThread && myengine != caller) {
                    h = send_impl(a1,a2,a3,a4);
                    if ( h.collect() == SendSuccess )
                        return h.ret(a1,a2,a3,a4);
                    else
                        throw SendFailure;
                } else
                    return this->invoke(a1,a2,a3,a4);
                return NA<result_type>::na();
            }

            result_type ret_impl()
            {
                return this->retn.result(); // may return void.
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
                if ( this->retn.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retn.result(); // may return void.
            }

            template<class T1,class T2>
            result_type ret_impl(T1 a1, T2 a2)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2> vArgs( boost::ref(a1), boost::ref(a2) );
                if ( this->retn.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg< boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retn.result(); // may return void.
            }

            template<class T1,class T2, class T3>
            result_type ret_impl(T1 a1, T2 a2, T3 a3)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3) );
                if ( this->retn.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retn.result(); // may return void.
            }

            template<class T1,class T2, class T3, class T4>
            result_type ret_impl(T1 a1, T2 a2, T3 a3, T4 a4)
            {
                typedef mpl::and_<boost::is_reference<mpl::_>, mpl::not_<boost::is_const<boost::remove_reference<mpl::_> > > > pred;
                bf::vector<T1,T2,T3,T4> vArgs( boost::ref(a1), boost::ref(a2), boost::ref(a3), boost::ref(a4) );
                if ( this->retn.isExecuted())
                    as_vector(bf::filter_if< pred >(vArgs)) = bf::filter_if< is_out_arg<boost::remove_reference<mpl::_> > >(this->vStore);
                return this->retn.result(); // may return void.
            }

        };


        /**
         * A Method which is dispatched remotely to a MethodProcessor.
         * @param MethodT The function signature of the method. For example,
         * bool( int, Frame, double)
         *
         */
        template<class MethodT>
        class RemoteMethod
            : public Invoker<MethodT,RemoteMethodImpl<MethodT> >
        {
        public:
            typedef MethodT Signature;

            /**
             * Create a RemoteMethod object which executes a remote method
             *
             * @param name The name of this method.
             * @param com The OperationFactory for methods.
             */
            RemoteMethod(ServiceProviderPtr of, std::string name, ExecutionEngine* caller)
            {
                // create the method.
                this->mmeth = MethodC(of, name, caller);
                // add the arguments to the method.
                this->initArgs( this->mmeth );
                this->initRet(  this->mmeth );
            }

            virtual void readArguments() {}

            virtual bool ready() const {
                return this->mmeth.ready();
            }

            virtual bool execute() {
                return this->mmeth.call();
            }

            /**
             * Creates a clone of this RemoteMethod object.
             * Use this method to get a new method object
             * which has its own state information.
             *
             * @return
             */
            virtual base::ActionInterface* clone() const {
                return new RemoteMethod(*this);
            }

            virtual base::MethodBase<MethodT>* cloneI() const {
                return new RemoteMethod(*this);
            }
        };
    }
}
#endif
