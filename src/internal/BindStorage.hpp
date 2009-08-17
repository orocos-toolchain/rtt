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


#ifndef ORO_TASK_BIND_STORAGE_HPP
#define ORO_TASK_BIND_STORAGE_HPP

#include <boost/function.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include "boost/function_types/function_type.hpp"
#include "boost/function_types/function_type_arity.hpp"

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
            : public quickbind_impl<F,O, boost::function_type_arity<F>::value>
        {
            quickbind(F f, O o)
                : quickbind_impl<F,O, boost::function_type_arity<F>::value>(f,o) {}
        };

        /**
         * A class which binds a C style function F. If it is invoked
         * with more arguments than F, the extra arguments are discarded.
         */
        template<class F>
        struct quickbindC
            : public quickbindC_impl<F, boost::function_type_arity<F>::value>
        {
            quickbindC(F f)
                : quickbindC_impl<F, boost::function_type_arity<F>::value>(f) {}
        };

        /**
         * Store a bound argument which may be a reference, const reference or
         * any other type.
         */
        template<class T>
        struct AStore {
            T arg;
            AStore() : arg() {}
            AStore(T t) : arg(t) {}

            T operator()() { return arg; }
            void operator()(T a) { arg = a; }
        };

        template<class T>
        struct AStore<T&>
        {
            T* arg;
            AStore() : arg(0) {}
            AStore(T& t) : arg(&t) {}

            T& operator()() { return *arg; }
            void operator()(T& a) { arg = &a; }
        };

        template<class T>
        struct AStore<const T &>
        {
            const T* arg;
            AStore() : arg(0) {}
            AStore(const T& t) : arg(&t) {}

            const T& operator()() { return *arg; }
            void operator()(const T& a) { arg = &a; }
        };

        template<int, class T>
        struct BindStorageImpl;

        /**
         * When no arguments are to be stored, the
         * implementation stores the function in a boost::function object.
         */
        template<class ToBind>
        struct BindStorageImpl<0, ToBind>
        {
            typedef bool result_type;

            // stores the original function pointer
            boost::function<ToBind> exec;
            boost::function<ToBind> check;

            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                exec = quickbind<F,ObjectType>( f, t); // allocates
                check  = quickbind<C,ObjectType>( c, t); // allocates
            }

            template<class F, class C>
            void setup(F f, C c)
            {
                exec = f;
                check = c;
            }

            void setup(boost::function<ToBind> f, boost::function<ToBind> c)
            {
                exec = f;
                check = c;
            }

            boost::function<ToBind> command() const {return exec;}
            boost::function<ToBind> condition() const {return check;}
        };

        /**
         * Stores a pointer to a function and its arguments.
         */
        template<class ToBind>
        struct BindStorageImpl<1, ToBind>
        {
            typedef bool result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;

            // stores the original function pointer, supplied by the user.
            boost::function<ToBind>  comm;
            // Wrap the condition.
            boost::function<ToBind> cond;
            // Store the argument.
            mutable AStore<arg1_type> a1;

            void store(arg1_type t1) { a1(t1); }
            bool exec() { return comm( a1() ); }
            bool check() const { return cond( a1() ); }

            /**
             * The object already stores the user class function pointer and
             * the pointer to the class object.
             */
            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                comm = quickbind<F,ObjectType>( f, t); // allocates
                cond = quickbind<C,ObjectType>( c, t); // allocates
            }

            template<class F, class C>
            void setup(F f, C c)
            {
                comm = quickbindC<F>(f); // allocates
                cond = quickbindC<C>(c);
            }

            void setup(boost::function<ToBind> f, boost::function<ToBind> c)
            {
                comm = f;
                cond = c;
            }

            boost::function<ToBind> command() const {return comm;}
            boost::function<ToBind> condition() const {return cond;}
        };

        template<class ToBind>
        struct BindStorageImpl<2, ToBind>
        {
            typedef bool result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;

            // stores the original function pointer
            boost::function<ToBind> comm;
            boost::function<ToBind> cond;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;

            void store(arg1_type t1, arg2_type t2) { a1(t1); a2(t2); }
            bool exec() { return comm( a1(), a2() ); }
            bool check() const { return cond( a1(), a2() ); }

            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                comm = boost::bind<bool>( boost::mem_fn(f), t, _1, _2 ); // allocates
                cond = quickbind<C,ObjectType>( c, t); // allocates
            }

            template<class F, class C>
            void setup(F f, C c)
            {
                comm = boost::bind<bool>( f, _1, _2 ); // allocates
                cond = quickbindC<C>(c);
            }

            void setup(boost::function<ToBind> f, boost::function<ToBind> c)
            {
                comm = f;
                cond = c;
            }

            boost::function<ToBind> command() const {return comm;}
            boost::function<ToBind> condition() const {return cond;}
        };

        template<class ToBind>
        struct BindStorageImpl<3, ToBind>
        {
            typedef bool result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;

            // stores the original function pointer
            boost::function<ToBind> comm;
            boost::function<ToBind> cond;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;

            void store(arg1_type t1, arg2_type t2, arg3_type t3) { a1(t1); a2(t2); a3(t3); }
            bool exec() { return comm( a1(), a2(), a3() ); }
            bool check() const { return cond( a1(), a2(), a3() ); }

            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                comm = boost::bind<bool>( boost::mem_fn(f), t, _1, _2, _3 ); // allocates
                cond = quickbind<C,ObjectType>( c, t); // allocates
            }

            boost::function<ToBind> command() const {return comm;}
            boost::function<ToBind> condition() const {return cond;}
        };

        template<class ToBind>
        struct BindStorageImpl<4, ToBind>
        {
            typedef bool result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;
            typedef typename boost::function_traits<ToBind>::arg4_type   arg4_type;

            // stores the original function pointer
            boost::function<ToBind> comm;
            boost::function<ToBind> cond;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;
            mutable AStore<arg4_type> a4;

            void store(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4) { a1(t1); a2(t2); a3(t3); a4(t4); }
            bool exec() { return comm( a1(), a2(), a3(), a4() ); }
            bool check() const { return cond( a1(), a2(), a3(), a4() ); }

            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                comm = boost::bind<bool>( boost::mem_fn(f), t, _1, _2, _3, _4 ); // allocates
                cond = quickbind<C,ObjectType>( c, t); // allocates
            }

            boost::function<ToBind> command() const {return comm;}
            boost::function<ToBind> condition() const {return cond;}
        };


        /**
         * A helper-class for the Command implementation which stores the
         * command and condition function objects. It can store both
         * pointers to member functions (with their object pointer) and
         * plain C functions.
         */
        template<class ToBind>
        struct BindStorage
            : public BindStorageImpl<boost::function_traits<ToBind>::arity, ToBind>
        {
        };

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
                return boost::bind( boost::mem_fn(m), o, _1 );
            }
        };

        template<class F>
        struct MethodBinderImpl<2,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, _1, _2 );
            }
        };

        template<class F>
        struct MethodBinderImpl<3,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, _1, _2, _3 );
            }
        };

        template<class F>
        struct MethodBinderImpl<4,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( boost::mem_fn(m), o, _1, _2, _3, _4 );
            }
        };

        template<class F>
        struct MethodBinder
            : public MethodBinderImpl<boost::function_traits<F>::arity, F>
        {};
    }
}
#endif
