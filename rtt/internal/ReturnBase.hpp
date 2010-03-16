#ifndef ORO_RETURN_BASE_HPP
#define ORO_RETURN_BASE_HPP

namespace RTT
{
    namespace internal
    {
        /**
         * Implementation of ret() which takes the same signature
         * as call().
         */
        template<int, class F>
        struct ReturnBaseImpl;

        template<class F>
        struct ReturnBaseImpl<0,F>
        {
            virtual ~ReturnBaseImpl() {}

            virtual result_type ret() = 0;
        };

        template<class F>
        struct ReturnBaseImpl<1,F>
        {
            typedef typename boost::function<F>::result_type result_type;
            typedef typename boost::function<F>::arg1_type arg1_type;
            virtual ~ReturnBaseImpl() {}

            virtual result_type ret(arg1_type a1) = 0;
            virtual result_type ret() = 0;
        };

        template<class F>
        struct ReturnBaseImpl<2,F>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            virtual ~ReturnBaseImpl() {}

            virtual result_type ret(arg1_type a1, arg2_type a2) = 0;
            virtual result_type ret() = 0;
        };

        template<class F>
        struct ReturnBaseImpl<3,F>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            virtual ~ReturnBaseImpl() {}

            virtual result_type ret(arg1_type a1, arg2_type a2, arg3_type a3) = 0;
            virtual result_type ret() = 0;
        };

        template<class F>
        struct ReturnBaseImpl<4,F>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            typedef typename boost::function<F>::arg4_type arg4_type;
            virtual ~ReturnBaseImpl() {}

            virtual result_type ret(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) = 0;
            virtual result_type ret() = 0;
        };
    }
}
#endif
