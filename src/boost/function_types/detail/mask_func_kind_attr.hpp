
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_DETAIL_MASK_FUNC_KIND_ATTR_HPP_INCLUDED
#define BOOST_FT_DETAIL_MASK_FUNC_KIND_ATTR_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/mpl/bitand.hpp>
#include <boost/mpl/integral_c.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include "tags.hpp"
//------------------------------------------------------------------------------
namespace boost { namespace function_types {
//------------------------------------------------------------------------------
namespace detail
{
  template<typename T> struct mask_func_kind_attr
    : mpl::bitand_< T, base_type_mask >
  { };

  typedef mask_func_kind_attr<member_function_pointer>::type   member_function;
  typedef mask_func_kind_attr<plain_function>::type            static_function;
}
//------------------------------------------------------------------------------
} } // namespace ::boost::function_types
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_DETAIL_MASK_FUNC_KIND_ATTR_HPP_INCLUDED
