/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  Collect.hpp

                        Collect.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_COLLECT_HPP
#define ORO_COLLECT_HPP

#include "CollectSignature.hpp"
#include "../SendStatus.hpp"
#include <boost/function.hpp>
#include <boost/type_traits.hpp>

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
            typedef typename boost::function_traits< F >::result_type result_type;
            using CollectImpl< boost::function_traits< typename CollectType<F>::Ft >::arity, typename CollectType<F>::Ft, BaseImpl >::collectIfDone;

                SendStatus collect()
                {
                    return BaseImpl::collect_impl();
                }

                SendStatus collectIfDone()
                {
                    return BaseImpl::collectIfDone_impl();
                }

            result_type ret()
            {
                return BaseImpl::ret_impl();
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

        template<class Ft, class BaseImpl>
        struct CollectImpl<5,Ft,BaseImpl>
        : public BaseImpl
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            typedef typename boost::function<Ft>::arg2_type arg2_type;
            typedef typename boost::function<Ft>::arg3_type arg3_type;
            typedef typename boost::function<Ft>::arg4_type arg4_type;
            typedef typename boost::function<Ft>::arg5_type arg5_type;
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
            virtual SendStatus collect(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5)
            {
                return BaseImpl::collect_impl(a1,a2,a3,a4,a5);
            }
            virtual SendStatus collectIfDone(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5)
            {
                return BaseImpl::collectIfDone_impl(a1,a2,a3,a4,a5);
            }
        };

        template<class Ft, class BaseImpl>
        struct CollectImpl<6,Ft,BaseImpl>
        : public BaseImpl
        {
            typedef typename boost::function<Ft>::arg1_type arg1_type;
            typedef typename boost::function<Ft>::arg2_type arg2_type;
            typedef typename boost::function<Ft>::arg3_type arg3_type;
            typedef typename boost::function<Ft>::arg4_type arg4_type;
            typedef typename boost::function<Ft>::arg5_type arg5_type;
            typedef typename boost::function<Ft>::arg6_type arg6_type;
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
            virtual SendStatus collect(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6)
            {
                return BaseImpl::collect_impl(a1,a2,a3,a4,a5,a6);
            }
            virtual SendStatus collectIfDone(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6)
            {
                return BaseImpl::collectIfDone_impl(a1,a2,a3,a4,a5,a6);
            }
        };

    }
}
#endif
