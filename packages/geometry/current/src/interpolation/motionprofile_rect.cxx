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
 *		$Id: motionprofile_rect.cpp,v 1.1.1.1.2.5 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/error.h"
#include "geometry/motionprofile_rect.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


void MotionProfile_Rectangular::SetProfile(
	double pos1,
	double pos2
	) 
{
	double diff;
	diff = pos2-pos1;          // increment per sec.
	v    = (diff>0)?maxvel:-maxvel;
	p    = pos1;               // start pos    
	d    = diff/v;
}

void MotionProfile_Rectangular::
	SetProfileDuration(double pos1,double pos2,double duration)
{
	double diff;
	diff = pos2-pos1;          // increment per sec.
	v    = diff/duration;
    if (v > maxvel) 
        v=maxvel;
	p    = pos1;               // start pos    
	d    = diff/v;
}

double MotionProfile_Rectangular::Duration() const {
	return d;
}

double MotionProfile_Rectangular::Pos(double time) const {
	return v*time+p;
}

double MotionProfile_Rectangular::Vel(double time) const {
	return v;
}

double MotionProfile_Rectangular::Acc(double time) const {
#ifdef HAVE_EXCEPTIONS
	throw Error_MotionPlanning_Incompatible();
#endif
	return 0;
}


#ifdef OROINT_OS_STDIOSTREAM
void MotionProfile_Rectangular::Write(ostream& os) const {
	os << "CONSTVEL[" << maxvel << "]";
}
#endif



#ifdef USE_NAMESPACE
}
#endif

