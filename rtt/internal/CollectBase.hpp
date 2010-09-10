/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  CollectBase.hpp

                        CollectBase.hpp -  description
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


#ifndef ORO_COLLECT_BASE_HPP
#define ORO_COLLECT_BASE_HPP

#include "CollectSignature.hpp"
#include "../SendStatus.hpp"
#include "ReturnBase.hpp"
#include <boost/function.hpp>

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
            : public CollectBaseImpl< boost::function_traits<typename CollectType<F>::Ft>::arity, typename CollectType<F>::Ft >,
              public ReturnBaseImpl< boost::function_traits<F>::arity, F>
        {
            typedef boost::shared_ptr<CollectBase<F> > shared_ptr;
        };

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
            typedef typename boost::function<Ft>::result_type result_type;
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

            virtual SendStatus collectIfDone(arg1_type a1) = 0;
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
            virtual SendStatus collectIfDone(arg1_type a1, arg2_type a2) = 0;
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
            virtual SendStatus collectIfDone(arg1_type a1, arg2_type a2, arg3_type a3) = 0;
        };
    }
}
#endif
