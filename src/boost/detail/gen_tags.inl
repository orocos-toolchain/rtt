
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/identity.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_shifted_params.hpp>
//------------------------------------------------------------------------------
#ifndef BOOST_FT_UNROLL
//------------------------------------------------------------------------------
//  Setup the unroll facility (used later and by dependent components)
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
#   define BOOST_FT_UNROLL() BOOST_FT_FRAGMENT(tags,unroll entry point)

#   define BOOST_FT_PLAIN_FUNC     (0)
#   define BOOST_FT_FUNC_REF       (1)
#   define BOOST_FT_FUNC_PTR       (2)
#   define BOOST_FT_MEM_FUN_PTRS   (3)(4)(5)(6)
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//  Setup tables for public naming
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
#   define BOOST_FT_BASE_NAMES (function)(member_function)
#   define BOOST_FT_NAME_DERIVATION_TABLE \
      /* has prefix | prefix \ has_suffix | suffix     */\
      /*- - - - - - - - - - - -\- - - - - - - - - - - -*/\
      /*   [   freestanding or static functions    ]   */\
      ((           1, plain         ,    0, -          ))\
      ((           0, -             ,    1, reference  ))\
      ((           0, -             ,    1, pointer    ))\
      /*   [      member function pointers         ]   */\
      ((           0, -             ,    1, pointer    ))\
      ((           1, const         ,    1, pointer    ))\
      ((           1, volatile      ,    1, pointer    ))\
      ((           1, const_volatile,    1, pointer    ))
#   define BOOST_FT_VARIADIC_PREFIX     variadic
#   define BOOST_FT_NON_VARIADIC_PREFIX non_variadic
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//  Setup tables for encoding / type composition
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
#   define BOOST_FT_ALL_FUNCTIONS_FLAG xxx_function
#   define BOOST_FT_BASE_FLAGS         (static_function)(member_function)
#   define BOOST_FT_VARIADIC_FLAG      variadic
#   define BOOST_FT_NON_VARIADIC_FLAG  non_variadic
#   define BOOST_FT_N_FLAGS            n_bits

#   define BOOST_FT_TYPE_DERIVATION_TABLE \
      /* base type idx / flags \  has t mod. \ t mod.    | has cvq / cvq     */\
      /*- /==========/- - - - - -\===========  \=|- - - -|- -/===/- - - - - -*/\
      ((0,  plain                             , 0,-      , 0, -              ))\
      ((0,  reference                         , 1,&      , 0, -              ))\
      ((0,  pointer                           , 1,*      , 0, -              ))\
      ((1,  pointer                           , 1,T0::*  , 0, -              ))\
      ((1,  pointer | const_qualified         , 1,T0::*  , 1, const          ))\
      ((1,  pointer | volatile_qualified      , 1,T0::*  , 1, volatile       ))\
      ((1,  pointer | const_volatile_qualified, 1,T0::*  , 1, const volatile ))

#   define BOOST_FT_TYPE_FUNCTION_0(n,opt_name,opt_cc,opt_ell) \
      R opt_cc() opt_name() ( BOOST_PP_ENUM_PARAMS(n,T) opt_ell() ) 

#   define BOOST_FT_TYPE_FUNCTION_1(n,opt_name,opt_cc,opt_ell) \
      R opt_cc() opt_name() ( BOOST_PP_ENUM_SHIFTED_PARAMS(n,T) opt_ell() ) 
//------------------------------------------------------------------------------
//  Generate constants
//------------------------------------------------------------------------------
namespace boost { namespace function_types {
//------------------------------------------------------------------------------
namespace constants 
{
  enum generated_tag_constants
  {
    dummy_enumerator_to_avoid_PP_COMMA_IF_logic

#   define  BOOST_FT_GEN_CONSTANT /**/

#   define  BOOST_FT_FOR \
      BOOST_FT_PLAIN_FUNC \
      BOOST_FT_FUNC_REF   \
      BOOST_FT_FUNC_PTR   \
      BOOST_FT_MEM_FUN_PTRS
#   define  BOOST_FT_GEN_ABSTRACT     /**/
#   define  BOOST_FT_FORCE_DEF_CC     /**/
#   define  BOOST_FT_CLIENT BOOST_FT_FRAGMENT(tags,generate constant)
#   include BOOST_FT_UNROLL()

#   undef   BOOST_FT_GEN_CONSTANT
  };
} // namespace constants
//------------------------------------------------------------------------------
namespace tags 
{
#   define BOOST_FT_GEN_TAG(c) \
      typedef mpl::integral_c<long,constants:: c> c;

#   define  BOOST_FT_FOR \
      BOOST_FT_PLAIN_FUNC \
      BOOST_FT_FUNC_REF   \
      BOOST_FT_FUNC_PTR   \
      BOOST_FT_MEM_FUN_PTRS
#   define  BOOST_FT_GEN_ABSTRACT     /**/
#   define  BOOST_FT_FORCE_DEF_CC     /**/
#   define  BOOST_FT_CLIENT BOOST_FT_FRAGMENT(tags,generate tag)
#   include BOOST_FT_UNROLL()
} 
using namespace tags;
#   undef BOOST_FT_GEN_TAG
//------------------------------------------------------------------------------
} // namespace function_types
//------------------------------------------------------------------------------
using namespace function_types::tags;
//------------------------------------------------------------------------------
} // namespace boost
//------------------------------------------------------------------------------
#elif defined(n) && defined(BOOST_FT_GEN_CONSTANT)          // generate constant
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  , n
      = f
//------------------------------------------------------------------------------
#elif defined(n) && defined(BOOST_FT_GEN_TAG)                    // generate tag
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  BOOST_FT_GEN_TAG(n)
//______________________________________________________________________________
//==============================================================================
// #include BOOST_FT_UNROLL() 
//==============================================================================
// Named input paramters:
//   BOOST_FT_CLIENT            - sub program to #include (quoted file name) 
//   BOOST_FT_FOR               - sequence to select type derivations to iterate
//   BOOST_FT_GEN_ABSTRACT      - visit abstract versions
//   BOOST_FT_FORCE_DEF_CC      - force visiting the default cc
// Macros defined in client program (no need to undefine there):
//   BOOST_FT_FIRST_UNROLL_DIM - first PP file iteration dimension used
//   n - The identifier name for the tag
//   f - The flags encoded in the tag
//   t - Type function of (n,opt_name) to generate an n-ary named type expr.
// Uses short macro names: n? t? f?
//------------------------------------------------------------------------------
#elif !defined(BOOST_FT_FIRST_UNROLL_DIM)                  // unroll entry point
//------------------------------------------------------------------------------
#   ifndef BOOST_PP_IS_ITERATING
#     define  BOOST_PP_FILENAME_1 BOOST_FT_FRAGMENT(tags,type derivation)
#     define BOOST_FT_FIRST_UNROLL_DIM 1
#   else
#     define  BOOST_PP_FILENAME_2 BOOST_FT_FRAGMENT(tags,type derivation)
#     define BOOST_FT_FIRST_UNROLL_DIM 2
#   endif
#   define  BOOST_PP_ITERATION_LIMITS (0,BOOST_PP_SEQ_SIZE(BOOST_FT_FOR)-1)
#   include BOOST_PP_ITERATE()
#   undef   BOOST_FT_FOR
#   undef   BOOST_FT_CLIENT
#   ifdef   BOOST_FT_FORCE_DEF_CC
#     undef   BOOST_FT_FORCE_DEF_CC
#   endif
#   ifdef   BOOST_FT_GEN_ABSTRACT
#     undef   BOOST_FT_GEN_ABSTRACT
#   endif
#   undef   BOOST_FT_FIRST_UNROLL_DIM
//------------------------------------------------------------------------------
#elif BOOST_PP_ITERATION_DEPTH() == BOOST_FT_FIRST_UNROLL_DIM // type derivation
//------------------------------------------------------------------------------
#   define ti BOOST_PP_SEQ_ELEM( \
                BOOST_PP_FRAME_ITERATION(BOOST_FT_FIRST_UNROLL_DIM) \
              , BOOST_FT_FOR)

#   define tt BOOST_PP_SEQ_ELEM( ti, BOOST_FT_TYPE_DERIVATION_TABLE )

#   define tn BOOST_PP_SEQ_ELEM( ti, BOOST_FT_NAME_DERIVATION_TABLE )

#   define tc BOOST_PP_TUPLE_ELEM( 2, BOOST_PP_TUPLE_ELEM(6,0,tt) \
                                 , BOOST_FT_CALLING_CONVENTIONS )
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Starting types (this will be refined successively until we call the client)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define n1 BOOST_PP_SEQ_ELEM( BOOST_PP_TUPLE_ELEM(6,0,tt) \
                               , BOOST_FT_BASE_NAMES )

#   define f1 BOOST_FT_ALL_FUNCTIONS_FLAG \
            | BOOST_PP_SEQ_ELEM( BOOST_PP_TUPLE_ELEM(6,0,tt) \
                               , BOOST_FT_BASE_FLAGS ) \
            | BOOST_PP_TUPLE_ELEM(6,1,tt)

#   define t1 BOOST_PP_CAT(BOOST_FT_TYPE_FUNCTION_,BOOST_PP_TUPLE_ELEM(6,0,tt))
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Add top-level type decoration to type function
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   if BOOST_PP_TUPLE_ELEM(6,2,tt) 
#     define t2(n,o_name,o_cc,o_ell) \
        t1( n \
          , BOOST_PP_IDENTITY((o_cc() BOOST_PP_TUPLE_ELEM(6,3,tt) o_name())) \
          , BOOST_PP_EMPTY \
          , o_ell )
#   else
#     define t2(n,o_name,o_cc,o_ell) t1(n,o_name,o_cc,o_ell)
#   endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Append cv qualifiers to type function result
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   if BOOST_PP_TUPLE_ELEM(6,4,tt) 
#     define t3(n,o_name,o_cc,o_ell) \
        t2(n,o_name,o_cc,o_ell) BOOST_PP_TUPLE_ELEM(6,5,tt)
#   else
#     define t3(n,o_name,o_cc,o_ell) t2(n,o_name,o_cc,o_ell)
#   endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Iterate calling conventions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   if BOOST_FT_FIRST_UNROLL_DIM == 1
#     define BOOST_PP_FILENAME_2 BOOST_FT_FRAGMENT(tags, CCs)
#   else
#     define BOOST_PP_FILENAME_3 BOOST_FT_FRAGMENT(tags, CCs)
#   endif
#   define  BOOST_PP_ITERATION_LIMITS (0,BOOST_PP_SEQ_SIZE(tc)-1)
#   include BOOST_PP_ITERATE()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   undef t3
#   undef t2
#   undef t1
#   undef f1
#   undef n1
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   undef tc
#   undef tn
#   undef tt
#   undef ti 
//------------------------------------------------------------------------------
#elif BOOST_PP_ITERATION_DEPTH() == BOOST_FT_FIRST_UNROLL_DIM + 1         // CCs
//------------------------------------------------------------------------------
#   if !defined(fi)
#     define fi BOOST_PP_FRAME_ITERATION(BOOST_PP_ITERATION_DEPTH())
#     define fc BOOST_PP_SEQ_ELEM(fi,tc)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    Include name of cc in identifier name, encode cc id along with the flags
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     define n2 BOOST_PP_SEQ_CAT( (BOOST_PP_TUPLE_ELEM(3,0,fc))(_)(n1) )
#     define f2 f1 | (1 << ( BOOST_FT_N_FLAGS + fi ))
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//    Default cc 
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#     if fi == 0 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Curry type function with empty cc specifier
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define t4(n,o_name,o_ell) t3(n,o_name,BOOST_PP_EMPTY,o_ell)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Abstract cc (no explicit identifier / no cc flags)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       if defined(BOOST_FT_GEN_ABSTRACT)
#         define f3 f1
#         define n3 n1
#         define fv 1
#         include BOOST_FT_FRAGMENT(tags, vary variadic)
#         undef  fv
#         undef  n3
#         undef  f3
#       endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Non abstract cc (explicit identifier name ('defaultcall')
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define f3 f2
#       define n3 n2
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Create variadic version if default cc allowed for default cc or forced
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define fv BOOST_PP_TUPLE_ELEM(3,2,fc)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Only continue if no custom CCs or forced
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       if BOOST_PP_SEQ_SIZE(tc) == 1 || defined(BOOST_FT_FORCE_DEF_CC)
#         include BOOST_FT_FRAGMENT(tags, vary variadic)
#       endif
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//    Custom cc
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#     else
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Curry type function with cc specifier, name and flag are ok, for now
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define t4(n,o_name,o_ell) \
          t3(n,o_name,BOOST_PP_IDENTITY(BOOST_PP_TUPLE_ELEM(3,1,fc)),o_ell)
#       define n3 n2
#       define f3 f2
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define fv BOOST_PP_TUPLE_ELEM(3,2,fc) 
#       include BOOST_FT_FRAGMENT(tags, vary variadic)
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef fv
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef t4
#     undef f3
#     undef f2
#     undef n3
#     undef n2
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef fc
#     undef fi
//------------------------------------------------------------------------------
#   elif !defined(f)                                            // vary variadic
//------------------------------------------------------------------------------
//    Non-variadic function types
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#     define t(n,o_name) t4(n,o_name,BOOST_PP_EMPTY)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    Abstract/implicit non-variadic variant, if requested
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     ifdef BOOST_FT_GEN_ABSTRACT
#       define f f3
#       define n4 n3
#       include BOOST_FT_FRAGMENT(tags, invoke client)
#       undef  n4
#       undef  f
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    Explicit non-variadic variant (with identifier annotation and flag)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     define f f3 | BOOST_FT_NON_VARIADIC_FLAG
#     define n4 BOOST_PP_SEQ_CAT((BOOST_FT_NON_VARIADIC_PREFIX)(_)(n3))
#     include BOOST_FT_FRAGMENT(tags, invoke client)
#     undef  n4
#     undef  f
#     undef  t
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//    Variadic function types, if allowed/forced
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#     if fv
#       define f f3 | BOOST_FT_VARIADIC_FLAG
#       define n4 BOOST_PP_SEQ_CAT((BOOST_FT_VARIADIC_PREFIX)(_)(n3))
#       define t(n,o_name) t4(n,o_name,BOOST_PP_IDENTITY(...))
#       include BOOST_FT_FRAGMENT(tags, invoke client)
#       undef  t
#       undef  n4
#       undef  f
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef t
#     undef f
#     undef n4
//------------------------------------------------------------------------------
#   elif !defined(n)                                            // invoke client
//------------------------------------------------------------------------------
//    Prepend pending identifier prefix/suffix, if any and invoke the client
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     if BOOST_PP_TUPLE_ELEM(4,0,tn)
#       define n5 BOOST_PP_SEQ_CAT((BOOST_PP_TUPLE_ELEM(4,1,tn))(_)(n4))
#     else
#       define n5 n4
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     if BOOST_PP_TUPLE_ELEM(4,2,tn) == 1
#       define n BOOST_PP_SEQ_CAT((n5)(_)(BOOST_PP_TUPLE_ELEM(4,3,tn)))
#     else
#       define n n5
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     include BOOST_FT_CLIENT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef n
#     undef n5
#   endif
//------------------------------------------------------------------------------
#endif


// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/identity.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_shifted_params.hpp>
//------------------------------------------------------------------------------
#ifndef BOOST_FT_UNROLL
//------------------------------------------------------------------------------
//  Setup the unroll facility (used later and by dependent components)
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
#   define BOOST_FT_UNROLL() BOOST_FT_FRAGMENT(tags,unroll entry point)

#   define BOOST_FT_PLAIN_FUNC     (0)
#   define BOOST_FT_FUNC_REF       (1)
#   define BOOST_FT_FUNC_PTR       (2)
#   define BOOST_FT_MEM_FUN_PTRS   (3)(4)(5)(6)
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//  Setup tables for public naming
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
#   define BOOST_FT_BASE_NAMES (function)(member_function)
#   define BOOST_FT_NAME_DERIVATION_TABLE \
      /* has prefix | prefix \ has_suffix | suffix     */\
      /*- - - - - - - - - - - -\- - - - - - - - - - - -*/\
      /*   [   freestanding or static functions    ]   */\
      ((           1, plain         ,    0, -          ))\
      ((           0, -             ,    1, reference  ))\
      ((           0, -             ,    1, pointer    ))\
      /*   [      member function pointers         ]   */\
      ((           0, -             ,    1, pointer    ))\
      ((           1, const         ,    1, pointer    ))\
      ((           1, volatile      ,    1, pointer    ))\
      ((           1, const_volatile,    1, pointer    ))
#   define BOOST_FT_VARIADIC_PREFIX     variadic
#   define BOOST_FT_NON_VARIADIC_PREFIX non_variadic
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//  Setup tables for encoding / type composition
//  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
#   define BOOST_FT_ALL_FUNCTIONS_FLAG xxx_function
#   define BOOST_FT_BASE_FLAGS         (static_function)(member_function)
#   define BOOST_FT_VARIADIC_FLAG      variadic
#   define BOOST_FT_NON_VARIADIC_FLAG  non_variadic
#   define BOOST_FT_N_FLAGS            n_bits

#   define BOOST_FT_TYPE_DERIVATION_TABLE \
      /* base type idx / flags \  has t mod. \ t mod.    | has cvq / cvq     */\
      /*- /==========/- - - - - -\===========  \=|- - - -|- -/===/- - - - - -*/\
      ((0,  plain                             , 0,-      , 0, -              ))\
      ((0,  reference                         , 1,&      , 0, -              ))\
      ((0,  pointer                           , 1,*      , 0, -              ))\
      ((1,  pointer                           , 1,T0::*  , 0, -              ))\
      ((1,  pointer | const_qualified         , 1,T0::*  , 1, const          ))\
      ((1,  pointer | volatile_qualified      , 1,T0::*  , 1, volatile       ))\
      ((1,  pointer | const_volatile_qualified, 1,T0::*  , 1, const volatile ))

#   define BOOST_FT_TYPE_FUNCTION_0(n,opt_name,opt_cc,opt_ell) \
      R opt_cc() opt_name() ( BOOST_PP_ENUM_PARAMS(n,T) opt_ell() ) 

#   define BOOST_FT_TYPE_FUNCTION_1(n,opt_name,opt_cc,opt_ell) \
      R opt_cc() opt_name() ( BOOST_PP_ENUM_SHIFTED_PARAMS(n,T) opt_ell() ) 
//------------------------------------------------------------------------------
//  Generate constants
//------------------------------------------------------------------------------
namespace boost { namespace function_types {
//------------------------------------------------------------------------------
namespace constants 
{
  enum generated_tag_constants
  {
    dummy_enumerator_to_avoid_PP_COMMA_IF_logic

#   define  BOOST_FT_GEN_CONSTANT /**/

#   define  BOOST_FT_FOR \
      BOOST_FT_PLAIN_FUNC \
      BOOST_FT_FUNC_REF   \
      BOOST_FT_FUNC_PTR   \
      BOOST_FT_MEM_FUN_PTRS
#   define  BOOST_FT_GEN_ABSTRACT     /**/
#   define  BOOST_FT_FORCE_DEF_CC     /**/
#   define  BOOST_FT_CLIENT BOOST_FT_FRAGMENT(tags,generate constant)
#   include BOOST_FT_UNROLL()

#   undef   BOOST_FT_GEN_CONSTANT
  };
} // namespace constants
//------------------------------------------------------------------------------
namespace tags 
{
#   define BOOST_FT_GEN_TAG(c) \
      typedef mpl::integral_c<long,constants:: c> c;

#   define  BOOST_FT_FOR \
      BOOST_FT_PLAIN_FUNC \
      BOOST_FT_FUNC_REF   \
      BOOST_FT_FUNC_PTR   \
      BOOST_FT_MEM_FUN_PTRS
#   define  BOOST_FT_GEN_ABSTRACT     /**/
#   define  BOOST_FT_FORCE_DEF_CC     /**/
#   define  BOOST_FT_CLIENT BOOST_FT_FRAGMENT(tags,generate tag)
#   include BOOST_FT_UNROLL()
} 
using namespace tags;
#   undef BOOST_FT_GEN_TAG
//------------------------------------------------------------------------------
} // namespace function_types
//------------------------------------------------------------------------------
using namespace function_types::tags;
//------------------------------------------------------------------------------
} // namespace boost
//------------------------------------------------------------------------------
#elif defined(n) && defined(BOOST_FT_GEN_CONSTANT)          // generate constant
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  , n
      = f
//------------------------------------------------------------------------------
#elif defined(n) && defined(BOOST_FT_GEN_TAG)                    // generate tag
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  BOOST_FT_GEN_TAG(n)
//______________________________________________________________________________
//==============================================================================
// #include BOOST_FT_UNROLL() 
//==============================================================================
// Named input paramters:
//   BOOST_FT_CLIENT            - sub program to #include (quoted file name) 
//   BOOST_FT_FOR               - sequence to select type derivations to iterate
//   BOOST_FT_GEN_ABSTRACT      - visit abstract versions
//   BOOST_FT_FORCE_DEF_CC      - force visiting the default cc
// Macros defined in client program (no need to undefine there):
//   BOOST_FT_FIRST_UNROLL_DIM - first PP file iteration dimension used
//   n - The identifier name for the tag
//   f - The flags encoded in the tag
//   t - Type function of (n,opt_name) to generate an n-ary named type expr.
// Uses short macro names: n? t? f?
//------------------------------------------------------------------------------
#elif !defined(BOOST_FT_FIRST_UNROLL_DIM)                  // unroll entry point
//------------------------------------------------------------------------------
#   ifndef BOOST_PP_IS_ITERATING
#     define  BOOST_PP_FILENAME_1 BOOST_FT_FRAGMENT(tags,type derivation)
#     define BOOST_FT_FIRST_UNROLL_DIM 1
#   else
#     define  BOOST_PP_FILENAME_2 BOOST_FT_FRAGMENT(tags,type derivation)
#     define BOOST_FT_FIRST_UNROLL_DIM 2
#   endif
#   define  BOOST_PP_ITERATION_LIMITS (0,BOOST_PP_SEQ_SIZE(BOOST_FT_FOR)-1)
#   include BOOST_PP_ITERATE()
#   undef   BOOST_FT_FOR
#   undef   BOOST_FT_CLIENT
#   ifdef   BOOST_FT_FORCE_DEF_CC
#     undef   BOOST_FT_FORCE_DEF_CC
#   endif
#   ifdef   BOOST_FT_GEN_ABSTRACT
#     undef   BOOST_FT_GEN_ABSTRACT
#   endif
#   undef   BOOST_FT_FIRST_UNROLL_DIM
//------------------------------------------------------------------------------
#elif BOOST_PP_ITERATION_DEPTH() == BOOST_FT_FIRST_UNROLL_DIM // type derivation
//------------------------------------------------------------------------------
#   define ti BOOST_PP_SEQ_ELEM( \
                BOOST_PP_FRAME_ITERATION(BOOST_FT_FIRST_UNROLL_DIM) \
              , BOOST_FT_FOR)

#   define tt BOOST_PP_SEQ_ELEM( ti, BOOST_FT_TYPE_DERIVATION_TABLE )

#   define tn BOOST_PP_SEQ_ELEM( ti, BOOST_FT_NAME_DERIVATION_TABLE )

#   define tc BOOST_PP_TUPLE_ELEM( 2, BOOST_PP_TUPLE_ELEM(6,0,tt) \
                                 , BOOST_FT_CALLING_CONVENTIONS )
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Starting types (this will be refined successively until we call the client)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define n1 BOOST_PP_SEQ_ELEM( BOOST_PP_TUPLE_ELEM(6,0,tt) \
                               , BOOST_FT_BASE_NAMES )

#   define f1 BOOST_FT_ALL_FUNCTIONS_FLAG \
            | BOOST_PP_SEQ_ELEM( BOOST_PP_TUPLE_ELEM(6,0,tt) \
                               , BOOST_FT_BASE_FLAGS ) \
            | BOOST_PP_TUPLE_ELEM(6,1,tt)

#   define t1 BOOST_PP_CAT(BOOST_FT_TYPE_FUNCTION_,BOOST_PP_TUPLE_ELEM(6,0,tt))
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Add top-level type decoration to type function
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   if BOOST_PP_TUPLE_ELEM(6,2,tt) 
#     define t2(n,o_name,o_cc,o_ell) \
        t1( n \
          , BOOST_PP_IDENTITY((o_cc() BOOST_PP_TUPLE_ELEM(6,3,tt) o_name())) \
          , BOOST_PP_EMPTY \
          , o_ell )
#   else
#     define t2(n,o_name,o_cc,o_ell) t1(n,o_name,o_cc,o_ell)
#   endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Append cv qualifiers to type function result
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   if BOOST_PP_TUPLE_ELEM(6,4,tt) 
#     define t3(n,o_name,o_cc,o_ell) \
        t2(n,o_name,o_cc,o_ell) BOOST_PP_TUPLE_ELEM(6,5,tt)
#   else
#     define t3(n,o_name,o_cc,o_ell) t2(n,o_name,o_cc,o_ell)
#   endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Iterate calling conventions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   if BOOST_FT_FIRST_UNROLL_DIM == 1
#     define BOOST_PP_FILENAME_2 BOOST_FT_FRAGMENT(tags, CCs)
#   else
#     define BOOST_PP_FILENAME_3 BOOST_FT_FRAGMENT(tags, CCs)
#   endif
#   define  BOOST_PP_ITERATION_LIMITS (0,BOOST_PP_SEQ_SIZE(tc)-1)
#   include BOOST_PP_ITERATE()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   undef t3
#   undef t2
#   undef t1
#   undef f1
#   undef n1
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   undef tc
#   undef tn
#   undef tt
#   undef ti 
//------------------------------------------------------------------------------
#elif BOOST_PP_ITERATION_DEPTH() == BOOST_FT_FIRST_UNROLL_DIM + 1         // CCs
//------------------------------------------------------------------------------
#   if !defined(fi)
#     define fi BOOST_PP_FRAME_ITERATION(BOOST_PP_ITERATION_DEPTH())
#     define fc BOOST_PP_SEQ_ELEM(fi,tc)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    Include name of cc in identifier name, encode cc id along with the flags
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     define n2 BOOST_PP_SEQ_CAT( (BOOST_PP_TUPLE_ELEM(3,0,fc))(_)(n1) )
#     define f2 f1 | (1 << ( BOOST_FT_N_FLAGS + fi ))
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//    Default cc 
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#     if fi == 0 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Curry type function with empty cc specifier
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define t4(n,o_name,o_ell) t3(n,o_name,BOOST_PP_EMPTY,o_ell)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Abstract cc (no explicit identifier / no cc flags)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       if defined(BOOST_FT_GEN_ABSTRACT)
#         define f3 f1
#         define n3 n1
#         define fv 1
#         include BOOST_FT_FRAGMENT(tags, vary variadic)
#         undef  fv
#         undef  n3
#         undef  f3
#       endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Non abstract cc (explicit identifier name ('defaultcall')
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define f3 f2
#       define n3 n2
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Create variadic version if default cc allowed for default cc or forced
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define fv BOOST_PP_TUPLE_ELEM(3,2,fc)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Only continue if no custom CCs or forced
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       if BOOST_PP_SEQ_SIZE(tc) == 1 || defined(BOOST_FT_FORCE_DEF_CC)
#         include BOOST_FT_FRAGMENT(tags, vary variadic)
#       endif
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//    Custom cc
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#     else
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Curry type function with cc specifier, name and flag are ok, for now
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define t4(n,o_name,o_ell) \
          t3(n,o_name,BOOST_PP_IDENTITY(BOOST_PP_TUPLE_ELEM(3,1,fc)),o_ell)
#       define n3 n2
#       define f3 f2
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#       define fv BOOST_PP_TUPLE_ELEM(3,2,fc) 
#       include BOOST_FT_FRAGMENT(tags, vary variadic)
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef fv
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef t4
#     undef f3
#     undef f2
#     undef n3
#     undef n2
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef fc
#     undef fi
//------------------------------------------------------------------------------
#   elif !defined(f)                                            // vary variadic
//------------------------------------------------------------------------------
//    Non-variadic function types
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#     define t(n,o_name) t4(n,o_name,BOOST_PP_EMPTY)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    Abstract/implicit non-variadic variant, if requested
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     ifdef BOOST_FT_GEN_ABSTRACT
#       define f f3
#       define n4 n3
#       include BOOST_FT_FRAGMENT(tags, invoke client)
#       undef  n4
#       undef  f
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    Explicit non-variadic variant (with identifier annotation and flag)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     define f f3 | BOOST_FT_NON_VARIADIC_FLAG
#     define n4 BOOST_PP_SEQ_CAT((BOOST_FT_NON_VARIADIC_PREFIX)(_)(n3))
#     include BOOST_FT_FRAGMENT(tags, invoke client)
#     undef  n4
#     undef  f
#     undef  t
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//    Variadic function types, if allowed/forced
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#     if fv
#       define f f3 | BOOST_FT_VARIADIC_FLAG
#       define n4 BOOST_PP_SEQ_CAT((BOOST_FT_VARIADIC_PREFIX)(_)(n3))
#       define t(n,o_name) t4(n,o_name,BOOST_PP_IDENTITY(...))
#       include BOOST_FT_FRAGMENT(tags, invoke client)
#       undef  t
#       undef  n4
#       undef  f
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef t
#     undef f
#     undef n4
//------------------------------------------------------------------------------
#   elif !defined(n)                                            // invoke client
//------------------------------------------------------------------------------
//    Prepend pending identifier prefix/suffix, if any and invoke the client
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     if BOOST_PP_TUPLE_ELEM(4,0,tn)
#       define n5 BOOST_PP_SEQ_CAT((BOOST_PP_TUPLE_ELEM(4,1,tn))(_)(n4))
#     else
#       define n5 n4
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     if BOOST_PP_TUPLE_ELEM(4,2,tn) == 1
#       define n BOOST_PP_SEQ_CAT((n5)(_)(BOOST_PP_TUPLE_ELEM(4,3,tn)))
#     else
#       define n n5
#     endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     include BOOST_FT_CLIENT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#     undef n
#     undef n5
#   endif
//------------------------------------------------------------------------------
#endif

