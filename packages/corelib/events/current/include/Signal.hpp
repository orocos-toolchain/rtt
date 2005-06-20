#ifndef ORO_CORELIB_SIGNAL_HPP
#define ORO_CORELIB_SIGNAL_HPP

#define OROCOS_SIGNAL_MAX_ARGS 10

#include <boost/type_traits/function_traits.hpp>
#include <boost/function.hpp>
#include "Handle.hpp"
#include "impl/signal0.hpp"
#include "impl/signal1.hpp"
#include "impl/signal2.hpp"
#include "impl/signal3.hpp"
#include "impl/signal4.hpp"
#include "impl/signal5.hpp"
#include "impl/signal6.hpp"
#include "impl/signal7.hpp"
#include "impl/signal8.hpp"
#include "impl/signal9.hpp"
#include "impl/signal10.hpp"


namespace sigslot {
    namespace detail {
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
      class real_get_signal_impl<8, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal8<typename traits::result_type,
                        typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        typename traits::arg6_type,
                        typename traits::arg7_type,
                        typename traits::arg8_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<9, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal9<typename traits::result_type,
                        typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        typename traits::arg6_type,
                        typename traits::arg7_type,
                        typename traits::arg8_type,
                        typename traits::arg9_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<10, Signature,
                                 SlotFunction>
      {
        typedef boost::function_traits<Signature> traits;

      public:
        typedef signal10<typename traits::result_type,
                         typename traits::arg1_type,
                         typename traits::arg2_type,
                         typename traits::arg3_type,
                         typename traits::arg4_type,
                         typename traits::arg5_type,
                         typename traits::arg6_type,
                         typename traits::arg7_type,
                         typename traits::arg8_type,
                         typename traits::arg9_type,
                         typename traits::arg10_type,
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

    } // end namespace detail

    /**
     * Very lightweight wrapper around the signalN classes that allows signals to
     * be created where the number of arguments does not need to be part of the
     * class name.
     */
    template<
        typename Signature, // function type R (T1, T2, ..., TN)
        typename SlotFunction = boost::function<Signature>
    >
    class signal :
        public detail::get_signal_impl<Signature,
                                       SlotFunction>::type
    {
    protected:
        typedef typename detail::get_signal_impl< Signature,
                                                  SlotFunction>::type base_type;
        
    public:
        signal() {}
    };
    
} // namespace sigslot


#endif

