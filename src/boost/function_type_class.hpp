
// (C) Copyright Jonathan Turkanis and Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_FUNCTION_TYPE_CLASS_HPP_INCLUDED
#define BOOST_FT_FUNCTION_TYPE_CLASS_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/mpl/at.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/function_types/function_type_signature.hpp>
//------------------------------------------------------------------------------
namespace boost {
//------------------------------------------------------------------------------
namespace function_types 
{
  namespace detail
  {
    template<typename S,typename K> struct class_impl
    { };

    template<typename S> 
    struct class_impl<S,member_function>
      : mpl::at_c< S,1 > 
    { };
  }

  template<typename T> struct function_type_class
    : detail::class_impl
      < typename function_type_signature<T>::types
      , typename detail::tag_core_type_id
        < typename function_type_signature<T>::kind >::type
      >
  { };
}
using function_types::function_type_class;
//------------------------------------------------------------------------------
} // namespace ::boost
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_FUNCTION_TYPE_CLASS_HPP_INCLUDED

// (C) Copyright Jonathan Turkanis and Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_FUNCTION_TYPE_CLASS_HPP_INCLUDED
#define BOOST_FT_FUNCTION_TYPE_CLASS_HPP_INCLUDED
//------------------------------------------------------------------------------
#include <boost/mpl/at.hpp>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <boost/function_types/function_type_signature.hpp>
//------------------------------------------------------------------------------
namespace boost {
//------------------------------------------------------------------------------
namespace function_types 
{
  namespace detail
  {
    template<typename S,typename K> struct class_impl
    { };

    template<typename S> 
    struct class_impl<S,member_function>
      : mpl::at_c< S,1 > 
    { };
  }

  template<typename T> struct function_type_class
    : detail::class_impl
      < typename function_type_signature<T>::types
      , typename detail::tag_core_type_id
        < typename function_type_signature<T>::kind >::type
      >
  { };
}
using function_types::function_type_class;
//------------------------------------------------------------------------------
} // namespace ::boost
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_FUNCTION_TYPE_CLASS_HPP_INCLUDED
