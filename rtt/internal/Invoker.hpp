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


#ifndef ORO_INVOKER_HPP
#define ORO_INVOKER_HPP

#include <boost/function.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/type_traits.hpp>
#include "InvokerBase.hpp"
#include "Collect.hpp"

namespace RTT
{
    namespace internal
    {
        template<int, class F, class BaseImpl>
        struct InvokerImpl;

        /**
         * Creates an invocation object with a function
         * signature to invoke and an implementation in which
         * an operator(args) is available which has this signature.
         */
        template<class F, class BaseImpl>
        struct Invoker
            : public InvokerImpl<boost::function_traits<F>::arity, F, BaseImpl>
        {};

        template<class F, class BaseImpl>
        struct InvokerImpl<0,F,BaseImpl>
            : public Collect<F,BaseImpl> // inherits from BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            /**
             * Invoke this operator if the method has no arguments.
             */
            result_type call()
            {
                return BaseImpl::call_impl();
            }

            result_type ret()
            {
                return BaseImpl::ret_impl();
            }

            SendHandle<F> send()
            {
                return BaseImpl::send_impl();
            }

        };

        template<class F, class BaseImpl>
        struct InvokerImpl<1,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            /**
             * Invoke this operator if the method has one argument.
             */
            result_type call(arg1_type a1)
            {
                return BaseImpl::template call_impl<arg1_type>( a1 );
            }
            result_type ret(arg1_type a1)
            {
                return BaseImpl::template ret_impl<arg1_type>( a1 );
            }
            result_type ret()
            {
                return BaseImpl::ret_impl();
            }
            SendHandle<F> send(arg1_type a1)
            {
                return BaseImpl::template send_impl<arg1_type>( a1 );
            }
        };

        template<class F, class BaseImpl>
        struct InvokerImpl<2,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;

            /**
             * Invoke this operator if the method has two arguments.
             */
            result_type call(arg1_type t1, arg2_type t2)
            {
                return BaseImpl::template call_impl<arg1_type, arg2_type>(t1, t2);
            }

            result_type ret(arg1_type t1, arg2_type t2)
            {
                return BaseImpl::template ret_impl<arg1_type, arg2_type>(t1, t2);
            }

            result_type ret()
            {
                return BaseImpl::ret_impl();
            }

            SendHandle<F> send(arg1_type t1, arg2_type t2)
            {
                return BaseImpl::template send_impl<arg1_type, arg2_type>(t1, t2);
            }
        };

        template<class F, class BaseImpl>
        struct InvokerImpl<3,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;

            /**
             * Invoke this operator if the method has three arguments.
             */
            result_type call(arg1_type t1, arg2_type t2, arg3_type t3)
            {
                return BaseImpl::template call_impl<arg1_type, arg2_type, arg3_type>(t1, t2, t3);
            }

            result_type ret(arg1_type t1, arg2_type t2, arg3_type t3)
            {
                return BaseImpl::template ret_impl<arg1_type, arg2_type, arg3_type>(t1, t2, t3);
            }

            result_type ret()
            {
                return BaseImpl::ret_impl();
            }

            SendHandle<F> send(arg1_type t1, arg2_type t2, arg3_type t3)
            {
                return BaseImpl::template send_impl<arg1_type, arg2_type, arg3_type>(t1, t2, t3);
            }

        };

        template<class F, class BaseImpl>
        struct InvokerImpl<4,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;

            /**
             * Invoke this operator if the method has four arguments.
             */
            result_type call(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4)
            {
                return BaseImpl::template call_impl<arg1_type, arg2_type, arg3_type, arg4_type>(t1, t2, t3, t4);
            }

            result_type ret(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4)
            {
                return BaseImpl::template ret_impl<arg1_type, arg2_type, arg3_type, arg4_type>(t1, t2, t3, t4);
            }

            result_type ret()
            {
                return BaseImpl::ret_impl();
            }

            SendHandle<F> send(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4)
            {
                return BaseImpl::template send_impl<arg1_type, arg2_type, arg3_type, arg4_type>(t1, t2, t3, t4);
            }

        };

   }
}
#endif
