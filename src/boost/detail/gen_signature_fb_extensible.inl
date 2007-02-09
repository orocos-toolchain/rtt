
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#include <boost/preprocessor/cat.hpp>
//------------------------------------------------------------------------------
#ifndef op
//------------------------------------------------------------------------------
namespace boost { namespace mpl {
//------------------------------------------------------------------------------

// Extensible Sequence

#   define op clear
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)

// (insert/erase default implementation should work just fine)


// Front Extensible Sequence

#   define op push_front
#   define p T
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)

#   define op pop_front
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)


// Back Extensible Sequence

#   define op push_back
#   define p T
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)

#   define op pop_back
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)

//------------------------------------------------------------------------------
} } // namespace ::boost::mpl
//------------------------------------------------------------------------------
#else // MPL-intrinsic decoration
//------------------------------------------------------------------------------
    template<> 
    struct BOOST_PP_CAT(op,_impl) 
    < function_types::detail::signature_mpl_sequence_tag >
    { 
#   ifndef p
      template< typename S > 
#   else
      template< typename S, typename p >
#   endif
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
#   ifndef p
            , typename op < typename S::types >::type
#   else
            , typename op < typename S::types, p >::type 
#     undef p
#   endif
            >::type
          >
      { };
    };
#   undef op
//------------------------------------------------------------------------------
#endif

// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#include <boost/preprocessor/cat.hpp>
//------------------------------------------------------------------------------
#ifndef op
//------------------------------------------------------------------------------
namespace boost { namespace mpl {
//------------------------------------------------------------------------------

// Extensible Sequence

#   define op clear
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)

// (insert/erase default implementation should work just fine)


// Front Extensible Sequence

#   define op push_front
#   define p T
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)

#   define op pop_front
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)


// Back Extensible Sequence

#   define op push_back
#   define p T
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)

#   define op pop_back
#   include BOOST_FT_FRAGMENT(signature_fb_extensible, MPL-intrinsic decoration)

//------------------------------------------------------------------------------
} } // namespace ::boost::mpl
//------------------------------------------------------------------------------
#else // MPL-intrinsic decoration
//------------------------------------------------------------------------------
    template<> 
    struct BOOST_PP_CAT(op,_impl) 
    < function_types::detail::signature_mpl_sequence_tag >
    { 
#   ifndef p
      template< typename S > 
#   else
      template< typename S, typename p >
#   endif
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
#   ifndef p
            , typename op < typename S::types >::type
#   else
            , typename op < typename S::types, p >::type 
#     undef p
#   endif
            >::type
          >
      { };
    };
#   undef op
//------------------------------------------------------------------------------
#endif
