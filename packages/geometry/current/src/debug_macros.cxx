/*****************************************************************************
 *  \author 
 *  	Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *		ORO_Geometry V0.2
 *
 *	\par History
 *		- $log$
 *
 *	\par Release
 *		$Id: debug_macros.cpp,v 1.1.1.1.2.1 2003/01/06 16:14:02 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#include "geometry/debug_macros.h"
#ifdef WANT_STD_IOSTREAM
    using namespace std;
#endif

ofstream& logger() {
  static ofstream log("Verify.log",ios::app);
  return log;
}
ofstream& errorlogger() {
  static ofstream errorlog("Error.log",ios::app);
  return errorlog;
}
int DEBUG_RETURNVALUE = 0;
