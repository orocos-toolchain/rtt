
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_PP_IS_ITERATING
//------------------------------------------------------------------------------
#   include <boost/preprocessor/cat.hpp>
#   include <boost/preprocessor/arithmetic/inc.hpp>
#   include <boost/preprocessor/arithmetic/dec.hpp>
#   include <boost/preprocessor/punctuation/comma.hpp>
#   include <boost/preprocessor/facilities/identity.hpp>
#   include <boost/preprocessor/iteration/local.hpp>
#   include <boost/preprocessor/iteration/iterate.hpp>
#   include <boost/preprocessor/repetition/enum_trailing_params.hpp>
//------------------------------------------------------------------------------
namespace boost { namespace function_types { namespace detail {
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename S, std::size_t N> 
struct function_type_impl<no_function,S, N> 
{ 
  typedef S type; 
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define  BOOST_PP_FILENAME_1 BOOST_DETAIL_FT_FRAGMENT(function_type_impl_spec,arity)
#   define  BOOST_PP_ITERATION_LIMITS (0,BOOST_PP_INC(BOOST_FT_MAX_ARITY))
#   include BOOST_PP_ITERATE()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
} } } // namespace ::boost::function_types::detail
//------------------------------------------------------------------------------
#elif !defined(i)                                                       // arity
//------------------------------------------------------------------------------
#   define i BOOST_PP_FRAME_ITERATION(1)
#   define ii BOOST_PP_INC(i)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define BOOST_FT_CLIENT \
              BOOST_DETAIL_FT_FRAGMENT(function_type_impl_spec,specialization) 
#   if ii < 2
#     define BOOST_FT_FOR BOOST_FT_PLAIN_FUNC
#   else
#     define BOOST_FT_FOR BOOST_FT_PLAIN_FUNC BOOST_FT_MEM_FUN_PTRS
#   endif

#   define BOOST_FT_FORCE_DEF_CC

#   include BOOST_FT_UNROLL()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   undef ii
#   undef i
//------------------------------------------------------------------------------
#else                                                          // specialization
//------------------------------------------------------------------------------
#   define s(i) BOOST_PP_CAT(itr,BOOST_PP_IF(i,BOOST_PP_DEC(i),R))
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template<typename S>
struct function_type_impl<n, S, ii>
{
  template<typename R BOOST_PP_ENUM_TRAILING_PARAMS(i,typename T)>
  struct synthesize
  {
    typedef t(i,BOOST_PP_IDENTITY(type));
  };
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Create sub type iterators
#   define BOOST_PP_LOCAL_LIMITS (0,i)
#   define BOOST_PP_LOCAL_MACRO(j) \
  typedef typename BOOST_PP_IF(j, \
    mpl::next<s(BOOST_PP_DEC(j))>, mpl::begin<S>) :: type s(j);
#   include BOOST_PP_LOCAL_ITERATE()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Dereference sub type iterators and use with synthesize template
  typedef typename 
    synthesize
#   define BOOST_PP_LOCAL_LIMITS (0,i)
#   define BOOST_PP_LOCAL_MACRO(j)  \
  BOOST_PP_IF(j,BOOST_PP_COMMA,BOOST_PP_IDENTITY(<))() \
    typename mpl::deref< s(j) >::type
#   include BOOST_PP_LOCAL_ITERATE() 
    >::type 
  type; 
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   undef s
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif
//------------------------------------------------------------------------------

