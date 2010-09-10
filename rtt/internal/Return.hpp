/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  Return.hpp

                        Return.hpp -  description
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
        template<int, class F, class BaseImpl>
        struct ReturnImpl;

        template<class F, class BaseImpl>
        struct Return
            : public ReturnImpl<boost::function_traits<F>::arity,F,BaseImpl>
        {};


        template<class F, class BaseImpl>
        struct ReturnImpl<0,F, BaseImpl>
            : public Collect<F,BaseImpl>
        {
            virtual ~ReturnImpl() {}

        };

        template<class F, class BaseImpl>
        struct ReturnImpl<1,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::result_type result_type;
            typedef typename boost::function<F>::arg1_type arg1_type;
            virtual ~ReturnImpl() {}

        };

        template<class F, class BaseImpl>
        struct ReturnImpl<2,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            virtual ~ReturnImpl() {}

        };

        template<class F, class BaseImpl>
        struct ReturnImpl<3,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            virtual ~ReturnImpl() {}

        };

        template<class F, class BaseImpl>
        struct ReturnImpl<4,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            typedef typename boost::function<F>::arg4_type arg4_type;
            virtual ~ReturnImpl() {}

        };

        template<class F, class BaseImpl>
        struct ReturnImpl<5,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            typedef typename boost::function<F>::arg4_type arg4_type;
            typedef typename boost::function<F>::arg5_type arg5_type;
            virtual ~ReturnImpl() {}

        };

        template<class F, class BaseImpl>
        struct ReturnImpl<6,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            typedef typename boost::function<F>::arg4_type arg4_type;
            typedef typename boost::function<F>::arg5_type arg5_type;
            typedef typename boost::function<F>::arg6_type arg6_type;
            virtual ~ReturnImpl() {}

        };

        template<class F, class BaseImpl>
        struct ReturnImpl<7,F,BaseImpl>
            : public Collect<F,BaseImpl>
        {
            typedef typename boost::function<F>::arg1_type arg1_type;
            typedef typename boost::function<F>::arg2_type arg2_type;
            typedef typename boost::function<F>::arg3_type arg3_type;
            typedef typename boost::function<F>::arg4_type arg4_type;
            typedef typename boost::function<F>::arg5_type arg5_type;
            typedef typename boost::function<F>::arg6_type arg6_type;
            typedef typename boost::function<F>::arg7_type arg7_type;
            virtual ~ReturnImpl() {}

        };
    }
}
#endif
