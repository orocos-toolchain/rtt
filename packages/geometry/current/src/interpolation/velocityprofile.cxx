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
 *		$Id: velocityprofile.cpp,v 1.1.1.1.2.3 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/error.h"
#include "geometry/error_stack.h"
#include "geometry/velocityprofile_rect.h"
#include "geometry/velocityprofile_trap.h"
#include "geometry/velocityprofile_traphalf.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


#ifdef OROINT_OS_STDIOSTREAM
VelocityProfile* VelocityProfile::Read(istream& is) {
	IOTrace("VelocityProfile::Read");
	char storage[25];
	EatWord(is,"[",storage,sizeof(storage));
	Eat(is,'[');
	if (strcmp(storage,"CONSTVEL")==0) {
		double vel;
		is >> vel;
		Eat(is,']');
		IOTracePop();
		return new VelocityProfile_Rectangular(vel);
	} else if (strcmp(storage,"TRAPEZOIDAL")==0) {
		double maxvel;
		double maxacc;
 		is >> maxvel;
		Eat(is,',');
		is >> maxacc;
		Eat(is,']');
		IOTracePop();
		return new VelocityProfile_Trap(maxvel,maxacc);
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
		return new VelocityProfile_TrapHalf(maxvel,maxacc,starting);
	}
	else {
#ifdef HAVE_EXCEPTIONS
		throw Error_MotionIO_Unexpected_MotProf();
#endif
	}
    return 0;
}
#endif



#ifdef USE_NAMESPACE
}
#endif
