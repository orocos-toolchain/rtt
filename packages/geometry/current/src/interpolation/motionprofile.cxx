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
 *		$Id: motionprofile.cpp,v 1.1.1.1.2.3 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/error.h"
#include "geometry/error_stack.h"
#include "geometry/motionprofile_rect.h"
#include "geometry/motionprofile_trap.h"
#include "geometry/motionprofile_traphalf.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


#ifdef OROINT_OS_STDIOSTREAM
MotionProfile* MotionProfile::Read(istream& is) {
	IOTrace("MotionProfile::Read");
	char storage[25];
	EatWord(is,"[",storage,sizeof(storage));
	Eat(is,'[');
	if (strcmp(storage,"CONSTVEL")==0) {
		double vel;
		is >> vel;
		Eat(is,']');
		IOTracePop();
		return new MotionProfile_Rectangular(vel);
	} else if (strcmp(storage,"TRAPEZOIDAL")==0) {
		double maxvel;
		double maxacc;
 		is >> maxvel;
		Eat(is,',');
		is >> maxacc;
		Eat(is,']');
		IOTracePop();
		return new MotionProfile_Trap(maxvel,maxacc);
	} else if (strcmp(storage,"TRAPEZOIDALHALF")==0) {
		double maxvel;
		double maxacc;
 		is >> maxvel;
		Eat(is,',');
		is >> maxacc;
		Eat(is,',');
		bool starting;
		is >> starting;
		Eat(is,']');
		IOTracePop();
		return new MotionProfile_TrapHalf(maxvel,maxacc,starting);
	}
	else {
#ifdef HAVE_EXCEPTIONS
		throw Error_MotionIO_Unexpected_MotProf();
#endif
	}
}
#endif



#ifdef USE_NAMESPACE
}
#endif
