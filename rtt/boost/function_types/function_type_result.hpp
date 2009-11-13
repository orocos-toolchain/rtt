
// (C) Copyright Jonathan Turkanis and Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_FUNCTION_TYPE_RESULT_HPP_INCLUDED
#define BOOST_FT_FUNCTION_TYPE_RESULT_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/mpl/at.hpp>
#include <boost/mpl/plus.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include "function_type_signature.hpp"
//------------------------------------------------------------------------------
namespace boost {
//------------------------------------------------------------------------------
namespace function_types
{
  namespace detail
  {
    template<typename S,typename K> struct result_impl
      : mpl::at_c< S, 0 >
    { };

    template<typename S>
    struct result_impl <S,no_function>
    { };
  }

  template<typename T> struct function_type_result
    : detail::result_impl
      < typename function_type_signature<T>::types
      , detail::tag_core_type_id< typename function_type_signature<T>::kind >
      >
  { };
}
using function_types::function_type_result;
//------------------------------------------------------------------------------
} // namespace ::boost
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_FUNCTION_TYPE_PARAMETER_HPP_INCLUDED
