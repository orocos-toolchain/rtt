
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_FUNCTION_TYPE_HPP_INCLUDED
#define BOOST_FT_FUNCTION_TYPE_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <cstddef>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/size.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/function_types/detail/tags.hpp>
//------------------------------------------------------------------------------
namespace boost { namespace function_types {
//------------------------------------------------------------------------------
template<typename Tag, typename Ts> struct function_type;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace detail
{
  template<typename T, typename S, std::size_t N = ::boost::mpl::size<S>::value> 
  struct function_type_impl; 

  template<typename T> struct to_sequence;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename T> struct function_type_signature;  // fwd.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename Tag, typename Ts> struct function_type
{
  typedef typename 
    detail::function_type_impl
    < typename detail::concretize_tag_to_default<Tag>::type
    , typename detail::to_sequence<Ts>::type
    , ::boost::mpl::size< typename detail::to_sequence<Ts>::type >::value
    >::type  
  type;
};
//------------------------------------------------------------------------------
namespace detail { 
//------------------------------------------------------------------------------
// Specialize signature for non-sequence types / strip decoration if signature
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename T>
struct to_sequence
{
  typedef typename
   mpl::eval_if
   < mpl::is_sequence<T>
   , mpl::identity<T>
   , to_sequence< function_type_signature<T> >
   >::type
  type;
};
template<typename T>
struct to_sequence< function_type_signature<T> > 
{
  typedef typename function_type_signature<T>::types type;
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Use plain_function specializations for function ptr/ref (primary template)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct empty_on_error { };

template<typename Tag, typename CorrespTo, typename S, std::size_t N>
struct corresponding_plain_function
{ 
  typedef typename 
    function_type_impl
    < typename transmute_tag<Tag, CorrespTo, plain_function>::type
    , S
    , N
    >::type
  type;
};

template<typename MFunc>
struct ptr_decorated
{
  typedef typename MFunc::type * type;
};

template<typename MFunc>
struct ref_decorated
{
  typedef typename MFunc::type & type;
};

template<typename Tag, typename S, std::size_t N>
struct function_type_impl 
  : mpl::if_
    < tag_represents_subset_of< Tag, function_pointer >
    , ptr_decorated<corresponding_plain_function<Tag,function_pointer,S,N> >
    , typename mpl::if_
      < tag_represents_subset_of< Tag, function_reference >
      , ref_decorated<corresponding_plain_function<Tag,function_reference,S,N> >
      , empty_on_error
      >::type 
    >::type
{ };
//------------------------------------------------------------------------------
} } } // namespace ::boost::function_types::detail
//------------------------------------------------------------------------------
#include BOOST_FT_DEPENDENCIES(function_type_impl_spec)
#include BOOST_FT_PREPROCESSED(function_type_impl_spec)
//------------------------------------------------------------------------------
namespace boost
{
  using function_types::function_type;
}
//------------------------------------------------------------------------------
// MPL Forward/Backward Extensible Sequence (is included when both function_type
// and function_type_signature are available)
#ifdef BOOST_FT_FUNCTION_TYPE_SIGNATURE_HPP_INCLUDED
#   include BOOST_FT_DEPENDENCIES(signature_fb_extensible)
#   include BOOST_FT_PREPROCESSED(signature_fb_extensible)
#endif
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_FUNCTION_TYPE_HPP_INCLUDED
