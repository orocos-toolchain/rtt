
// (C) Copyright Jonathan Turkanis and Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_FUNCTION_TYPE_ARITY_HPP_INCLUDED
#define BOOST_FT_FUNCTION_TYPE_ARITY_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/function_types/function_type_signature.hpp>
//------------------------------------------------------------------------------
namespace boost {
//------------------------------------------------------------------------------
namespace function_types 
{
  namespace detail
  {
    template<typename Arity,typename Kind> struct arity_impl
      : Arity
    { };

    template<typename T> struct arity_impl<T,no_function> { };
  }
  template<typename T> struct function_type_arity
    : detail::arity_impl
      < typename function_type_signature<T>::arity
      , typename function_type_signature<T>::kind >
  { };
}
using function_types::function_type_arity;
//------------------------------------------------------------------------------
} // namespace ::boost
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_FUNCTION_TYPE_ARITY_HPP_INCLUDED
