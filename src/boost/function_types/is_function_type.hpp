
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_IS_FUNCTION_TYPE_HPP_INCLUDED
#define BOOST_FT_IS_FUNCTION_TYPE_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/mpl/bitand.hpp>
#include <boost/mpl/equal_to.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include "function_type_signature.hpp"
//------------------------------------------------------------------------------
namespace boost { 
//------------------------------------------------------------------------------
namespace function_types 
{
  template<typename Kind, typename T> struct is_function_type
    : detail::tag_represents_subset_of
      < typename function_type_signature<T>::kind
      , Kind 
      >
  { };
}
using function_types::is_function_type;
//------------------------------------------------------------------------------
} // namespace ::boost
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_IS_FUNCTION_TYPE_HPP_INCLUDED
