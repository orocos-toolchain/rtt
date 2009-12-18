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
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/parameter_types.hpp>
#include "quickbind.hpp"

namespace RTT
{
    namespace internal
    {
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

#if 0
        template<class T>
        struct AStore<const T &>
        {
            const T* arg;
            AStore() : arg(0) {}
            AStore(const T& t) : arg(&t) {}

            const T& operator()() { return *arg; }
            void operator()(const T& a) { arg = &a; }
        };
#endif

        /**
         * Store a return value which may be a void, reference, const reference or
         * any other type.
         */
        template<class T>
        struct RStore {
            T arg;
            RStore() : arg() {}

            /**
             * Stores the result of a function.
             * The RStore<void> specialisation will not store anything, and
             * just call f().
             * @param f The function object to execute and store the results from
             */
            template<class F>
            void store(F f) {
              arg = f();
            }

            /**
             * Call Collect function f with the provided arguments.
             *
             * @param f A function object with the signature as defined above which
             * receives the return value (if any) and arguments (if any) which were
             * stored in store().
             */
            template<class F>
            void collect(F f) {
              f(arg);
            }

            template<class F,class A1>
            void collect(F f, A1 a1) {
              f(arg, a1);
            }
            template<class F,class A1, class A2>
            void collect(F f, A1 a1, A2 a2) {
              f(arg,a1,a2);
            }
            template<class F,class A1, class A2, class A3>
            void collect(F f, A1 a1, A2 a2, A3 a3) {
              f(arg,a1,a2,a3);
            }
            template<class F,class A1, class A2, class A3, class A4>
            void collect(F f, A1 a1, A2 a2, A3 a3, A4 a4) {
              f(arg,a1,a2,a3,a4);
            }
        };

        template<class T>
        struct RStore<T&>
        {
            T* arg;
            RStore() : arg(0) {}

            template<class F>
            void store(F f) {
              arg = f();
            }

            template<class F>
            void collect(F f) {
              f(arg);
            }

            template<class F,class A1>
            void collect(F f, A1 a1) {
              f(*arg, a1);
            }
            template<class F,class A1, class A2>
            void collect(F f, A1 a1, A2 a2) {
              f(*arg,a1,a2);
            }
            template<class F,class A1, class A2, class A3>
            void collect(F f, A1 a1, A2 a2, A3 a3) {
              f(*arg,a1,a2,a3);
            }
            template<class F,class A1, class A2, class A3, class A4>
            void collect(F f, A1 a1, A2 a2, A3 a3, A4 a4) {
              f(*arg,a1,a2,a3,a4);
            }
        };

        template<>
        struct RStore<void> {
          RStore() {}

          template<class Signature>
          struct CollectSignature {
              typedef Signature type;
          };

          template<class F>
          void store(F f) {
            f();
          }
          template<class F>
          void collect(F f) {
            f();
          }
          template<class F,class A1>
          void collect(F f, A1 a1) {
            f(a1);
          }
          template<class F,class A1, class A2>
          void collect(F f, A1 a1, A2 a2) {
            f(a1,a2);
          }
          template<class F,class A1, class A2, class A3>
          void collect(F f, A1 a1, A2 a2, A3 a3) {
            f(a1,a2,a3);
          }
          template<class F,class A1, class A2, class A3, class A4>
          void collect(F f, A1 a1, A2 a2, A3 a3, A4 a4) {
            f(a1,a2,a3,a4);
          }
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
            typedef typename CollectType<ToBind>::type Collect;

            mutable RStore<result_type> ret;
            // stores the original function pointer

            boost::function<ToBind> mcomm;
            boost::function<Collect> mcoll;

            void exec() { ret.store( mcomm ); }
            void coll() { ret.collect( mcoll() ); }

            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                mcomm = quickbind<F,ObjectType>( f, t); // allocates
                mcoll  = quickbind<C,ObjectType>( c, t); // allocates
            }

            template<class F, class C>
            void setup(F f, C c)
            {
                mcomm = f;
                mcoll = c;
            }

            void setup(boost::function<ToBind> f, boost::function<Collect> c)
            {
                mcomm = f;
                mcoll = c;
            }
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
            boost::function<ToBind>  coll;
            // Store the argument.
            mutable AStore<arg1_type> a1;
            mutable RStore<result_type> ret;

            void store(arg1_type t1) { a1(t1); }
            void exec() { ret.store( boost::bind(comm, boost::ref(a1()) ) ); }
            void collect() { ret.collect( coll, a1() ) ; }

            /**
             * The object already stores the user class function pointer and
             * the pointer to the class object.
             */
            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                comm = quickbind<F,ObjectType>( f, t); // allocates
                coll = quickbind<C,ObjectType>( c, t); // allocates
            }

            template<class F, class C>
            void setup(F f, C c)
            {
                comm = quickbindC<F>(f); // allocates
                coll = quickbindC<C>(c);
            }

            void setup(boost::function<ToBind> f, boost::function<ToBind> c)
            {
                comm = f;
                coll = c;
            }

        };

        template<class ToBind>
        struct BindStorageImpl<2, ToBind>
        {
            typedef bool result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;

            // stores the original function pointer
            boost::function<ToBind> comm;
            boost::function<ToBind> coll;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;

            void store(arg1_type t1, arg2_type t2) { a1(t1); a2(t2); }
            bool exec() { return comm( a1(), a2() ); }
            bool collect() const { return coll( a1(), a2() ); }

            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                comm = boost::bind<bool>( boost::mem_fn(f), t, _1, _2 ); // allocates
                coll = quickbind<C,ObjectType>( c, t); // allocates
            }

            template<class F, class C>
            void setup(F f, C c)
            {
                comm = boost::bind<bool>( f, _1, _2 ); // allocates
                coll = quickbindC<C>(c);
            }

            void setup(boost::function<ToBind> f, boost::function<ToBind> c)
            {
                comm = f;
                coll = c;
            }

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
            boost::function<ToBind> coll;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;

            void store(arg1_type t1, arg2_type t2, arg3_type t3) { a1(t1); a2(t2); a3(t3); }
            bool exec() { return comm( a1(), a2(), a3() ); }
            bool collect() const { return coll( a1(), a2(), a3() ); }

            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                comm = boost::bind<bool>( boost::mem_fn(f), t, _1, _2, _3 ); // allocates
                coll = quickbind<C,ObjectType>( c, t); // allocates
            }
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
            boost::function<ToBind> coll;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;
            mutable AStore<arg4_type> a4;

            void store(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4) { a1(t1); a2(t2); a3(t3); a4(t4); }
            bool exec() { return comm( a1(), a2(), a3(), a4() ); }
            bool collect() const { return coll( a1(), a2(), a3(), a4() ); }

            template<class F, class C, class ObjectType>
            void setup(F f, C c, ObjectType t)
            {
                comm = boost::bind<bool>( boost::mem_fn(f), t, _1, _2, _3, _4 ); // allocates
                coll = quickbind<C,ObjectType>( c, t); // allocates
            }

        };


        /**
         * A helper-class for the Command implementation which stores the
         * command and collition function objects. It can store both
         * pointers to member functions (with their object pointer) and
         * plain C functions.
         *
         * There is no constructor and all functions are
         * implemented in the BindStorageImpl base classes.
         *
         * First create the BindStorage object,
         * then use setup() to register the two functions. Next store()
         * stores the arguments to the functions and exec() executes the
         * command function from setup with the stored arguments.
         *
         * @param ToBind A function signature (like: 'bool(int)') for which
         * two functions can be bound (command and collection fcn).
         *
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
