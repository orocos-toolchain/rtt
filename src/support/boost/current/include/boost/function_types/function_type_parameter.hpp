
// (C) Copyright Jonathan Turkanis and Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_FUNCTION_TYPE_PARAMETER_HPP_INCLUDED
#define BOOST_FT_FUNCTION_TYPE_PARAMETER_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <cstddef>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/mpl/at.hpp>
#include <boost/mpl/plus.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/function_types/function_type_signature.hpp>
//------------------------------------------------------------------------------
namespace boost {
//------------------------------------------------------------------------------
namespace function_types 
{
  namespace detail
  {
    template<typename S,typename K,typename I> struct parameter_impl
      : mpl::at< S, mpl::plus<I,K> >
    { };

    template<typename S,typename I> 
    struct parameter_impl<S,no_function,I>
    { };
  }

  template<typename T,typename I> struct function_type_parameter
    : detail::parameter_impl
      < typename function_type_signature<T>::types
      , typename detail::tag_core_type_id
        < typename function_type_signature<T>::kind >::type
      , I
      >
  { };

  template<typename T, std::size_t I> struct function_type_parameter_c
    : detail::parameter_impl
      < typename function_type_signature<T>::types
      , typename detail::tag_core_type_id
        < typename function_type_signature<T>::kind >::type
      , mpl::integral_c<long,I>
      >
  { };
}
using function_types::function_type_parameter;
using function_types::function_type_parameter_c;
//------------------------------------------------------------------------------
} // namespace ::boost
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_FUNCTION_TYPE_PARAMETER_HPP_INCLUDED
