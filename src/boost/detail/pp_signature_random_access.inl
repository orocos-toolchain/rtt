
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace mpl {
    template<>
    struct size_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : size <typename S::types>
      { };
    };
    template<>
    struct empty_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : empty <typename S::types>
      { };
    };
    template<>
    struct front_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : front <typename S::types>
      { };
    };
    template<>
    struct back_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : back <typename S::types>
      { };
    };
    template<>
    struct begin_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : begin <typename S::types>
      { };
    };
    template<>
    struct end_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : end <typename S::types>
      { };
    };
    template<>
    struct at_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S, typename N > struct apply
        : at <typename S::types, N >
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
    struct size_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : size <typename S::types>
      { };
    };
    template<>
    struct empty_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : empty <typename S::types>
      { };
    };
    template<>
    struct front_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : front <typename S::types>
      { };
    };
    template<>
    struct back_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : back <typename S::types>
      { };
    };
    template<>
    struct begin_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : begin <typename S::types>
      { };
    };
    template<>
    struct end_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S > struct apply
        : end <typename S::types>
      { };
    };
    template<>
    struct at_impl
    < function_types::detail::signature_mpl_sequence_tag >
    {
      template< typename S, typename N > struct apply
        : at <typename S::types, N >
      { };
    };
} }
