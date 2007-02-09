
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_PP_IS_ITERATING
//------------------------------------------------------------------------------
#   include <boost/preprocessor/cat.hpp>
#   include <boost/preprocessor/arithmetic/inc.hpp>
#   include <boost/preprocessor/arithmetic/dec.hpp>
#   include <boost/preprocessor/facilities/empty.hpp>
#   include <boost/preprocessor/facilities/identity.hpp>
#   include <boost/preprocessor/facilities/intercept.hpp>
#   include <boost/preprocessor/iteration/iterate.hpp>
#   include <boost/preprocessor/repetition/enum_params.hpp>
#   include <boost/preprocessor/repetition/enum_trailing_params.hpp>
//------------------------------------------------------------------------------
namespace boost { namespace function_types { namespace detail {
//------------------------------------------------------------------------------
template<typename T>
struct signature_impl
  : conditioned_transmuted_signature_base
    < signature_impl<T*>, T
    , function_pointer, /* --> */ plain_function
    , nullary_signature_base >
{ 
  typedef function_type_signature<T> type;
};
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
template<typename T>
struct signature_impl< T& >
  : conditioned_transmuted_signature_base
    < signature_impl<T*>, T&
    , function_pointer, /* --> */ function_reference 
    , nullary_signature_base >
{ 
  typedef function_type_signature<T&> type;
};
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
template<typename T>
struct signature_impl< T* >
  : nullary_signature_base
{ 
  typedef function_type_signature<T*> type;
}; 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define  BOOST_PP_FILENAME_1 BOOST_FT_FRAGMENT(signature_impl_spec,arity)
#   define  BOOST_PP_ITERATION_LIMITS (0,BOOST_PP_INC(BOOST_FT_MAX_ARITY))
#   include BOOST_PP_ITERATE()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
} } } // namespace ::boost::function_types::detail
//------------------------------------------------------------------------------
#elif !defined(i)                                                  
//------------------------------------------------------------------------------
#   define i BOOST_PP_FRAME_ITERATION(1)
#   define ii BOOST_PP_INC(i)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define sp \
        typename R BOOST_PP_ENUM_TRAILING_PARAMS(i,typename T) 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define a i

#   define BOOST_FT_CLIENT \
                BOOST_FT_FRAGMENT(signature_impl_spec,specialization)

#   define  BOOST_FT_FOR BOOST_FT_FUNC_PTR
#   include BOOST_FT_UNROLL()

#   undef  a
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   if ii >= 2 // member function pointers must have at least two sub types
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     define a BOOST_PP_DEC(i)

#     define BOOST_FT_CLIENT \
                  BOOST_FT_FRAGMENT(signature_impl_spec,specialization)

#     define  BOOST_FT_FOR BOOST_FT_MEM_FUN_PTRS
#     include BOOST_FT_UNROLL()

#     undef a
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   endif // ii >= 2
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   undef sp
#   undef ii
#   undef i
//------------------------------------------------------------------------------
#else                                                          // specialization
//------------------------------------------------------------------------------
template< sp >
struct signature_impl
< t(i,BOOST_PP_EMPTY) >
{
  typedef t(i,BOOST_PP_IDENTITY(representee));
  typedef n kind;

  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;

  typedef mpl::size_t< a > arity;

  typedef BOOST_PP_CAT(mpl::vector,ii)
    < R BOOST_PP_ENUM_TRAILING_PARAMS(i,T) > types;
};
//------------------------------------------------------------------------------
#endif 

// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_PP_IS_ITERATING
//------------------------------------------------------------------------------
#   include <boost/preprocessor/cat.hpp>
#   include <boost/preprocessor/arithmetic/inc.hpp>
#   include <boost/preprocessor/arithmetic/dec.hpp>
#   include <boost/preprocessor/facilities/empty.hpp>
#   include <boost/preprocessor/facilities/identity.hpp>
#   include <boost/preprocessor/facilities/intercept.hpp>
#   include <boost/preprocessor/iteration/iterate.hpp>
#   include <boost/preprocessor/repetition/enum_params.hpp>
#   include <boost/preprocessor/repetition/enum_trailing_params.hpp>
//------------------------------------------------------------------------------
namespace boost { namespace function_types { namespace detail {
//------------------------------------------------------------------------------
template<typename T>
struct signature_impl
  : conditioned_transmuted_signature_base
    < signature_impl<T*>, T
    , function_pointer, /* --> */ plain_function
    , nullary_signature_base >
{ 
  typedef function_type_signature<T> type;
};
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
template<typename T>
struct signature_impl< T& >
  : conditioned_transmuted_signature_base
    < signature_impl<T*>, T&
    , function_pointer, /* --> */ function_reference 
    , nullary_signature_base >
{ 
  typedef function_type_signature<T&> type;
};
// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - 
template<typename T>
struct signature_impl< T* >
  : nullary_signature_base
{ 
  typedef function_type_signature<T*> type;
}; 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define  BOOST_PP_FILENAME_1 BOOST_FT_FRAGMENT(signature_impl_spec,arity)
#   define  BOOST_PP_ITERATION_LIMITS (0,BOOST_PP_INC(BOOST_FT_MAX_ARITY))
#   include BOOST_PP_ITERATE()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
} } } // namespace ::boost::function_types::detail
//------------------------------------------------------------------------------
#elif !defined(i)                                                  
//------------------------------------------------------------------------------
#   define i BOOST_PP_FRAME_ITERATION(1)
#   define ii BOOST_PP_INC(i)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define sp \
        typename R BOOST_PP_ENUM_TRAILING_PARAMS(i,typename T) 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define a i

#   define BOOST_FT_CLIENT \
                BOOST_FT_FRAGMENT(signature_impl_spec,specialization)

#   define  BOOST_FT_FOR BOOST_FT_FUNC_PTR
#   include BOOST_FT_UNROLL()

#   undef  a
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   if ii >= 2 // member function pointers must have at least two sub types
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     define a BOOST_PP_DEC(i)

#     define BOOST_FT_CLIENT \
                  BOOST_FT_FRAGMENT(signature_impl_spec,specialization)

#     define  BOOST_FT_FOR BOOST_FT_MEM_FUN_PTRS
#     include BOOST_FT_UNROLL()

#     undef a
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   endif // ii >= 2
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   undef sp
#   undef ii
#   undef i
//------------------------------------------------------------------------------
#else                                                          // specialization
//------------------------------------------------------------------------------
template< sp >
struct signature_impl
< t(i,BOOST_PP_EMPTY) >
{
  typedef t(i,BOOST_PP_IDENTITY(representee));
  typedef n kind;

  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;

  typedef mpl::size_t< a > arity;

  typedef BOOST_PP_CAT(mpl::vector,ii)
    < R BOOST_PP_ENUM_TRAILING_PARAMS(i,T) > types;
};
//------------------------------------------------------------------------------
#endif 
