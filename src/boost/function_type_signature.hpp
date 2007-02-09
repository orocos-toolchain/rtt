
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_FUNCTION_TYPE_SIGNATURE_HPP_INCLUDED
#define BOOST_FT_FUNCTION_TYPE_SIGNATURE_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/type_traits/remove_cv.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/mpl/if.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/vector/vector0.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/function_types/detail/tags.hpp>
//------------------------------------------------------------------------------
namespace boost { namespace function_types {
//------------------------------------------------------------------------------
template<typename T> struct function_type_signature;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace detail 
{
  template<typename T> struct signature_impl;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename T> struct function_type_signature
  : detail::signature_impl< typename remove_cv<T>::type >
{ };
//------------------------------------------------------------------------------
namespace detail {
//------------------------------------------------------------------------------
struct signature_mpl_sequence_tag; 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct nullary_signature_base
{
  typedef mpl::vector0<> types;
  typedef mpl::vector0<> representee;
  typedef mpl::size_t<0> arity;
  typedef no_function kind;  
  typedef signature_mpl_sequence_tag tag;
};
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
template<typename Signature, typename T, typename KindFuncSpec>
struct transmuted_signature_base
{
  typedef T representee;
  typedef typename Signature::types types;
  typedef typename Signature::arity arity;
  typedef typename KindFuncSpec::type kind;
  typedef signature_mpl_sequence_tag tag;
};
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
template
< typename Signature
, typename T
, typename IfKind
, typename ThenKind 
, typename ElseBase 
>
struct conditioned_transmuted_signature_base
  : mpl::if_
    < tag_represents_subset_of<typename Signature::kind, IfKind>
    , transmuted_signature_base
      < Signature,T
      , mpl::bitxor_<typename Signature::kind, IfKind, ThenKind>
      >
    , ElseBase
    >::type
{ };
//------------------------------------------------------------------------------
} } } // namespace ::boost:function_types::detail
//------------------------------------------------------------------------------
// Include implementation
#include BOOST_FT_DEPENDENCIES(signature_impl_spec)
#include BOOST_FT_PREPROCESSED(signature_impl_spec)
//------------------------------------------------------------------------------
namespace boost 
{
  using function_types::function_type_signature;
} 
//------------------------------------------------------------------------------
// MPL Random Acess Sequence interface
#include BOOST_FT_DEPENDENCIES(signature_random_access)
#include BOOST_FT_PREPROCESSED(signature_random_access)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// MPL Forward/Backward Extensible Sequence (is included when both function_type
// and function_type_signature are available)
#ifdef BOOST_FT_FUNCTION_TYPE_HPP_INCLUDED
#   include BOOST_FT_DEPENDENCIES(signature_fb_extensible)
#   include BOOST_FT_PREPROCESSED(signature_fb_extensible)
#endif
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_FUNCTION_TYPE_SIGNATURE_HPP_INCLUDED

// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_FUNCTION_TYPE_SIGNATURE_HPP_INCLUDED
#define BOOST_FT_FUNCTION_TYPE_SIGNATURE_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/type_traits/remove_cv.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/mpl/if.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/vector/vector0.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/function_types/detail/tags.hpp>
//------------------------------------------------------------------------------
namespace boost { namespace function_types {
//------------------------------------------------------------------------------
template<typename T> struct function_type_signature;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace detail 
{
  template<typename T> struct signature_impl;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename T> struct function_type_signature
  : detail::signature_impl< typename remove_cv<T>::type >
{ };
//------------------------------------------------------------------------------
namespace detail {
//------------------------------------------------------------------------------
struct signature_mpl_sequence_tag; 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct nullary_signature_base
{
  typedef mpl::vector0<> types;
  typedef mpl::vector0<> representee;
  typedef mpl::size_t<0> arity;
  typedef no_function kind;  
  typedef signature_mpl_sequence_tag tag;
};
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
template<typename Signature, typename T, typename KindFuncSpec>
struct transmuted_signature_base
{
  typedef T representee;
  typedef typename Signature::types types;
  typedef typename Signature::arity arity;
  typedef typename KindFuncSpec::type kind;
  typedef signature_mpl_sequence_tag tag;
};
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
template
< typename Signature
, typename T
, typename IfKind
, typename ThenKind 
, typename ElseBase 
>
struct conditioned_transmuted_signature_base
  : mpl::if_
    < tag_represents_subset_of<typename Signature::kind, IfKind>
    , transmuted_signature_base
      < Signature,T
      , mpl::bitxor_<typename Signature::kind, IfKind, ThenKind>
      >
    , ElseBase
    >::type
{ };
//------------------------------------------------------------------------------
} } } // namespace ::boost:function_types::detail
//------------------------------------------------------------------------------
// Include implementation
#include BOOST_FT_DEPENDENCIES(signature_impl_spec)
#include BOOST_FT_PREPROCESSED(signature_impl_spec)
//------------------------------------------------------------------------------
namespace boost 
{
  using function_types::function_type_signature;
} 
//------------------------------------------------------------------------------
// MPL Random Acess Sequence interface
#include BOOST_FT_DEPENDENCIES(signature_random_access)
#include BOOST_FT_PREPROCESSED(signature_random_access)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// MPL Forward/Backward Extensible Sequence (is included when both function_type
// and function_type_signature are available)
#ifdef BOOST_FT_FUNCTION_TYPE_HPP_INCLUDED
#   include BOOST_FT_DEPENDENCIES(signature_fb_extensible)
#   include BOOST_FT_PREPROCESSED(signature_fb_extensible)
#endif
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_FUNCTION_TYPE_SIGNATURE_HPP_INCLUDED
