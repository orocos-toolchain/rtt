#ifndef ORO_TASK_COMMAND_BASE_HPP
#define ORO_TASK_COMMAND_BASE_HPP

#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_type_arity.hpp>
#include "DispatchInterface.hpp"
#include "corelib/ActionInterface.hpp"
#include "NA.hpp"

namespace ORO_Execution
{
    namespace detail
    {
        /**
         * A class which converts a function type signature \a F
         * to a virtual operator()() interface.
         */
        template<int, class F, class R>
        struct InvokerBaseImpl;

        template<class F, class R>
        struct InvokerBaseImpl<0,F,R>
        {
            virtual R operator()() = 0;
        };

        template<class F, class R>
        struct InvokerBaseImpl<1,F,R>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            virtual R operator()(arg1_type a1) = 0;
        };

        template<class F, class R>
        struct InvokerBaseImpl<2,F,R>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            virtual R operator()(arg1_type a1, arg2_type a2) = 0;
        };

        template<class F, class R>
        struct InvokerBaseImpl<3,F,R>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            virtual R operator()(arg1_type a1, arg2_type a2, arg3_type a3) = 0;
        };

        template<class F, class R>
        struct InvokerBaseImpl<4,F,R>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            typedef typename boost::function<F>::arg4_type arg4_type;
            virtual R operator()(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) = 0;
        };

        template<class F>
        struct InvokerBase
            : public InvokerBaseImpl<boost::function_traits<F>::arity, F, typename boost::function_traits<F>::result_type>
        {};

        

        /**
         * The base class for all command implementations. Both local and remove
         * command implementations must inherit from this class.
         */
        template<class F>
        struct CommandBase
            : public InvokerBaseImpl<boost::function_traits<F>::arity, F, typename boost::function_traits<F>::result_type>,
              public DispatchInterface
        {
            virtual ~CommandBase() {}
            virtual CommandBase<F>* clone() const = 0;
        };

        /**
         * The base class for all method implementations. Both local and remove
         * method implementations must inherit from this class.
         */
        template<class F>
        struct MethodBase
            : public InvokerBaseImpl<boost::function_traits<F>::arity, F, typename boost::function_traits<F>::result_type>,
              public ORO_CoreLib::ActionInterface
        {
            virtual ~MethodBase() {}
            virtual MethodBase<F>* clone() const = 0;
        };

        template<int, class F, class BaseImpl>
        struct InvokerImpl;
        
        template<class F, class BaseImpl>
        struct InvokerImpl<0,F,BaseImpl>
            : public InvokerBase<F>, public BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            /**
             * Invoke this operator if the method has no arguments.
             */
            result_type operator()()
            {
                return BaseImpl::operator()();
            }
        };

        template<class F, class BaseImpl>
        struct InvokerImpl<1,F,BaseImpl>
            : public InvokerBase<F>, public BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            /**
             * Invoke this operator if the method has one argument.
             */
            result_type operator()(arg1_type a1)
            {
                return BaseImpl::template operator()<arg1_type>( a1 );
            }
        };

        template<class F, class BaseImpl>
        struct InvokerImpl<2,F,BaseImpl>
            : public InvokerBase<F>, public BaseImpl
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;

            /**
             * Invoke this operator if the method has two arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2)
            {
                return BaseImpl::template operator()<arg1_type, arg2_type>(t1, t2);
            }

        };

        template<class F, class BaseImpl>
        struct InvokerImpl<3,F,BaseImpl>
            : public InvokerBase<F>, public BaseImpl
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
                return BaseImpl::template operator()<arg1_type, arg2_type, arg3_type>(t1, t2, t3);
            }

        };

        template<class F, class BaseImpl>
        struct InvokerImpl<4,F,BaseImpl>
            : public InvokerBase<F>, public BaseImpl
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
                return BaseImpl::template operator()<arg1_type, arg2_type, arg3_type, arg4_type>(t1, t2, t3, t4);
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

            InvokerSignature() : impl(0) {}
            InvokerSignature(ToInvoke* implementation) : impl(implementation) {}
            ~InvokerSignature() { delete impl; }
            InvokerSignature(const InvokerSignature& c) : impl( c.impl ? c.impl->clone() : 0) {}
            InvokerSignature& operator-(const InvokerSignature& c) {
                if ( impl == c.impl )
                    return *this; // self assignment or null.
                impl = c.impl ? c.impl->clone() : 0;
                return *this;
            }

            /**
             * Invoke this operator if the method has no arguments.
             */
            result_type operator()()
            {
                if (impl)
                    return (*impl)();
                return NA<result_type>::na;
            }
        protected:
            ToInvoke* impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<1,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;

            InvokerSignature() : impl(0) {}
            InvokerSignature(ToInvoke* implementation) : impl(implementation) {}
            ~InvokerSignature() { delete impl; }
            InvokerSignature(const InvokerSignature& c) : impl( c.impl ? c.impl->clone() : 0) {}
            InvokerSignature& operator-(const InvokerSignature& c) {
                if ( impl == c.impl )
                    return *this; // self assignment or null.
                impl = c.impl ? c.impl->clone() : 0;
                return *this;
            }


            /**
             * Invoke this operator if the method has one argument.
             */
            result_type operator()(arg1_type a1)
            {
                if (impl)
                    return (*impl)( a1 );
                return NA<result_type>::na;
            }
        protected:
            ToInvoke* impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<2,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;

            InvokerSignature() : impl(0) {}
            InvokerSignature(ToInvoke* implementation) : impl(implementation) {}
            ~InvokerSignature() { delete impl; }
            InvokerSignature(const InvokerSignature& c) : impl( c.impl ? c.impl->clone() : 0) {}
            InvokerSignature& operator-(const InvokerSignature& c) {
                if ( impl == c.impl )
                    return *this; // self assignment or null.
                impl = c.impl ? c.impl->clone() : 0;
                return *this;
            }

            /**
             * Invoke this operator if the method has two arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2)
            {
                if (impl)
                    return (*impl)(t1, t2);
                return NA<result_type>::na;
            }

        protected:
            ToInvoke* impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<3,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;

            InvokerSignature() : impl(0) {}
            InvokerSignature(ToInvoke* implementation) : impl(implementation) {}
            ~InvokerSignature() { delete impl; }
            InvokerSignature(const InvokerSignature& c) : impl( c.impl ? c.impl->clone() : 0) {}
            InvokerSignature& operator-(const InvokerSignature& c) {
                if ( impl == c.impl )
                    return *this; // self assignment or null.
                impl = c.impl ? c.impl->clone() : 0;
                return *this;
            }

            /**
             * Invoke this operator if the method has three arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2, arg3_type t3)
            {
                if (impl)
                    return (*impl)(t1, t2, t3);
                return NA<result_type>::na;
            }

        protected:
            ToInvoke* impl;
        };

        template<class F, class ToInvoke>
        struct InvokerSignature<4,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;

            InvokerSignature() : impl(0) {}
            InvokerSignature(ToInvoke* implementation) : impl(implementation) {}
            ~InvokerSignature() { delete impl; }
            InvokerSignature(const InvokerSignature& c) : impl( c.impl ? c.impl->clone() : 0) {}
            InvokerSignature& operator-(const InvokerSignature& c) {
                if ( impl == c.impl )
                    return *this; // self assignment or null.
                impl = c.impl ? c.impl->clone() : 0;
                return *this;
            }

            /**
             * Invoke this operator if the method has four arguments.
             */
            result_type operator()(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4)
            {
                if (impl)
                    return (*impl)(t1, t2, t3, t4);
                return NA<result_type>::na;
            }
            
        protected:
            ToInvoke* impl;
        };

    }
}
#endif
