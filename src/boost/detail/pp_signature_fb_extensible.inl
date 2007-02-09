
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace mpl {
    template<>
    struct clear_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename clear < typename S::types >::type
            >::type
          >
      { };
    };
    template<>
    struct push_front_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S, typename T >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename push_front < typename S::types, T >::type
            >::type
          >
      { };
    };
    template<>
    struct pop_front_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename pop_front < typename S::types >::type
            >::type
          >
      { };
    };
    template<>
    struct push_back_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S, typename T >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename push_back < typename S::types, T >::type
            >::type
          >
      { };
    };
    template<>
    struct pop_back_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename pop_back < typename S::types >::type
            >::type
          >
      { };
    };
} }

// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace mpl {
    template<>
    struct clear_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename clear < typename S::types >::type
            >::type
          >
      { };
    };
    template<>
    struct push_front_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S, typename T >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename push_front < typename S::types, T >::type
            >::type
          >
      { };
    };
    template<>
    struct pop_front_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename pop_front < typename S::types >::type
            >::type
          >
      { };
    };
    template<>
    struct push_back_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S, typename T >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename push_back < typename S::types, T >::type
            >::type
          >
      { };
    };
    template<>
    struct pop_back_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S >
      struct apply
        : function_types::function_type_signature
          < typename function_types::detail::function_type_impl
            < typename S::kind
            , typename pop_back < typename S::types >::type
            >::type
          >
      { };
    };
} }
