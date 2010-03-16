#ifndef ORO_RETURN_HPP
#define ORO_RETURN_HPP

#include "Collect.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * Implementation of ret() which takes the same signature
         * as call().
         */
        template<int, class F>
        struct ReturnImpl;

        template<class F>
        struct ReturnImpl<0,F, BaseImpl>
            : public Collect<F,BaseImpl>
        {
            virtual ~ReturnImpl() {}

        };

        template<class F>
        struct ReturnImpl<1,F>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::result_type result_type;
            typedef typename boost::function<F>::arg1_type arg1_type;
            virtual ~ReturnImpl() {}

        };

        template<class F>
        struct ReturnImpl<2,F>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            virtual ~ReturnImpl() {}

        };

        template<class F>
        struct ReturnImpl<3,F>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            virtual ~ReturnImpl() {}

        };

        template<class F>
        struct ReturnImpl<4,F>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            typedef typename boost::function<F>::arg4_type arg4_type;
            virtual ~ReturnImpl() {}

        };
    }
}
#endif
