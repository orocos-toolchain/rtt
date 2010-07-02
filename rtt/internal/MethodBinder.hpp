/***************************************************************************
  tag: FMTC  do nov 2 13:06:12 CET 2006  BindStorage.hpp

                        BindStorage.hpp -  description
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


#ifndef ORO_TASK_METHOD_BINDER_HPP
#define ORO_TASK_METHOD_BINDER_HPP

#include <boost/function.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

namespace RTT
{
    namespace internal
    {
        template<int, class F>
        struct MethodBinderImpl;

        template<class F>
        struct MethodBinderImpl<0,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o );
            }
        };

        template<class F>
        struct MethodBinderImpl<1,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1 );
            }
        };

        template<class F>
        struct MethodBinderImpl<2,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1, ::_2 );
            }
        };

        template<class F>
        struct MethodBinderImpl<3,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1, ::_2, ::_3 );
            }
        };

        template<class F>
        struct MethodBinderImpl<4,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1, ::_2, ::_3, ::_4 );
            }
        };

        template<class F>
        struct MethodBinderImpl<5,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1, ::_2, ::_3, ::_4, ::_5 );
            }
        };

        template<class F>
        struct MethodBinderImpl<6,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1, ::_2, ::_3, ::_4, ::_5, ::_6 );
            }
        };

        template<class F>
        struct MethodBinderImpl<7,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1, ::_2, ::_3, ::_4, ::_5, ::_6, ::_7 );
            }
        };

        template<class F>
        struct MethodBinderImpl<8,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1, ::_2, ::_3, ::_4, ::_5, ::_6, ::_7, ::_8 );
            }
        };

        template<class F>
        struct MethodBinderImpl<9,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, ::_1, ::_2, ::_3, ::_4, ::_5, ::_6, ::_7, ::_8, ::_9 );
            }
        };

        /**
         * Very simple \b factory class to bind a member function to an
         * object pointer and leave the arguments open. The operator()
         * returns a boost::function<F> object.
         *
         * There is no constructor and the operator() is
         * implemented in the MethodBinderImpl base classes.
         * @param F A function signature (like 'int(double)')
         * which is the signature of the member function to be bound
         * and the boost::function signature to return.
         */
        template<class F>
        struct MethodBinder
            : public MethodBinderImpl<boost::function_traits<F>::arity, F>
        {};
    }
}
#endif
