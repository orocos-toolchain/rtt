/****************************************
*										*
*	Implementation of rint for MSVC		*
*	Date: March 12, 2009				*
*	Author: David Yu					*
*										*
****************************************/
#include "os/rint.h"
#include <cmath>

#ifdef _MSC_VER
double rint( double value )
  {
	return floor( value + 0.5 );
  }
#endif
