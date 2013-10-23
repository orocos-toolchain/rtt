#ifndef RTT_INTERNAL_BITS_REMOVE_CR_HPP
#define RTT_INTERNAL_BITS_REMOVE_CR_HPP

#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
namespace RTT { namespace internal {
    using namespace boost;

    // combines remove_reference and remove_const
    template<typename T>
    struct remove_cr
    {
      typedef typename boost::remove_const<
        typename boost::remove_reference<T>::type>::type type;
    };
}}

#endif

