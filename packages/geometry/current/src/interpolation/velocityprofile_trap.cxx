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
 *		$Id: velocityprofile_trap.cpp,v 1.1.1.1.2.7 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


//#include "error.h"
#include "geometry/velocityprofile_trap.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


VelocityProfile_Trap::VelocityProfile_Trap(double _maxvel,double _maxacc):
		  maxvel(_maxvel),maxacc(_maxacc) {}
		// constructs motion profile class with <maxvel> as parameter of the
		// trajectory.

void VelocityProfile_Trap::SetProfile(double pos1,double pos2) {
	startpos = pos1;
	endpos   = pos2;
	t1 = maxvel/maxacc;
	double s       = sign(endpos-startpos);
	double deltax1 = s*maxacc*sqr(t1)/2.0;
	double deltaT  = (endpos-startpos-2.0*deltax1) / (s*maxvel);
	if (deltaT > 0.0) {
		// plan a complete profile :
		duration = 2*t1+deltaT;
		t2 = duration - t1;
	} else {
		// plan an incomplete profile :
		t1 = ::sqrt((endpos-startpos)/s/maxacc);
		duration = t1*2.0;
		t2=t1;
	}
	a3 = s*maxacc/2.0;
	a2 = 0;
	a1 = startpos;

	b3 = 0;
	b2 = a2+2*a3*t1 - 2.0*b3*t1;
	b1 = a1+t1*(a2+a3*t1) - t1*(b2+t1*b3);

	c3 = -s*maxacc/2.0;
	c2 = b2+2*b3*t2 - 2.0*c3*t2;
	c1 = b1+t2*(b2+b3*t2) - t2*(c2+t2*c3);
}

void VelocityProfile_Trap::SetProfileDuration(
	double pos1,double pos2,double newduration) {
	// duration should be longer than originally planned duration
    // Fastest :
	SetProfile(pos1,pos2);
    // Must be Slower  :
	double factor = duration/newduration;
    if (factor > 1)
        return; // do not exceed max
	a2*=factor;
	a3*=factor*factor;
	b2*=factor;
	b3*=factor*factor;
	c2*=factor;
	c3*=factor*factor;
	duration = newduration;
	t1 /= factor;
	t2 /= factor;
}

void VelocityProfile_Trap::SetMax(double _maxvel,double _maxacc)
{
    maxvel = _maxvel; maxacc = _maxacc;
}

double VelocityProfile_Trap::Duration() const {
	return duration;
}

double VelocityProfile_Trap::Pos(double time) const {
	if (time < 0) {
		return startpos;
	} else if (time<t1) {
		return a1+time*(a2+a3*time);
	} else if (time<t2) {
		return b1+time*(b2+b3*time);
	} else if (time<=duration) {
		return c1+time*(c2+c3*time);
	} else {
		return endpos;
	}
}
double VelocityProfile_Trap::Vel(double time) const {
	if (time < 0) {
		return 0;
	} else if (time<t1) {
		return a2+2*a3*time;
	} else if (time<t2) {
		return b2+2*b3*time;
	} else if (time<=duration) {
		return c2+2*c3*time;
	} else {
		return 0;
	}
}

double VelocityProfile_Trap::Acc(double time) const {
	if (time < 0) {
		return 0;
	} else if (time<t1) {
		return 2*a3;
	} else if (time<t2) {
		return 2*b3;
	} else if (time<=duration) {
		return 2*c3;
	} else {
		return 0;
	}
}

VelocityProfile* VelocityProfile_Trap::Clone() {
	return new VelocityProfile_Trap(maxvel,maxacc);
}

VelocityProfile_Trap::~VelocityProfile_Trap() {}


#ifdef OROINT_OS_STDIOSTREAM
void VelocityProfile_Trap::Write(ostream& os) const {
	os << "TRAPEZOIDAL[" << maxvel << "," << maxacc <<"]";
}
#endif





#ifdef USE_NAMESPACE
}
#endif

