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


#ifndef ORO_INVOKER_BASE_HPP
#define ORO_INVOKER_BASE_HPP

#include <boost/function.hpp>
#include "boost/function_types/function_type.hpp"
#include "boost/function_types/function_type_arity.hpp"
#include "NA.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A class which converts a function type signature \a F
         * to a virtual operator()() interface.
         */
        template<int, class F>
        struct InvokerBaseImpl;

        template<class F>
        struct InvokerBaseImpl<0,F>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            virtual ~InvokerBaseImpl() {}
            virtual result_type operator()() = 0;
        };

        template<class F>
        struct InvokerBaseImpl<1,F>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function<F>::arg1_type arg1_type;
            virtual ~InvokerBaseImpl() {}
            virtual result_type operator()(arg1_type a1) = 0;
        };

        template<class F>
        struct InvokerBaseImpl<2,F>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            virtual ~InvokerBaseImpl() {}
            virtual result_type operator()(arg1_type a1, arg2_type a2) = 0;
        };

        template<class F>
        struct InvokerBaseImpl<3,F>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            virtual ~InvokerBaseImpl() {}
            virtual result_type operator()(arg1_type a1, arg2_type a2, arg3_type a3) = 0;
        };

        template<class F>
        struct InvokerBaseImpl<4,F>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            typedef typename boost::function<F>::arg4_type arg4_type;
            virtual ~InvokerBaseImpl() {}
            virtual result_type operator()(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) = 0;
        };

        template<class F>
        struct InvokerBase
            : public InvokerBaseImpl<boost::function_traits<F>::arity, F>
        {};

        template<int, class F, class BaseImpl>
        struct InvokerImpl;

        template<class F, class BaseImpl>
        struct InvokerImpl<0,F,BaseImpl>
            : public BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            /**
             * Invoke this operator if the method has no arguments.
             */
            result_type operator()()
            {
                return BaseImpl::invoke();
            }
        };

        template<class F, class BaseImpl>
        struct InvokerImpl<1,F,BaseImpl>
            : public BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            /**
             * Invoke this operator if the method has one argument.
             */
            result_type operator()(arg1_type a1)
            {
                return BaseImpl::template invoke<arg1_type>( a1 );
            }
        };

        template<class F, class BaseImpl>
        struct InvokerImpl<2,F,BaseImpl>
            : public BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;

            /**
             * Invoke this operator if the method has two arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2)
            {
                return BaseImpl::template invoke<arg1_type, arg2_type>(t1, t2);
            }

        };

        template<class F, class BaseImpl>
        struct InvokerImpl<3,F,BaseImpl>
            : public BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;

            /**
             * Invoke this operator if the method has three arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2, arg3_type t3)
            {
                return BaseImpl::template invoke<arg1_type, arg2_type, arg3_type>(t1, t2, t3);
            }

        };

        template<class F, class BaseImpl>
        struct InvokerImpl<4,F,BaseImpl>
            : public BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;

            /**
             * Invoke this operator if the method has four arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4)
            {
                return BaseImpl::template invoke<arg1_type, arg2_type, arg3_type, arg4_type>(t1, t2, t3, t4);
            }

        };

        /**
         * Creates an invocation object with a function
         * signature to invoke and an implementation in which
         * an operator(args) is available which has this signature.
         */
        template<class F, class BaseImpl>
        struct Invoker
            : public InvokerImpl<boost::function_traits<F>::arity, F, BaseImpl>
        {};


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
                    return (*impl)();
                return NA<result_type>::na();
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<1,F,ToInvoke>
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
                    return (*impl)( a1 );
                return NA<result_type>::na();
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<2,F,ToInvoke>
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
                    return (*impl)(t1, t2);
                return NA<result_type>::na();
            }

        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<3,F,ToInvoke>
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
                    return (*impl)(t1, t2, t3);
                return NA<result_type>::na();
            }

        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<4,F,ToInvoke>
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
                    return (*impl)(t1, t2, t3, t4);
                return NA<result_type>::na();
            }

        protected:
            ToInvoke impl;
        };

    }
}
#endif
