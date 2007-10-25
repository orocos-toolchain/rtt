
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

#ifndef BOOST_FT_DETAIL_CONFIG_HPP_INCLUDED
#define BOOST_FT_DETAIL_CONFIG_HPP_INCLUDED
//==============================================================================
// User configuration
//==============================================================================
// These macros have to be defined before the inclusion of the first library 
// header in order to take effect.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The default settings in this file must be in sync with the preprocessed files
// for the library to work correctly.
// The preprocessed files can be recreated by preprocessing and running the perl
// script libs/function_types/tools/preprocess.pl.
//==============================================================================
// Maximum arity
//------------------------------------------------------------------------------
#ifndef BOOST_FT_MAX_ARITY 
#   define BOOST_FT_MAX_ARITY 10
#elif !defined(BOOST_FT_PREPROCESSING_MODE)
#   define BOOST_FT_PREPROCESSING_MODE
#endif
//------------------------------------------------------------------------------
// Calling conventions
//------------------------------------------------------------------------------
// Custom calling conventions are disabled by default since they are not part of
// the C++ language standard. 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef BOOST_FT_CALLING_CONVENTIONS
#   define BOOST_FT_CALLING_CONVENTIONS \
             ( (( defaultcall, -, 1 )) \
             , (( defaultcall, -, 1 )) )
#elif !defined(BOOST_FT_PREPROCESSING_MODE)
#   define BOOST_FT_PREPROCESSING_MODE /**/
#endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Example configuration for custom calling conventions:                      //
//                                                                            //
//    #define BOOST_FT_CALLING_CONVENTIONS \                                  //
//              /*---------------------------------------*/ \                 //
//              /* name       | modifier \  allows '...' */ \                 //
//              /*------------|------------\-------------*/ \                 //
//              /*  [  non-member or static function  ]  */ \                 //
//            ( (( defaultcall,     -       , 0          )) \                 //
//              (( cdecl      , __cdecl     , 1          )) \                 //
//              (( stdcall    , __stdcall   , 0          )) \                 //
//              (( fastcall   , __fastcall  , 0          )) \                 //
//              /*  [     member function pointer     ]  */ \                 //
//            , (( defaultcall,     -       , 1          )) )                 //
//                                                                            //
// The example enables cdecl with-, stdcall and fastcall calling conventions  //
// without support for variadic function for non-member/static functions,     //
// The names used for tag identifiers are the same as their modifiers but     //
// without the double underscore prefix. The explicit name for the default    //
// calling convention is 'defaultcall'.                                       //
//______________________________________________________________________________
//==============================================================================
// Loader for preprocessed files
//==============================================================================
#ifdef BOOST_FT_PREPROCESSING_MODE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define BOOST_FT_PREPROCESSED(facility) \
      <rtt/boost/function_types/detail/gen_ ## facility.inl>

#   define BOOST_FT_FRAGMENT(facility,comment) \
      <rtt/boost/function_types/detail/gen_ ## facility.inl>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#else // def } BOOST_FT_PREPROCESSING_MODE { ndef
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   define BOOST_FT_PREPROCESSED(facility) \
      <rtt/boost/function_types/detail/pp_ ## facility.inl>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif // ndef BOOST_FT_PREPROCESSING_MODE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define BOOST_FT_DEPENDENCIES(facility) \
  <rtt/boost/function_types/detail/dep_ ## facility.inl>
//------------------------------------------------------------------------------
#endif // ndef BOOST_FT_DETAIL_CONFIG_HPP_INCLUDED
