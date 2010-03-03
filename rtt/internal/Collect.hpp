#ifndef ORO_COLLECT_HPP
#define ORO_COLLECT_HPP

#include "CollectSignature.hpp"
#include "../SendStatus.hpp"

namespace RTT
{
    namespace internal
    {
        template<int, class Ft, class BaseImpl>
        struct CollectImpl;

        /**
         * This struct takes the user's Function signature \a F and
         * transforms it to the form required in the CollectImpl
         * template specialisations.
         */
        template<class F, class BaseImpl>
        struct Collect
            : public CollectImpl< boost::function_traits< typename CollectType<F>::Ft >::arity, typename CollectType<F>::Ft, BaseImpl >
        {
                SendStatus collect()
                {
                    return BaseImpl::collect_impl();
                }

                SendStatus collectIfDone()
                {
                    return BaseImpl::collectIfDone_impl();
                }
        };

        template<class Ft, class BaseImpl>
        struct CollectImpl<0,Ft,BaseImpl>
            : public BaseImpl
        {
            virtual ~CollectImpl() {}
        };

        template<class Ft, class BaseImpl>
        struct CollectImpl<1,Ft,BaseImpl>
        : public BaseImpl
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            virtual ~CollectImpl() {}

            /**
             * Collect F without returning the results.
             * @return
             */
            virtual SendStatus collect()
            {
                return BaseImpl::collect_impl();
            }
            /**
             * Collect a void(arg1_type) F or
             * arg1_type(void) F
             * @return
             */
            virtual SendStatus collect(arg1_type a1)
            {
                return BaseImpl::collect_impl(a1);
            }
            virtual SendStatus collectIfDone(arg1_type a1)
            {
                return BaseImpl::collectIfDone_impl(a1);
            }
        };

        template<class Ft, class BaseImpl>
        struct CollectImpl<2,Ft,BaseImpl>
        : public BaseImpl
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            typedef typename boost::function<Ft>::arg2_type arg2_type;
            virtual ~CollectImpl() {}

            /**
             * Collect F without returning the results.
             * @return
             */
            virtual SendStatus collect()
            {
                return BaseImpl::collect_impl();
            }
            /**
             * Collect a void(arg1_type) F or
             * arg1_type(void) F
             * @return
             */
            virtual SendStatus collect(arg1_type a1, arg2_type a2)
            {
                return BaseImpl::collect_impl(a1,a2);
            }
            virtual SendStatus collectIfDone(arg1_type a1, arg2_type a2)
            {
                return BaseImpl::collectIfDone_impl(a1,a2);
            }
        };

        template<class Ft, class BaseImpl>
        struct CollectImpl<3,Ft,BaseImpl>
        : public BaseImpl
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            typedef typename boost::function<Ft>::arg2_type arg2_type;
            typedef typename boost::function<Ft>::arg3_type arg3_type;
            virtual ~CollectImpl() {}

            /**
             * Collect F without returning the results.
             * @return
             */
            virtual SendStatus collect()
            {
                return BaseImpl::collect_impl();
            }
            /**
             * Collect a void(arg1_type) F or
             * arg1_type(void) F
             * @return
             */
            virtual SendStatus collect(arg1_type a1, arg2_type a2, arg3_type a3)
            {
                return BaseImpl::collect_impl(a1,a2,a3);
            }
            virtual SendStatus collectIfDone(arg1_type a1, arg2_type a2, arg3_type a3)
            {
                return BaseImpl::collectIfDone_impl(a1,a2,a3);
            }
        };

        template<class Ft, class BaseImpl>
        struct CollectImpl<4,Ft,BaseImpl>
        : public BaseImpl
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            typedef typename boost::function<Ft>::arg2_type arg2_type;
            typedef typename boost::function<Ft>::arg3_type arg3_type;
            typedef typename boost::function<Ft>::arg4_type arg4_type;
            virtual ~CollectImpl() {}

            /**
             * Collect F without returning the results.
             * @return
             */
            virtual SendStatus collect()
            {
                return BaseImpl::collect_impl();
            }
            /**
             * Collect a void(arg1_type) F or
             * arg1_type(void) F
             * @return
             */
            virtual SendStatus collect(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4)
            {
                return BaseImpl::collect_impl(a1,a2,a3,a4);
            }
            virtual SendStatus collectIfDone(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4)
            {
                return BaseImpl::collectIfDone_impl(a1,a2,a3,a4);
            }
        };

    }
}
#endif
