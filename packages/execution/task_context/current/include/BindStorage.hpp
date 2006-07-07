#ifndef ORO_TASK_BIND_STORAGE_HPP
#define ORO_TASK_BIND_STORAGE_HPP

#include <boost/function.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_type_arity.hpp>
#include <boost/bind.hpp>

namespace ORO_Execution
{
    namespace detail
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
            bool operator()(T1 t1) {
                return (mo->*mf)(t1);
            }

            template<class T1, class T2>
            bool operator()(T1 t1, T2) {
                return (mo->*mf)(t1);
            }

            template<class T1, class T2, class T3>
            bool operator()(T1 t1, T2, T3) {
                return (mo->*mf)(t1);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1 t1, T2, T3, T4) {
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
            bool operator()(T1 t1, T2 t2) {
                return (mo->*mf)(t1,t2);
            }

            template<class T1, class T2, class T3>
            bool operator()(T1 t1, T2 t2, T3) {
                return (mo->*mf)(t1,t2);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1 t1, T2 t2, T3, T4) {
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
            bool operator()(T1 t1, T2 t2, T3 t3) {
                return (mo->*mf)(t1,t2,t3);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1 t1, T2 t2, T3 t3, T4) {
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
            bool operator()(T1 t1, T2 t2, T3 t3, T4 t4) {
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
            bool operator()(T1 t1) {
                return mf(t1);
            }

            template<class T1, class T2>
            bool operator()(T1 t1, T2) {
                return mf(t1);
            }

            template<class T1, class T2, class T3>
            bool operator()(T1 t1, T2, T3) {
                return mf(t1);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1 t1, T2, T3, T4) {
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
            bool operator()(T1 t1, T2 t2) {
                return mf(t1,t2);
            }

            template<class T1,class T2, class T3>
            bool operator()(T1 t1, T2 t2, T3) {
                return mf(t1,t2);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1 t1, T2 t2, T3, T4) {
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
            bool operator()(T1 t1, T2 t2, T3 t3) {
                return mf(t1,t2,t3);
            }

            template<class T1, class T2, class T3, class T4>
            bool operator()(T1 t1, T2 t2, T3 t3, T4) {
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
            bool operator()(T1 t1, T2 t2, T3 t3, T4 t4) {
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
            boost::function<ToBind> invoke;
            boost::function<ToBind> check;

            template<class F, class C, class ObjectType>
            BindStorageImpl(F f, C c, ObjectType t)
                :
                invoke(boost::bind<bool>( f, t ) ), // allocates
                check(boost::bind<bool>( c, t) )    // allocates
            {}

            template<class F, class C>
            BindStorageImpl(F f, C c)
                :
                invoke(f), // allocates
                check(c)    // allocates
            {}

            BindStorageImpl(boost::function<ToBind> f, boost::function<ToBind> c)
                :
                invoke(f), // allocates
                check(c)    // allocates
            {}

            boost::function<ToBind> command() const {return invoke;}
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
            // stores the bound function pointer (ie comm+its arguments),
            // in order to invoke comm(arguments) when the command is processed.
            boost::_bi::bind_t<result_type, boost::function<ToBind>, boost::_bi::list1<boost::_bi::value<arg1_type> > > invoke;

            // Wrap the condition.
            boost::function<ToBind> cond;
            // stores the condition pointer + arguments in order to check
            // the completion condition.
            boost::_bi::bind_t<result_type, boost::function<ToBind>, boost::_bi::list1<boost::_bi::value<arg1_type> > > check;

            /**
             * The object already stores the user class function pointer and
             * the pointer to the class object.
             * 'invoke' is initialised with a dummy load. It may be 
             * overwritten with comm(arguments) in real-time. Next, use
             * invoke() to effectively call comm(arguments) on a later time.
             */
            template<class F, class C, class ObjectType>
            BindStorageImpl(F f, C c, ObjectType t)
                :
                comm(boost::bind<bool>( f, t, _1 ) ), // allocates
                invoke(boost::bind<result_type>( comm, arg1_type() )),
                cond( quickbind<C,ObjectType>( c, t) ), // allocates
                check(boost::bind<result_type>( cond, arg1_type() ))
            {}

            template<class F, class C>
            BindStorageImpl(F f, C c)
                :
                comm(boost::bind<bool>( f, _1 ) ), // allocates
                invoke(boost::bind<result_type>( comm, arg1_type() )),
                cond( quickbindC<C>(c) ),
                check(boost::bind<result_type>( cond, arg1_type() ))
            {}

            BindStorageImpl(boost::function<ToBind> f, boost::function<ToBind> c)
                :
                comm(f),
                invoke(boost::bind<result_type>( comm, arg1_type() )),
                cond(c),
                check(boost::bind<result_type>( cond, arg1_type() ))
            {}

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
            boost::_bi::bind_t<result_type, boost::function<ToBind>, boost::_bi::list2<boost::_bi::value<arg1_type>,boost::_bi::value<arg2_type> > > invoke;
            boost::function<ToBind> cond;
            boost::_bi::bind_t<result_type, boost::function<ToBind>, boost::_bi::list2<boost::_bi::value<arg1_type>,boost::_bi::value<arg2_type> > > check;

            template<class F, class C, class ObjectType>
            BindStorageImpl(F f, C c, ObjectType t)
                :
                comm(boost::bind<bool>( f, t, _1, _2 ) ), // allocates
                cond( quickbind<C,ObjectType>(c,t) ),
                invoke(boost::bind<result_type>( comm, arg1_type(), arg2_type() )),
                check(boost::bind<result_type>( cond, arg1_type(), arg2_type() ))
            {}

            template<class F, class C>
            BindStorageImpl(F f, C c)
                :
                comm(boost::bind<bool>( f, _1, _2 ) ), // allocates
                cond( quickbindC<C>(c) ),
                invoke(boost::bind<result_type>( comm, arg1_type(), arg2_type() )),
                check(boost::bind<result_type>( cond, arg1_type(), arg2_type() ))
            {}

            BindStorageImpl(boost::function<ToBind> f, boost::function<ToBind> c)
                :
                comm( f ),
                cond( c ),
                invoke(boost::bind<result_type>( comm, arg1_type(), arg2_type() )),
                check(boost::bind<result_type>( cond, arg1_type(), arg2_type() ))
            {}

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
            boost::_bi::bind_t<result_type, boost::function<ToBind>, boost::_bi::list3<boost::_bi::value<arg1_type>,boost::_bi::value<arg2_type>,boost::_bi::value<arg3_type> > > invoke;
            boost::function<ToBind> cond;
            boost::_bi::bind_t<result_type, boost::function<ToBind>, boost::_bi::list3<boost::_bi::value<arg1_type>,boost::_bi::value<arg2_type>,boost::_bi::value<arg3_type> > > check;

            template<class F, class C, class ObjectType>
            BindStorageImpl(F f, C c, ObjectType t)
                :
                comm(boost::bind<bool>( f, t, _1, _2, _3 ) ), // allocates
                cond( quickbind<C,ObjectType>(c,t) ),
                invoke(boost::bind<result_type>( comm, arg1_type(), arg2_type(), arg3_type() )),
                check(boost::bind<result_type>( cond, arg1_type(), arg2_type(), arg3_type() ))
            {}

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
            boost::_bi::bind_t<result_type, boost::function<ToBind>, boost::_bi::list4<boost::_bi::value<arg1_type>,boost::_bi::value<arg2_type>,boost::_bi::value<arg3_type>,boost::_bi::value<arg4_type> > > invoke;
            boost::function<ToBind> cond;
            boost::_bi::bind_t<result_type, boost::function<ToBind>, boost::_bi::list4<boost::_bi::value<arg1_type>,boost::_bi::value<arg2_type>,boost::_bi::value<arg3_type>,boost::_bi::value<arg4_type> > > check;

            template<class F, class C, class ObjectType>
            BindStorageImpl(F f, C c, ObjectType t)
                :
                comm(boost::bind<bool>( f, t, _1, _2, _3, _4 ) ), // allocates
                cond( quickbind<C,ObjectType>(c,t) ),
                invoke(boost::bind<result_type>( comm, arg1_type(), arg2_type(), arg3_type(), arg4_type() )),
                check(boost::bind<result_type>( cond, arg1_type(), arg2_type(), arg3_type(), arg4_type() ))
            {}

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
            template<class F, class C, class ObjectType>
            BindStorage(F f, C c, ObjectType t)
                : BindStorageImpl<boost::function_traits<ToBind>::arity, ToBind>( f, c, t)
            {}

            template<class F, class C>
            BindStorage(F f, C c)
                : BindStorageImpl<boost::function_traits<ToBind>::arity, ToBind>( f, c)
            {}

            BindStorage(boost::function<ToBind> f, boost::function<ToBind> c)
                : BindStorageImpl<boost::function_traits<ToBind>::arity, ToBind>( f, c)
            {}
        };
    }
}
#endif
