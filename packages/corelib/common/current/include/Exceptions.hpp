#ifndef ORO_EXCEPTIONS_HPP
#define ORO_EXCEPTIONS_HPP

#include <pkgconf/os.h>

/**
 * @file Exceptions.hpp
 * When Orocos is compiled without exceptions (define ORO_EMBEDDED), the functions
 * which would throw an exception simply return. For void
 * functions, use ORO_THROW, for other functions, use ORO_THROW_OR_RETURN.
 */

#ifndef ORO_EMBEDDED
#define ORO_THROW(x) throw x
#define ORO_THROW_OR_RETURN(x, rv) throw x
#else
#define ORO_THROW(x) return
#define ORO_THROW_OR_RETURN(x, rv) return rv
#endif

#endif
