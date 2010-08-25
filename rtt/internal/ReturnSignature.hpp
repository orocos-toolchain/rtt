#ifndef ORO_RETURN_SIGNATURE_HPP
#define ORO_RETURN_SIGNATURE_HPP

#include <boost/type_traits.hpp>
#include "NA.hpp"
#include "../rtt-fwd.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * Defines ret(), given a Signature.
         * @param int The number of arguments of operator().
         * @param Signature The C-style function signature (function type).
         * @param ToInvoke A class type which is called within operator().
         */
        template<int, class Signature, class ToInvoke>
        struct ReturnSignature;

        template<class F, class ToInvoke>
        struct ReturnSignature<0,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;

            ReturnSignature() : impl() {}
            ReturnSignature(ToInvoke implementation) : impl(implementation) {}
            ~ReturnSignature() {}

            result_type ret() {
                if (impl)
                    return impl->ret();
                return NA<result_type>::na();
            }

        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct ReturnSignature<1,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;

            ReturnSignature() : impl() {}
            ReturnSignature(ToInvoke implementation) : impl(implementation) {}
            ~ReturnSignature() {}

            result_type ret(arg1_type a1) {
                if (impl)
                    return impl->ret( a1 );
                return NA<result_type>::na();
            }

            result_type ret() {
                if (impl)
                    return impl->ret();
                return NA<result_type>::na();
            }

        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct ReturnSignature<2,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;

            ReturnSignature() : impl() {}
            ReturnSignature(ToInvoke implementation) : impl(implementation) {}
            ~ReturnSignature() {}

            result_type ret(arg1_type a1, arg2_type a2) {
                if (impl)
                    return impl->ret( a1,a2 );
                return NA<result_type>::na();
            }

            result_type ret() {
                if (impl)
                    return impl->ret();
                return NA<result_type>::na();
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct ReturnSignature<3,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;

            ReturnSignature() : impl() {}
            ReturnSignature(ToInvoke implementation) : impl(implementation) {}
            ~ReturnSignature() { }

            result_type ret(arg1_type a1, arg2_type a2, arg3_type a3) {
                if (impl)
                    return impl->ret( a1,a2,a3 );
                return NA<result_type>::na();
            }

            result_type ret() {
                if (impl)
                    return impl->ret();
                return NA<result_type>::na();
            }

        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct ReturnSignature<4,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;

            ReturnSignature() : impl() {}
            ReturnSignature(ToInvoke implementation) : impl(implementation) {}
            ~ReturnSignature() { }

            result_type ret(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) {
                if (impl)
                    return impl->ret( a1,a2,a3,a4 );
                return NA<result_type>::na();
            }

            result_type ret() {
                if (impl)
                    return impl->ret();
                return NA<result_type>::na();
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct ReturnSignature<5,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;
            typedef typename boost::function_traits<F>::arg5_type arg5_type;

            ReturnSignature() : impl() {}
            ReturnSignature(ToInvoke implementation) : impl(implementation) {}
            ~ReturnSignature() { }

            result_type ret(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5) {
                if (impl)
                    return impl->ret( a1,a2,a3,a4,a5 );
                return NA<result_type>::na();
            }

            result_type ret() {
                if (impl)
                    return impl->ret();
                return NA<result_type>::na();
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct ReturnSignature<6,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;
            typedef typename boost::function_traits<F>::arg5_type arg5_type;
            typedef typename boost::function_traits<F>::arg6_type arg6_type;

            ReturnSignature() : impl() {}
            ReturnSignature(ToInvoke implementation) : impl(implementation) {}
            ~ReturnSignature() { }

            result_type ret(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6) {
                if (impl)
                    return impl->ret( a1,a2,a3,a4,a5,a6 );
                return NA<result_type>::na();
            }

            result_type ret() {
                if (impl)
                    return impl->ret();
                return NA<result_type>::na();
            }
        protected:
            ToInvoke impl;
        };

        template<class F, class ToInvoke>
        struct ReturnSignature<7,F,ToInvoke>
        {
            typedef typename boost::function_traits<F>::result_type result_type;
            typedef typename boost::function_traits<F>::arg1_type arg1_type;
            typedef typename boost::function_traits<F>::arg2_type arg2_type;
            typedef typename boost::function_traits<F>::arg3_type arg3_type;
            typedef typename boost::function_traits<F>::arg4_type arg4_type;
            typedef typename boost::function_traits<F>::arg5_type arg5_type;
            typedef typename boost::function_traits<F>::arg6_type arg6_type;
            typedef typename boost::function_traits<F>::arg7_type arg7_type;

            ReturnSignature() : impl() {}
            ReturnSignature(ToInvoke implementation) : impl(implementation) {}
            ~ReturnSignature() { }

            result_type ret(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7) {
                if (impl)
                    return impl->ret( a1,a2,a3,a4,a5,a6,a7 );
                return NA<result_type>::na();
            }

            result_type ret() {
                if (impl)
                    return impl->ret();
                return NA<result_type>::na();
            }
        protected:
            ToInvoke impl;
        };

    }
}
#endif
