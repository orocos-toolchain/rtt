
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------
#ifndef BOOST_FT_DETAIL_TAGS_HPP_INCLUDED
#define BOOST_FT_DETAIL_TAGS_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/mpl/if.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/bitand.hpp>
#include <boost/mpl/bitor.hpp>
#include <boost/mpl/bitxor.hpp>
#include <boost/mpl/shift_right.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include "config.hpp"
//------------------------------------------------------------------------------
namespace boost { namespace function_types {
//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
namespace constants 
{
  enum tag_bits
  {
  // These are encoded with the lowest two bits to get nice ordinals when 
  // masking out the attributes
    static_function           =  0x01
  , member_function           =  0x02
  // -- Attributes --
  // The next marks the two above as unset
  , unknown_or_template       =  0x04
  // Inverse of the above
  , xxx_function              =  0x08
  , variadic                  =  0x10
  , non_variadic              =  0x20
  // Pointer/reference
  , reference                 =  0x40
  , pointer                   =  0x80
  // The next marks the two above as unset
  , plain                     = 0x100
  // Testing semantic is: "is at least as cv-qualified as ?" 
  , const_qualified           = 0x200
  , volatile_qualified        = 0x400
  // Both bits above (to ease notation)
  , const_volatile_qualified  = 0x600
  };
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  enum { n_bits = 11 };
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  enum predefined_tag_constants
  {
    no_function 
      = unknown_or_template
  , any_function
      = xxx_function
  , any_variadic_function
      = xxx_function | variadic
  , any_non_variadic_function
      = xxx_function | non_variadic

  , core_type_mask
      = static_function | member_function
  , defaultcall
      = 1 << n_bits
  , variadic_mask
      = variadic | non_variadic
  };
}
//------------------------------------------------------------------------------
// Tag types from predefined constants
//------------------------------------------------------------------------------
#define BOOST_FT_GEN_TAG(c) typedef mpl::integral_c<long,constants:: c> c;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace tags
{
  BOOST_FT_GEN_TAG(no_function)
  BOOST_FT_GEN_TAG(any_function)
  BOOST_FT_GEN_TAG(any_variadic_function)
  BOOST_FT_GEN_TAG(any_non_variadic_function)
}
using namespace tags;

namespace detail
{
  BOOST_FT_GEN_TAG(core_type_mask)
  BOOST_FT_GEN_TAG(defaultcall)
  BOOST_FT_GEN_TAG(non_variadic)
  BOOST_FT_GEN_TAG(variadic_mask)
  BOOST_FT_GEN_TAG(n_bits)
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#undef BOOST_FT_GEN_TAG
//------------------------------------------------------------------------------
} } // namespace ::boost::function_types
//------------------------------------------------------------------------------
// Load generated tag constants and types
#include BOOST_FT_PREPROCESSED(tags)
//------------------------------------------------------------------------------
namespace boost { namespace function_types { namespace detail {
//------------------------------------------------------------------------------
// Tag metafunctions
//------------------------------------------------------------------------------
// Test if a tag represents a subset of a set represented by another tag
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename Tag, typename Set> 
struct tag_represents_subset_of
  : mpl::equal_to< mpl::bitand_<Tag,Set>, Set >
{ };
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Extract a type tag of the basic function type (member or non-member)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename Tag> 
struct tag_core_type_id
  : mpl::bitand_< Tag, core_type_mask >
{ };
// The two possible results of 'tag_core_type_id':
// ( Integral value of static_function is 1 )
typedef tag_core_type_id<plain_function>::type static_function;
// ( Integral value of member_function is 2 )
typedef tag_core_type_id<member_function_pointer>::type 
  member_function;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Change the kind of function a tag represents
template<typename Tag, typename FromKind, typename ToKind> 
struct transmute_tag
  : mpl::bitxor_< Tag, FromKind, ToKind >
{ };
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Used internally, needed to implement 'concretize_tag_if_abstract', below
template<typename Src, typename Cond, typename Bits>
struct set_bits_if_not
  : mpl::if_
    < mpl::equal_to<Cond, mpl::integral_c<long,0> >
    , mpl::bitor_<Src, Bits>
    , Src
    >::type
{ };
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Change "abstract" tags to "explicit non-variadic/defaultcall"
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
template<typename Tag>
struct concretize_tag_to_default
  : set_bits_if_not
    < set_bits_if_not
      < Tag
      , mpl::shift_right<Tag, n_bits>
      , defaultcall 
      >
    , mpl::bitand_<Tag, variadic_mask>
    , non_variadic
    >
{ };
//------------------------------------------------------------------------------
} } } // namespace ::boost::function_types::detail
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_DETAIL_TAGS_HPP_INCLUDED
