
#ifndef ORO_TASK_MESSAGE_STORAGE_HPP
#define ORO_TASK_MESSAGE_STORAGE_HPP

#include "BindStorage.hpp"

namespace RTT
{
    namespace detail
    {

        /**
         * Store a bound argument which may be a reference, const reference or
         * any other type.
         */
        template<class T>
        struct MStore {
            T arg;
            MStore() : arg() {}
            MStore(T t) : arg(t) {}

            T operator()() { return arg; }
            void operator()(T a) { arg = a; }
        };

        template<class T>
        struct MStore<T&>
        {
            T* arg;
            MStore() : arg(0) {}
            MStore(T& t) : arg(&t) {}

            T& operator()() { return *arg; }
            void operator()(T& a) { arg = &a; }
        };

        template<class T>
        struct MStore<const T &>
        {
            const T* arg;
            MStore() : arg(0) {}
            MStore(const T& t) : arg(&t) {}

            const T& operator()() { return *arg; }
            void operator()(const T& a) { arg = &a; }
        };

        template<int, class T>
        struct MessageStorageImpl;

        /**
         * When no arguments are to be stored, the
         * implementation stores the function in a boost::function object.
         */
        template<class ToBind>
        struct MessageStorageImpl<0, ToBind>
        {
            typedef bool result_type;

            // stores the original function pointer
            boost::function<ToBind> exec;

            template<class F, class ObjectType>
            void setup(F f, ObjectType t)
            {
                exec = quickbind<F,ObjectType>( f, t); // allocates
            }

            template<class F>
            void setup(F f)
            {
                exec = f;
            }

            void setup(boost::function<ToBind> f)
            {
                exec = f;
            }

            boost::function<ToBind> function() const {return exec;}
        };

        /**
         * Stores a pointer to a function and its arguments.
         */
        template<class ToBind>
        struct MessageStorageImpl<1, ToBind>
        {
            typedef void result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;

            // stores the original function pointer, supplied by the user.
            boost::function<ToBind>  mesg;
            // Store the argument.
            mutable MStore<arg1_type> a1;

            void store(arg1_type t1) { a1(t1); }
            void exec() { mesg( a1() ); }

            /**
             * The object already stores the user class function pointer and
             * the pointer to the class object.
             */
            template<class F, class ObjectType>
            void setup(F f, ObjectType t)
            {
                mesg = quickbind<F,ObjectType>( f, t); // allocates
            }

            template<class F>
            void setup(F f)
            {
                mesg = quickbindC<F>(f); // allocates
            }

            void setup(boost::function<ToBind> f)
            {
                mesg = f;
            }

            boost::function<ToBind> function() const {return mesg;}
        };

        template<class ToBind>
        struct MessageStorageImpl<2, ToBind>
        {
            typedef bool result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;

            // stores the original function pointer
            boost::function<ToBind> mesg;

            // Store the arguments.
            mutable MStore<arg1_type> a1;
            mutable MStore<arg2_type> a2;

            void store(arg1_type t1, arg2_type t2) { a1(t1); a2(t2); }
            bool exec() { return mesg( a1(), a2() ); }

            template<class F, class ObjectType>
            void setup(F f, ObjectType t)
            {
                mesg = boost::bind<bool>( boost::mem_fn(f), t, _1, _2 ); // allocates
            }

            template<class F>
            void setup(F f)
            {
                mesg = boost::bind<bool>( f, _1, _2 ); // allocates
            }

            void setup(boost::function<ToBind> f)
            {
                mesg = f;
            }

            boost::function<ToBind> function() const {return mesg;}
        };

        template<class ToBind>
        struct MessageStorageImpl<3, ToBind>
        {
            typedef bool result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;

            // stores the original function pointer
            boost::function<ToBind> mesg;
            // Store the arguments.
            mutable MStore<arg1_type> a1;
            mutable MStore<arg2_type> a2;
            mutable MStore<arg3_type> a3;

            void store(arg1_type t1, arg2_type t2, arg3_type t3) { a1(t1); a2(t2); a3(t3); }
            bool exec() { return mesg( a1(), a2(), a3() ); }

            template<class F, class ObjectType>
            void setup(F f, ObjectType t)
            {
                mesg = boost::bind<bool>( boost::mem_fn(f), t, _1, _2, _3 ); // allocates
            }

            boost::function<ToBind> function() const {return mesg;}
        };

        template<class ToBind>
        struct MessageStorageImpl<4, ToBind>
        {
            typedef bool result_type;
            typedef typename boost::function_traits<ToBind>::arg1_type   arg1_type;
            typedef typename boost::function_traits<ToBind>::arg2_type   arg2_type;
            typedef typename boost::function_traits<ToBind>::arg3_type   arg3_type;
            typedef typename boost::function_traits<ToBind>::arg4_type   arg4_type;

            // stores the original function pointer
            boost::function<ToBind> mesg;
            // Store the arguments.
            mutable MStore<arg1_type> a1;
            mutable MStore<arg2_type> a2;
            mutable MStore<arg3_type> a3;
            mutable MStore<arg4_type> a4;

            void store(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4) { a1(t1); a2(t2); a3(t3); a4(t4); }
            bool exec() { return mesg( a1(), a2(), a3(), a4() ); }

            template<class F, class ObjectType>
            void setup(F f, ObjectType t)
            {
                mesg = boost::bind<bool>( boost::mem_fn(f), t, _1, _2, _3, _4 ); // allocates
            }

            boost::function<ToBind> function() const {return mesg;}
        };


        /**
         * A helper-class for the Message implementation which stores the
         * command and condition function objects. It can store both
         * pointers to member functions (with their object pointer) and
         * plain C functions.
         *
         * There is no constructor and all functions are
         * implemented in the MessageStorageImpl base classes.
         *
         * First create the MessageStorage object,
         * then use setup() to register the two functions. Next store()
         * stores the arguments to the functions and exec() executes the
         * command function from setup with the stored arguments.
         *
         * @param ToBind A function signature (like: 'bool(int)') for which
         * two functions can be bound (command and condition fcn).
         *
         */
        template<class ToBind>
        struct MessageStorage
            : public MessageStorageImpl<boost::function_traits<ToBind>::arity, ToBind>
        {
        };

    }
}
#endif
