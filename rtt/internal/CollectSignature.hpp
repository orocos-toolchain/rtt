#ifndef ORO_COLLECT_SIGNATURE_HPP
#define ORO_COLLECT_SIGNATURE_HPP

#include <boost/function_types/function_type.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits.hpp>
#include "../SendStatus.hpp"

namespace RTT
{
    namespace internal
    {
        namespace ft=boost::function_types;
        namespace mpl=boost::mpl;
        namespace tt=boost;

        /**
         * This helper struct decomposes F and creates Ft,
         * as required by CollectBaseImpl.
         */
        template<class F>
        struct CollectType
        {
        private:
            // Decompose F into all components (ret, args,...):
            // Remove first component and store in ret_type:
            typedef typename ft::result_type<F>::type ret_type;
            typedef typename ft::parameter_types<F>::type arg_type;
            typedef typename tt::remove_const< typename tt::remove_reference<ret_type>::type >::type bare_ret_type;
            typedef typename mpl::if_<
                typename mpl::not_< typename tt::is_void<ret_type>::type >::type,
                typename mpl::push_front< arg_type, typename tt::add_reference< bare_ret_type >::type >::type,
                arg_type
                >::type basic_sig;
            // basic_sig now needs to be removed of all non-reference arguments.
            // we first with removing all consts and const references:
            // note about the lambda expression: we pass a struct, not a typenamed 'type'.
            typedef typename mpl::remove_if< basic_sig, tt::is_const< tt::remove_reference<mpl::_1> > >::type no_const_sig;
            // next we need to remove all non-reference values:
            typedef typename mpl::remove_if< no_const_sig, mpl::not_<tt::is_reference<mpl::_1> > >::type no_value_sig;
            // Finally, add ret_type as return value (first item in vector):
            typedef typename mpl::push_front< no_value_sig, ret_type>::type fttype;
        public:
            // Form function type again from the mpl vector:
            typedef typename ft::function_type<fttype>::type Ft;
            // Type is a more standard way of result type.
            typedef Ft type;
            // The collect type signature as an mpl list
            typedef fttype mpl_type;
        };

        /**
         * Used to implement collect(), given a Function Signature.
         * @param int The number of arguments of collect.
         * @param Signature The C-style function signature (function type).
         * @param ToCollect A class type which is called within collect.
         */
        template<int, class Signature, class ToCollect>
        struct CollectSignature;

        // This case is only present if the return value is void
        // and all arguments are of in kind.
        template<class F, class ToCollect>
        struct CollectSignature<0,F,ToCollect>
        {
            typedef void result_type;
            CollectSignature() : cimpl() {}
            CollectSignature(ToCollect implementation) : cimpl(implementation) {}
            ~CollectSignature() {}

            SendStatus collect()
            {
                if (this->cimpl)
                    return this->cimpl->collect();
                return SendFailure;
            }

            SendStatus collectIfDone()
            {
                if (this->cimpl)
                    return this->cimpl->collectIfDone();
                return SendFailure;
            }

        protected:
            ToCollect cimpl;
        };

        // Used when return non void with only in kind args OR return void
        // and one out/inout arg.
        template<class F, class ToCollect>
        struct CollectSignature<1,F,ToCollect>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;

            CollectSignature() : cimpl() {}
            CollectSignature(ToCollect implementation) : cimpl(implementation) {}
            ~CollectSignature() {}

            /**
             * Collect this operator if the method has one argument.
             */
            SendStatus collect(arg1_type a1)
            {
                if (cimpl)
                    return cimpl->collect( a1 );
                return SendFailure;
            }

            SendStatus collectIfDone(arg1_type a1)
            {
                if (cimpl)
                    return cimpl->collectIfDone( a1 );
                return SendFailure;
            }
        protected:
            ToCollect cimpl;
        };

        template<class F, class ToCollect>
        struct CollectSignature<2,F,ToCollect>
        {
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;

            CollectSignature() : cimpl() {}
            CollectSignature(ToCollect implementation) : cimpl(implementation) {}
            ~CollectSignature() {}

            /**
             * Collect this operator if the method has two arguments.
             */
            SendStatus collect(arg1_type t1, arg2_type t2)
            {
                if (cimpl)
                    return cimpl->collect(t1, t2);
                return SendFailure;
            }

            SendStatus collectIfDone(arg1_type t1, arg2_type t2)
            {
                if (cimpl)
                    return cimpl->collectIfDone(t1, t2);
                return SendFailure;
            }
        protected:
            ToCollect cimpl;
        };

        template<class F, class ToCollect>
        struct CollectSignature<3,F,ToCollect>
        {
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;

            CollectSignature() : cimpl() {}
            CollectSignature(ToCollect implementation) : cimpl(implementation) {}
            ~CollectSignature() { }

            /**
             * Collect this operator if the method has three arguments.
             */
            SendStatus collect(arg1_type t1, arg2_type t2, arg3_type t3)
            {
                if (cimpl)
                    return cimpl->collect(t1, t2, t3);
                return SendFailure;
            }

            SendStatus collectIfDone(arg1_type t1, arg2_type t2, arg3_type t3)
            {
                if (cimpl)
                    return cimpl->collectIfDone(t1, t2, t3);
                return SendFailure;
            }
        protected:
            ToCollect cimpl;
        };

        template<class F, class ToCollect>
        struct CollectSignature<4,F,ToCollect>
        {
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;

            CollectSignature() : cimpl() {}
            CollectSignature(ToCollect implementation) : cimpl(implementation) {}
            ~CollectSignature() { }

            /**
             * Collect this operator if the method has four arguments.
             */
            SendStatus collect(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4)
            {
                if (cimpl)
                    return cimpl->collect(t1, t2, t3, t4);
                return SendFailure;
            }

            SendStatus collectIfDone(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4)
            {
                if (cimpl)
                    return cimpl->collect(t1, t2, t3, t4);
                return SendFailure;
            }
        protected:
            ToCollect cimpl;
        };

        template<class F, class ToCollect>
        struct CollectSignature<6,F,ToCollect>
        {
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;
            typedef typename boost::function_traits<F>::arg5_type arg5_type;
            typedef typename boost::function_traits<F>::arg6_type arg6_type;

            CollectSignature() : cimpl() {}
            CollectSignature(ToCollect implementation) : cimpl(implementation) {}
            ~CollectSignature() { }

            /**
             * Collect this operator if the method has four arguments.
             */
            SendStatus collect(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4, arg5_type t5, arg6_type t6)
            {
                if (cimpl)
                    return cimpl->collect(t1, t2, t3, t4, t5, t6);
                return SendFailure;
            }

            SendStatus collectIfDone(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4, arg5_type t5, arg6_type t6)
            {
                if (cimpl)
                    return cimpl->collect(t1, t2, t3, t4, t5, t6);
                return SendFailure;
            }
        protected:
            ToCollect cimpl;
        };

        template<class F, class ToCollect>
        struct CollectSignature<7,F,ToCollect>
        {
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;
            typedef typename boost::function_traits<F>::arg5_type arg5_type;
            typedef typename boost::function_traits<F>::arg6_type arg6_type;
            typedef typename boost::function_traits<F>::arg7_type arg7_type;

            CollectSignature() : cimpl() {}
            CollectSignature(ToCollect implementation) : cimpl(implementation) {}
            ~CollectSignature() { }

            /**
             * Collect this operator if the method has four arguments.
             */
            SendStatus collect(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4, arg5_type t5, arg6_type t6, arg7_type t7)
            {
                if (cimpl)
                    return cimpl->collect(t1, t2, t3, t4, t5, t6, t7);
                return SendFailure;
            }

            SendStatus collectIfDone(arg1_type t1, arg2_type t2, arg3_type t3, arg4_type t4, arg5_type t5, arg6_type t6, arg7_type t7)
            {
                if (cimpl)
                    return cimpl->collect(t1, t2, t3, t4, t5, t6, t7);
                return SendFailure;
            }
        protected:
            ToCollect cimpl;
        };

    }
}
#endif
