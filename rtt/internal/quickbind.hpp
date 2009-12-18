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


#ifndef ORO_QUICKBIND_HPP
#define ORO_QUICKBIND_HPP

#include <boost/function.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/joint_view.hpp>

namespace RTT
{
    namespace internal
    {
        template<class F, class O, int>
        struct quickbind_impl;

        template<class F, class O>
        struct quickbind_impl<F,O,0>
        {
            F mf;
            O mo;

            quickbind_impl(F f, O o)
                : mf(f), mo(o) {}

            bool operator()() {
                return (mo->*mf)();
            }

            template<class T1>
            bool operator()(T1) {
                return (mo->*mf)();
            }

            template<class T1, class T2>
            bool operator()(T1, T2) {
                return (mo->*mf)();
            }

            template<class T1, class T2, class T3>
            bool operator()(T1, T2, T3) {
                return (mo->*mf)();
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1, T2, T3, T4) {
                return (mo->*mf)();
            }
        };

        template<class F, class O>
        struct quickbind_impl<F,O,1>
        {
            F mf;
            O mo;

            quickbind_impl(F f, O o)
                : mf(f), mo(o) {}

            template<class T1>
            bool operator()(T1& t1) {
                return (mo->*mf)(t1);
            }

            template<class T1, class T2>
            bool operator()(T1& t1, T2) {
                return (mo->*mf)(t1);
            }

            template<class T1, class T2, class T3>
            bool operator()(T1& t1, T2, T3) {
                return (mo->*mf)(t1);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1& t1, T2, T3, T4) {
                return (mo->*mf)(t1);
            }
        };

        template<class F, class O>
        struct quickbind_impl<F,O,2>
        {
            F mf;
            O mo;

            quickbind_impl(F f, O o)
                : mf(f), mo(o) {}

            template<class T1, class T2>
            bool operator()(T1& t1, T2& t2) {
                return (mo->*mf)(t1,t2);
            }

            template<class T1, class T2, class T3>
            bool operator()(T1& t1, T2 t2, T3) {
                return (mo->*mf)(t1,t2);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1& t1, T2 t2, T3, T4) {
                return (mo->*mf)(t1,t2);
            }
        };

        template<class F, class O>
        struct quickbind_impl<F,O,3>
        {
            F mf;
            O mo;

            quickbind_impl(F f, O o)
                : mf(f), mo(o) {}

            template<class T1, class T2, class T3>
            bool operator()(T1& t1, T2 t2, T3 t3) {
                return (mo->*mf)(t1,t2,t3);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1& t1, T2 t2, T3 t3, T4) {
                return (mo->*mf)(t1,t2,t3);
            }
        };

        template<class F, class O>
        struct quickbind_impl<F,O,4>
        {
            F mf;
            O mo;

            quickbind_impl(F f, O o)
                : mf(f), mo(o) {}

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1& t1, T2 t2, T3 t3, T4 t4) {
                return (mo->*mf)(t1,t2,t3,t4);
            }
        };


        template<class F, int>
        struct quickbindC_impl;

        template<class F>
        struct quickbindC_impl<F,0>
        {
            F mf;

            quickbindC_impl(F f)
                : mf(f) {}

            bool operator()() {
                return mf();
            }

            template<class T1>
            bool operator()(T1) {
                return mf();
            }

            template<class T1, class T2>
            bool operator()(T1, T2) {
                return mf();
            }

            template<class T1, class T2, class T3>
            bool operator()(T1, T2, T3) {
                return mf();
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1, T2, T3, T4) {
                return mf();
            }
        };

        template<class F>
        struct quickbindC_impl<F,1>
        {
            F mf;

            quickbindC_impl(F f)
                : mf(f) {}

            template<class T1>
            bool operator()(T1& t1) {
                return mf(t1);
            }

            template<class T1, class T2>
            bool operator()(T1& t1, T2) {
                return mf(t1);
            }

            template<class T1, class T2, class T3>
            bool operator()(T1& t1, T2, T3) {
                return mf(t1);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1& t1, T2, T3, T4) {
                return mf(t1);
            }
        };

        template<class F>
        struct quickbindC_impl<F,2>
        {
            F mf;

            quickbindC_impl(F f)
                : mf(f) {}

            template<class T1,class T2>
            bool operator()(T1& t1, T2& t2) {
                return mf(t1,t2);
            }

            template<class T1,class T2, class T3>
            bool operator()(T1& t1, T2& t2, T3) {
                return mf(t1,t2);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1& t1, T2& t2, T3, T4) {
                return mf(t1,t2);
            }
        };

        template<class F>
        struct quickbindC_impl<F,3>
        {
            F mf;

            quickbindC_impl(F f)
                : mf(f) {}

            template<class T1, class T2, class T3>
            bool operator()(T1& t1, T2& t2, T3& t3) {
                return mf(t1,t2,t3);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1& t1, T2& t2, T3& t3, T4) {
                return mf(t1,t2,t3);
            }
        };

        template<class F>
        struct quickbindC_impl<F,4>
        {
            F mf;

            quickbindC_impl(F f)
                : mf(f) {}
            template<class T1, class T2, class T3, class T4>
            bool operator()(T1& t1, T2& t2, T3& t3, T4& t4) {
                return mf(t1,t2,t3,t4);
            }
        };

        /**
         * A class which binds a Type F to object O. If it is invoked
         * with more arguments than F, the extra arguments are discarded.
         */
        template<class F, class O>
        struct quickbind
            : public quickbind_impl<F,O, boost::function_types::function_arity<F>::value>
        {
            quickbind(F f, O o)
                : quickbind_impl<F,O, boost::function_types::function_arity<F>::value>(f,o) {}
        };

        /**
         * A class which binds a C style function F. If it is invoked
         * with more arguments than F, the extra arguments are discarded.
         */
        template<class F>
        struct quickbindC
            : public quickbindC_impl<F, boost::function_types::function_arity<F>::value>
        {
            quickbindC(F f)
                : quickbindC_impl<F, boost::function_types::function_arity<F>::value>(f) {}
        };
    }
}
#endif
