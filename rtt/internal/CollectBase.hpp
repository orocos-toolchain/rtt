#ifndef ORO_COLLECT_BASE_HPP
#define ORO_COLLECT_BASE_HPP

#include "CollectSignature.hpp"
#include "../SendStatus.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * The collection signature is formed by Ft, which is a
         * transformed form of F. Ft returns SendStatus and has
         * a number of arguments containing in order:
         *  - The return value by reference if non void
         *  - Each non-const reference argument of F
         *
         */
        template<int, class Ft>
        struct CollectBaseImpl;

        /**
         * This struct takes the user's Function signature \a F and
         * transforms it to the form required in the CollectBaseImpl
         * template specialisations. It then defines the collection
         * methods as pure virtuals.
         */
        template<class F>
        struct CollectBase
            : public CollectBaseImpl< boost::function_traits<F>::arity, typename CollectSignature<F>::Ft >
        {};

        template<class Ft>
        struct CollectBaseImpl<0,Ft>
        {
            virtual ~CollectBaseImpl() {}

            /**
             * Collect a void(void) F
             * @return
             */
            virtual SendStatus collect() = 0;

            virtual SendStatus collectIfDone() = 0;
        };

        template<class Ft>
        struct CollectBaseImpl<1,Ft>
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            virtual ~CollectBaseImpl() {}

            /**
             * Collect F without returning the results.
             * @return
             */
            virtual SendStatus collect() = 0;
            /**
             * Collect a void(arg1_type) F or
             * arg1_type(void) F
             * @return
             */
            virtual SendStatus collect(arg1_type a1) = 0;
        };

        template<class Ft>
        struct CollectBaseImpl<2,Ft>
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            typedef typename boost::function<Ft>::arg2_type arg2_type;
            virtual ~CollectBaseImpl() {}

            /**
             * Collect F without returning the results.
             * @return
             */
            virtual SendStatus collect() = 0;
            /**
             * Collect a void(arg1_type) F or
             * arg1_type(void) F
             * @return
             */
            virtual SendStatus collect(arg1_type a1, arg2_type a2) = 0;
        };

        template<class Ft>
        struct CollectBaseImpl<3,Ft>
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            typedef typename boost::function<Ft>::arg2_type arg2_type;
            typedef typename boost::function<Ft>::arg3_type arg3_type;
            virtual ~CollectBaseImpl() {}

            /**
             * Collect F without returning the results.
             * @return
             */
            virtual SendStatus collect() = 0;
            /**
             * Collect a void(arg1_type) F or
             * arg1_type(void) F
             * @return
             */
            virtual SendStatus collect(arg1_type a1, arg2_type a2, arg3_type a3) = 0;
        };
    }
}
#endif
