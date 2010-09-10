/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:38 CET 2006  Signal.hpp

                        Signal.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#ifndef ORO_CORELIB_SIGNAL_HPP
#define ORO_CORELIB_SIGNAL_HPP

#define OROCOS_SIGNAL_MAX_ARGS 4

#include <boost/type_traits/function_traits.hpp>
#include <boost/function.hpp>
#include "../Handle.hpp"
#include "signal0.hpp"
#include "signal1.hpp"
#include "signal2.hpp"
#include "signal3.hpp"
#include "signal4.hpp"
#include "signal5.hpp"
#include "signal6.hpp"
#include "signal7.hpp"

namespace RTT {
    namespace internal {
      template<int Arity,
               typename Signature,
               typename SlotFunction>
      class real_get_signal_impl;

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<0, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal0<typename traits::result_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<1, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal1<typename traits::result_type,
                        typename traits::arg1_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<2, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal2<typename traits::result_type,
                        typename traits::arg1_type,
                        typename traits::arg2_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<3, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal3<typename traits::result_type,
                        typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<4, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal4<typename traits::result_type,
                        typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<5, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal5<typename traits::result_type,
                        typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<6, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal6<typename traits::result_type,
                        typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        typename traits::arg6_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<7, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal7<typename traits::result_type,
                        typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        typename traits::arg6_type,
                        typename traits::arg7_type,
                        SlotFunction> type;
      };


      template<typename Signature,
               typename SlotFunction>
      struct get_signal_impl :
        public real_get_signal_impl<(boost::function_traits<Signature>::arity),
                                    Signature,
                                    SlotFunction>
      {
      };

    /**
     * Very lightweight wrapper around the signalN classes that allows signals to
     * be created where the number of arguments does not need to be part of the
     * class name.
     */
    template<
        typename Signature,
        typename TSlotFunction = boost::function<Signature>
    >
    class Signal :
        public get_signal_impl<Signature,
                                       TSlotFunction>::type
    {
    protected:
        typedef typename get_signal_impl< Signature,
                                                  TSlotFunction>::type base_type;

    public:
        typedef boost::shared_ptr<Signal<Signature,TSlotFunction> > shared_ptr;
        Signal() {}
        typedef TSlotFunction SlotFunction;
    };

    } // end namespace detail

} // namespace RTT


#endif

