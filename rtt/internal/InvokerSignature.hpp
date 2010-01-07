/***************************************************************************
  tag: FMTC  do nov 2 13:05:58 CET 2006  Invoker.hpp

                        Invoker.hpp -  description
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


#ifndef ORO_INVOKER_SIGNATURE_HPP
#define ORO_INVOKER_SIGNATURE_HPP

#include <boost/type_traits.hpp>
#include "../SendHandle.hpp"
#include "CollectSignature.hpp"
#include "NA.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * Used by various classes to define operator(), given a Signature.
         * @param int The number of arguments of operator().
         * @param Signature The C-style function signature (function type).
         * @param ToInvoke A class type which is called within operator().
         */
        template<int, class Signature, class ToInvoke>
        struct InvokerSignature;

        template<class F, class ToInvoke>
        struct InvokerSignature<0,F,ToInvoke>
            : public CollectSignature< boost::function_traits<typename CollectType<F>::type>::arity, typename CollectType<F>::type, ToInvoke >
        {
            typedef typename boost::function_traits<F>::result_type result_type;

            InvokerSignature() : impl() {}
            InvokerSignature(ToInvoke implementation) : impl(implementation) {}
            ~InvokerSignature() {}

            /**
             * Invoke this operator if the method has no arguments.
             */
            result_type operator()()
            {
                if (impl)
                    return impl->call();
                return NA<result_type>::na();
            }

            result_type call() {
                return operator()();
            }

            SendHandle<F> send()
            {
                if (impl)
                    return impl->send();
                return SendFailure;
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<1,F,ToInvoke>
            : public CollectSignature< boost::function_traits<typename CollectType<F>::type>::arity, typename CollectType<F>::type, ToInvoke >
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;

            InvokerSignature() : impl() {}
            InvokerSignature(ToInvoke implementation) : impl(implementation) {}
            ~InvokerSignature() {}

            /**
             * Invoke this operator if the method has one argument.
             */
            result_type operator()(arg1_type a1)
            {
                if (impl)
                    return impl->call( a1 );
                return NA<result_type>::na();
            }
            result_type call(arg1_type a1) {
                return operator()(a1);
            }

            result_type ret(arg1_type a1) {
                if (impl)
                    return impl->ret( a1 );
                return NA<result_type>::na();
            }

            SendHandle<F> send(arg1_type a1)
            {
                if (impl)
                    return impl->send(a1);
                return SendFailure;
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<2,F,ToInvoke>
            : public CollectSignature< boost::function_traits<typename CollectType<F>::type>::arity, typename CollectType<F>::type, ToInvoke >
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;

            InvokerSignature() : impl() {}
            InvokerSignature(ToInvoke implementation) : impl(implementation) {}
            ~InvokerSignature() {}

            /**
             * Invoke this operator if the method has two arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2)
            {
                if (impl)
                    return impl->call(t1, t2);
                return NA<result_type>::na();
            }

            result_type call(arg1_type a1, arg2_type a2) {
                return operator()(a1,a2);
            }

            SendHandle<F> send(arg1_type a1, arg2_type a2)
            {
                if (impl)
                    return impl->send(a1,a2);
                return SendFailure;
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<3,F,ToInvoke>
        : public CollectSignature<boost::function_traits<typename CollectType<F>::type>::arity, typename CollectType<F>::type, ToInvoke >
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;

            InvokerSignature() : impl() {}
            InvokerSignature(ToInvoke implementation) : impl(implementation) {}
            ~InvokerSignature() { }

            /**
             * Invoke this operator if the method has three arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2, arg3_type t3)
            {
                if (impl)
                    return impl->call(t1, t2, t3);
                return NA<result_type>::na();
            }

            result_type call(arg1_type a1, arg2_type a2, arg3_type a3) {
                return operator()(a1,a2,a3);
            }

            SendHandle<F> send(arg1_type a1, arg2_type a2, arg3_type a3)
            {
                if (impl)
                    return impl->send(a1,a2,a3);
                return SendFailure;
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<4,F,ToInvoke>
            : public CollectSignature<boost::function_traits<typename CollectType<F>::type>::arity, typename CollectType<F>::type, ToInvoke >
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;

            InvokerSignature() : impl() {}
            InvokerSignature(ToInvoke implementation) : impl(implementation) {}
            ~InvokerSignature() { }

            /**
             * Invoke this operator if the method has four arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4)
            {
                if (impl)
                    return impl->call(t1, t2, t3, t4);
                return NA<result_type>::na();
            }

            result_type call(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) {
                return operator()(a1,a2,a3,a4);
            }

            SendHandle<F> send(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4)
            {
                if (impl)
                    return impl->send(a1,a2,a3,a4);
                return SendFailure;
            }

        protected:
            ToInvoke impl;
        };

    }
}
#endif
