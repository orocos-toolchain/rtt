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
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/filter_if.hpp>
#include "NA.hpp"
#include "../Logger.hpp"

#ifdef ORO_SIGNALLING_OPERATIONS
#include "Signal.hpp"
#endif

namespace RTT
{
    namespace internal
    {
        namespace bf=boost::fusion;
        namespace mpl=boost::mpl;
        /**
         * Store a bound argument which may be a reference, const reference or
         * any other type.
         */
        template<class T>
        struct AStore
        {
            typedef T arg_type;
            T arg;
            AStore() : arg() {}
            AStore(T t) : arg(t) {}
            AStore(AStore const& o) : arg(o.arg) {}

            T& get() { return arg; }
            void operator()(T a) { arg = a; }
            operator T&() { return arg; }
        };

        template<class T>
        struct AStore<T&>
        {
            typedef T& arg_type;
            T* arg;
            AStore() : arg( &NA<T&>::na() ) {}
            AStore(T& t) : arg(&t) {}
            AStore(AStore const& o) : arg(o.arg) {}

            T& get() { return *arg; }
            void operator()(T& a) { arg = &a; }
            operator T&() { return *arg; }
        };

        template<class T>
        std::ostream& operator<<(std::ostream& o, AStore<T>& a) { o << "aarg:"<<a.get(); return o;}

        template<>
        struct RStore<void> {
            bool executed;
            bool error;
            RStore() : executed(false), error(false) {}

            void checkError() const {
              if(error) throw std::runtime_error("Unable to complete the operation call. The called operation has thrown an exception");
            }

            bool isError() const {
              return error;
            }

            bool isExecuted() const {
                return executed;
            }

            template<class F>
            void exec(F f) {
                error = false;
                try{
                    f();
                } catch (std::exception& e) {
                    log(Error) << "Exception raised while executing an operation : "  << e.what() << endlog();
                    error = true;
                } catch (...) {
                    log(Error) << "Unknown exception raised while executing an operation." << endlog();
                    error = true;
                }
                executed = true;
            }

            void result() { checkError(); return; }
        };


        /**
         * Store a return value which may be a void, reference, const reference or
         * any other type. We need these specialisations because the collection
         * of the results will be different if R is non-void or poid (appears as
         * first arg of collect() or not respectively). So RStore is the only
         * instance that knows if a return value was stored or not.
         */
        template<class T>
        struct RStore : public RStore<void> {
            T arg;
            RStore() : arg() {}

            T& result() { checkError(); return arg; }
            operator T&() { return arg;}

            /**
             * Stores the result of a function.
             * The RStore<void> specialisation will not store anything, and
             * just call f().
             * @param f The function object to execute and store the results from
             */
            template<class F>
            void exec(F f) {
                error = false;
                try{
                    arg = f();
                } catch (std::exception& e) {
                    log(Error) << "Exception raised while executing an operation : "  << e.what() << endlog();
                    error = true;
                } catch (...) {
                    log(Error) << "Unknown exception raised while executing an operation." << endlog();
                    error = true;
                }
                executed = true;
            }
        };

        template<class T>
        struct RStore<T&> : public RStore<void>
        {
            T* arg;
            RStore() : arg() {}

            template<class F>
            void exec(F f) {
                error = false;
                try{
                    arg = &f();
                } catch (std::exception& e) {
                    log(Error) << "Exception raised while executing an operation : "  << e.what() << endlog();
                    error = true;
                } catch (...) {
                    log(Error) << "Unknown exception raised while executing an operation." << endlog();
                    error = true;
                }
                executed = true;
            }

            //bool operator()() { return executed; }

            T& result() { checkError(); return *arg; }
            operator T&() { checkError(); return *arg;}
        };

        template<class T>
        struct RStore<const T> : public RStore<void> {
            T arg;
            RStore() : arg() {}

            T& result() { checkError(); return arg; }
            operator T&() { checkError(); return arg;}

            /**
             * Stores the result of a function.
             * The RStore<void> specialisation will not store anything, and
             * just call f().
             * @param f The function object to execute and store the results from
             */
            template<class F>
            void exec(F f) {
                error = false;
                try{
                    arg = f();
                } catch (std::exception& e) {
                    log(Error) << "Exception raised while executing an operation : "  << e.what() << endlog();
                    error = true;
                } catch(...) {
                    log(Error) << "Unknown exception raised while executing an operation." << endlog();
                    error = true;
                }
                executed = true;
            }

        };

        template<class T>
        std::ostream& operator<<(std::ostream& o, RStore<T>& a) { o << "rarg:"<<a.result(); return o;}

        /**
         * This helper struct is required to filter out the AStore
         * elements that don't need to be returned to the user when
         * collect or return is called. We only return AStore elements
         * that store a pure reference and RStore elements that are not void.
         * @see LocalOperationCallerImpl
         */
        template<class Arg>
        struct is_arg_return : public mpl::false_ {};

        template<class T>
        struct is_arg_return<AStore<T&> > : public mpl::true_
        {};

        template<class T>
        struct is_arg_return<AStore<T const &> > : public mpl::false_
        {};

        template<>
        struct is_arg_return<RStore<void> > : public mpl::false_
        {};

        template<class T>
        struct is_arg_return<RStore<T> > : public mpl::true_
        {};

        /**
         * Outargs are of type AStore and contain a pure reference.
         */
        template<class Arg>
        struct is_out_arg : public mpl::false_ {};

        template<class T>
        struct is_out_arg<AStore<T&> > : public mpl::true_
        {};

        template<class T>
        struct is_out_arg<AStore<T const &> > : public mpl::false_
        {};

        template<int, class T>
        struct BindStorageImpl;

        /**
         * When no arguments are to be stored, the
         * implementation stores the function in a boost::function object.
         */
        template<class ToBind>
        struct BindStorageImpl<0, ToBind>
        {
            typedef typename boost::function_traits<ToBind>::result_type result_type;
            typedef RStore<result_type> RStoreType;

            // stores the original function pointer
            boost::function<ToBind> mmeth;
            mutable RStore<result_type> retv;
            // the list of all our storage.
            bf::vector< RStore<result_type>&> vStore;
#ifdef ORO_SIGNALLING_OPERATIONS
            typename Signal<ToBind>::shared_ptr msig;
#endif

            BindStorageImpl() :  vStore(retv) {}
            BindStorageImpl(const BindStorageImpl& orig) : mmeth(orig.mmeth), vStore(retv)
#ifdef ORO_SIGNALLING_OPERATIONS
                                                         , msig(orig.msig)
#endif
            {}

            void exec() {
#ifdef ORO_SIGNALLING_OPERATIONS
                if (msig) msig->emit();
#endif
                if (mmeth)
                    retv.exec( mmeth );
                else
                    retv.executed = true;
            }
        };

        /**
         * Stores a pointer to a function and its arguments.
         */
        template<class ToBind>
        struct BindStorageImpl<1, ToBind>
        {
            typedef typename boost::function_traits<ToBind>::result_type result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef RStore<result_type> RStoreType;

            // stores the original function pointer, supplied by the user.
            boost::function<ToBind>  mmeth;
            // Store the argument.
            mutable AStore<arg1_type> a1;
            mutable RStore<result_type> retv;
            // the list of all our storage.
            bf::vector< RStore<result_type>&, AStore<arg1_type>& > vStore;
#ifdef ORO_SIGNALLING_OPERATIONS
            typename Signal<ToBind>::shared_ptr msig;
#endif

            BindStorageImpl() : vStore(retv,a1) {}
            BindStorageImpl(const BindStorageImpl& orig) : mmeth(orig.mmeth), vStore(retv,a1)
#ifdef ORO_SIGNALLING_OPERATIONS
                                                         , msig(orig.msig)
#endif
            {}
            void store(arg1_type t1) { a1(t1); }
            void exec() {
#ifdef ORO_SIGNALLING_OPERATIONS
                if (msig) (*msig)(a1);
#endif
                if (mmeth)
                    retv.exec( boost::bind(mmeth, a1 ) );
                else
                    retv.executed = true;
            }

        };

        template<class ToBind>
        struct BindStorageImpl<2, ToBind>
        {
            typedef typename boost::function_traits<ToBind>::result_type result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef RStore<result_type> RStoreType;

            // stores the original function pointer
            boost::function<ToBind> mmeth;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable RStore<result_type> retv;
            // the list of all our storage.
            bf::vector< RStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>& > vStore;
#ifdef ORO_SIGNALLING_OPERATIONS
            typename Signal<ToBind>::shared_ptr msig;
#endif

            BindStorageImpl() : vStore(retv,a1,a2) {}
            BindStorageImpl(const BindStorageImpl& orig) : mmeth(orig.mmeth), vStore(retv,a1,a2)
#ifdef ORO_SIGNALLING_OPERATIONS
                                                         , msig(orig.msig)
#endif
            {}

            void store(arg1_type t1, arg2_type t2) { a1(t1); a2(t2); }
            void exec() {
#ifdef ORO_SIGNALLING_OPERATIONS
                if (msig) (*msig)(a1, a2);
#endif
                if (mmeth)
                    retv.exec( boost::bind(mmeth, a1, a2 ) );
                else
                    retv.executed = true;
            }

        };

        template<class ToBind>
        struct BindStorageImpl<3, ToBind>
        {
            typedef typename boost::function_traits<ToBind>::result_type result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;
            typedef RStore<result_type> RStoreType;

            // stores the original function pointer
            boost::function<ToBind> mmeth;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;
            mutable RStore<result_type> retv;
            // the list of all our storage.
            bf::vector< RStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>&, AStore<arg3_type>& > vStore;
#ifdef ORO_SIGNALLING_OPERATIONS
            typename Signal<ToBind>::shared_ptr msig;
#endif

            BindStorageImpl() : vStore(retv,a1,a2,a3) {}
            BindStorageImpl(const BindStorageImpl& orig) : mmeth(orig.mmeth), vStore(retv,a1,a2,a3)
#ifdef ORO_SIGNALLING_OPERATIONS
                                                         , msig(orig.msig)
#endif
            {}

            void store(arg1_type t1, arg2_type t2, arg3_type t3) { a1(t1); a2(t2); a3(t3); }
            void exec() {
#ifdef ORO_SIGNALLING_OPERATIONS
                if (msig) (*msig)(a1, a2, a3);
#endif
                if (mmeth)
                    retv.exec( boost::bind(mmeth, a1, a2, a3 ) );
                else
                    retv.executed = true;
            }
        };

        template<class ToBind>
        struct BindStorageImpl<4, ToBind>
        {
            typedef typename boost::function_traits<ToBind>::result_type result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;
            typedef typename boost::function_traits<ToBind>::arg4_type   arg4_type;
            typedef RStore<result_type> RStoreType;

            // stores the original function pointer
            boost::function<ToBind> mmeth;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;
            mutable AStore<arg4_type> a4;
            mutable RStore<result_type> retv;
            // the list of all our storage.
            bf::vector< RStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>&, AStore<arg3_type>&, AStore<arg4_type>& > vStore;
#ifdef ORO_SIGNALLING_OPERATIONS
            typename Signal<ToBind>::shared_ptr msig;
#endif

            BindStorageImpl() : vStore(retv,a1,a2,a3,a4) {}
            BindStorageImpl(const BindStorageImpl& orig) : mmeth(orig.mmeth), vStore(retv,a1,a2,a3,a4)
#ifdef ORO_SIGNALLING_OPERATIONS
                                                         , msig(orig.msig)
#endif
            {}

            void store(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4) { a1(t1); a2(t2); a3(t3); a4(t4); }
            void exec() {
#ifdef ORO_SIGNALLING_OPERATIONS
                if (msig) (*msig)(a1, a2, a3, a4);
#endif
                if (mmeth)
                    retv.exec( boost::bind( mmeth, a1, a2, a3, a4 ) );
                else
                    retv.executed = true;
            }
        };

        template<class ToBind>
        struct BindStorageImpl<5, ToBind>
        {
            typedef typename boost::function_traits<ToBind>::result_type result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;
            typedef typename boost::function_traits<ToBind>::arg4_type   arg4_type;
            typedef typename boost::function_traits<ToBind>::arg5_type   arg5_type;
            typedef RStore<result_type> RStoreType;

            // stores the original function pointer
            boost::function<ToBind> mmeth;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;
            mutable AStore<arg4_type> a4;
            mutable AStore<arg5_type> a5;
            mutable RStore<result_type> retv;
            // the list of all our storage.
            bf::vector< RStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>&, AStore<arg3_type>&, AStore<arg4_type>&, AStore<arg5_type>& > vStore;
#ifdef ORO_SIGNALLING_OPERATIONS
            typename Signal<ToBind>::shared_ptr msig;
#endif

            BindStorageImpl() : vStore(retv,a1,a2,a3,a4,a5) {}
            BindStorageImpl(const BindStorageImpl& orig) : mmeth(orig.mmeth), vStore(retv,a1,a2,a3,a4,a5)
#ifdef ORO_SIGNALLING_OPERATIONS
                                                         , msig(orig.msig)
#endif
            {}

            void store(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4, arg5_type t5) { a1(t1); a2(t2); a3(t3); a4(t4); a5(t5);}
            void exec() {
#ifdef ORO_SIGNALLING_OPERATIONS
                if (msig) (*msig)(a1, a2, a3, a4, a5);
#endif
                if (mmeth)
                    retv.exec( boost::bind( mmeth, a1, a2, a3, a4, a5 ) );
                else
                    retv.executed = true;
            }
        };

        template<class ToBind>
        struct BindStorageImpl<6, ToBind>
        {
            typedef typename boost::function_traits<ToBind>::result_type result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;
            typedef typename boost::function_traits<ToBind>::arg4_type   arg4_type;
            typedef typename boost::function_traits<ToBind>::arg5_type   arg5_type;
            typedef typename boost::function_traits<ToBind>::arg6_type   arg6_type;
            typedef RStore<result_type> RStoreType;

            // stores the original function pointer
            boost::function<ToBind> mmeth;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;
            mutable AStore<arg4_type> a4;
            mutable AStore<arg5_type> a5;
            mutable AStore<arg6_type> a6;
            mutable RStore<result_type> retv;
            // the list of all our storage.
            bf::vector< RStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>&, AStore<arg3_type>&, AStore<arg4_type>&, AStore<arg5_type>&, AStore<arg6_type>& > vStore;
#ifdef ORO_SIGNALLING_OPERATIONS
            typename Signal<ToBind>::shared_ptr msig;
#endif

            BindStorageImpl() : vStore(retv,a1,a2,a3,a4,a5,a6) {}
            BindStorageImpl(const BindStorageImpl& orig) : mmeth(orig.mmeth), vStore(retv,a1,a2,a3,a4,a5,a6) 
#ifdef ORO_SIGNALLING_OPERATIONS
                                                         , msig(orig.msig)
#endif
            {}

            void store(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4, arg5_type t5, arg6_type t6) { a1(t1); a2(t2); a3(t3); a4(t4); a5(t5); a6(t6);}
            void exec() {
#ifdef ORO_SIGNALLING_OPERATIONS
                if (msig) (*msig)(a1, a2, a3, a4, a5, a6);
#endif
                if (mmeth)
                    retv.exec( boost::bind( mmeth, a1, a2, a3, a4, a5, a6 ) );
                else
                    retv.executed = true;
            }
        };

        template<class ToBind>
        struct BindStorageImpl<7, ToBind>
        {
            typedef typename boost::function_traits<ToBind>::result_type result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;
            typedef typename boost::function_traits<ToBind>::arg4_type   arg4_type;
            typedef typename boost::function_traits<ToBind>::arg5_type   arg5_type;
            typedef typename boost::function_traits<ToBind>::arg6_type   arg6_type;
            typedef typename boost::function_traits<ToBind>::arg7_type   arg7_type;
            typedef RStore<result_type> RStoreType;

            // stores the original function pointer
            boost::function<ToBind> mmeth;
            // Store the arguments.
            mutable AStore<arg1_type> a1;
            mutable AStore<arg2_type> a2;
            mutable AStore<arg3_type> a3;
            mutable AStore<arg4_type> a4;
            mutable AStore<arg5_type> a5;
            mutable AStore<arg6_type> a6;
            mutable AStore<arg7_type> a7;
            mutable RStore<result_type> retv;
            // the list of all our storage.
            bf::vector< RStore<result_type>&, AStore<arg1_type>&, AStore<arg2_type>&, AStore<arg3_type>&, AStore<arg4_type>&, AStore<arg5_type>&, AStore<arg6_type>&, AStore<arg7_type>& > vStore;
#ifdef ORO_SIGNALLING_OPERATIONS
            typename Signal<ToBind>::shared_ptr msig;
#endif

            BindStorageImpl() : vStore(retv,a1,a2,a3,a4,a5,a6,a7) {}
            BindStorageImpl(const BindStorageImpl& orig) : mmeth(orig.mmeth), vStore(retv,a1,a2,a3,a4,a5,a6,a7)
#ifdef ORO_SIGNALLING_OPERATIONS
                                                         , msig(orig.msig)
#endif
            {}

            void store(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4, arg5_type t5, arg6_type t6, arg7_type t7) { a1(t1); a2(t2); a3(t3); a4(t4); a5(t5); a6(t6); a7(t7);}
            void exec() {
#ifdef ORO_SIGNALLING_OPERATIONS
                if (msig) (*msig)(a1, a2, a3, a4, a5, a6, a7);
#endif
                if (mmeth)
                    retv.exec( boost::bind( mmeth, a1, a2, a3, a4, a5, a6, a7 ) );
                else
                    retv.executed = true;
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
    }
}
#endif
