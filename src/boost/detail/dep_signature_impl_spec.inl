
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------
// Dependecies common for both preprocessed and preprocessing mode versions
// are kept separate to allow easier preprocessing.
//------------------------------------------------------------------------------
#if   BOOST_FT_MAX_ARITY <=  8
#   include <boost/mpl/vector/vector10.hpp>
#elif BOOST_FT_MAX_ARITY <= 18
#   include <boost/mpl/vector/vector20.hpp>
#elif BOOST_FT_MAX_ARITY <= 28
#   include <boost/mpl/vector/vector30.hpp>
#elif BOOST_FT_MAX_ARITY <= 38
#   include <boost/mpl/vector/vector40.hpp>
#elif BOOST_FT_MAX_ARITY <= 48
#   include <boost/mpl/vector/vector50.hpp>
#else
#   error "Default limit of mpl::vector exceeded."
#endif
//------------------------------------------------------------------------------

// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------
// Dependecies common for both preprocessed and preprocessing mode versions
// are kept separate to allow easier preprocessing.
//------------------------------------------------------------------------------
#if   BOOST_FT_MAX_ARITY <=  8
#   include <boost/mpl/vector/vector10.hpp>
#elif BOOST_FT_MAX_ARITY <= 18
#   include <boost/mpl/vector/vector20.hpp>
#elif BOOST_FT_MAX_ARITY <= 28
#   include <boost/mpl/vector/vector30.hpp>
#elif BOOST_FT_MAX_ARITY <= 38
#   include <boost/mpl/vector/vector40.hpp>
#elif BOOST_FT_MAX_ARITY <= 48
#   include <boost/mpl/vector/vector50.hpp>
#else
#   error "Default limit of mpl::vector exceeded."
#endif
//------------------------------------------------------------------------------
