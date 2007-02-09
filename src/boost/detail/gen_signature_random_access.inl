
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

// Forward Sequence

#   define op size
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)

#   define op empty
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)

#   define op front
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)


// Bidirectional Sequence

#   define op back
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)


// Random Access Sequence

#   define op begin
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinisc decoration)

#   define op end
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)

#   define op at
#   define p N
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)

//------------------------------------------------------------------------------
} } // namespace ::boost::mpl
//------------------------------------------------------------------------------
#else // MPL-inrinsic decoration
//------------------------------------------------------------------------------
    template<> 
    struct BOOST_PP_CAT(op,_impl) 
    < function_types::detail::signature_mpl_sequence_tag >
    { 
#   ifndef p
      template< typename S > struct apply
        : op <typename S::types>
#   else
      template< typename S, typename p > struct apply
        : op <typename S::types, p >
#     undef p
#   endif
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

// Forward Sequence

#   define op size
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)

#   define op empty
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)

#   define op front
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)


// Bidirectional Sequence

#   define op back
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)


// Random Access Sequence

#   define op begin
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinisc decoration)

#   define op end
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)

#   define op at
#   define p N
#   include BOOST_FT_FRAGMENT(signature_random_access, MPL-intrinsic decoration)

//------------------------------------------------------------------------------
} } // namespace ::boost::mpl
//------------------------------------------------------------------------------
#else // MPL-inrinsic decoration
//------------------------------------------------------------------------------
    template<> 
    struct BOOST_PP_CAT(op,_impl) 
    < function_types::detail::signature_mpl_sequence_tag >
    { 
#   ifndef p
      template< typename S > struct apply
        : op <typename S::types>
#   else
      template< typename S, typename p > struct apply
        : op <typename S::types, p >
#     undef p
#   endif
      { };
    };
#   undef op
//------------------------------------------------------------------------------
#endif
