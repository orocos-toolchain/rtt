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
 *		$Id: velocityprofile_rect.cpp,v 1.1.1.1.2.5 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/error.h"
#include "geometry/velocityprofile_rect.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


void VelocityProfile_Rectangular::SetProfile(
	double pos1,
	double pos2
	) 
{
	double diff;
	diff = pos2-pos1;          // increment per sec.
    if ( diff != 0 )
        {
            v    = (diff>0)?maxvel:-maxvel;
            p    = pos1;               // start pos    
            d    = diff/v;
        }
    else
        {
            v = 0;
            p = pos1;
            d = 0;
        }
}

    void VelocityProfile_Rectangular::SetMax( double vMax )
    {
        maxvel = vMax;
    }
    

void VelocityProfile_Rectangular::
	SetProfileDuration(double pos1,double pos2,double duration)
{
	double diff;
	diff = pos2-pos1;          // increment per sec.
    if ( diff != 0 )
        {
            v    = diff/duration;
            if (v > maxvel || duration==0 ) // safety. 
                v=maxvel;
            p    = pos1;               // start pos    
            d    = diff/v;
        }
    else
        {
            v    = 0;
            p    = pos1;
            d    = duration;
        }
}

double VelocityProfile_Rectangular::Duration() const {
	return d;
}

double VelocityProfile_Rectangular::Pos(double time) const {
	return v*time+p;
}

double VelocityProfile_Rectangular::Vel(double time) const {
	return v;
}

double VelocityProfile_Rectangular::Acc(double time) const {
#ifdef HAVE_EXCEPTIONS
	throw Error_MotionPlanning_Incompatible();
#endif
	return 0;
}


#ifdef OROINT_OS_STDIOSTREAM
void VelocityProfile_Rectangular::Write(ostream& os) const {
	os << "CONSTVEL[" << maxvel << "]";
}
#endif



#ifdef USE_NAMESPACE
}
#endif

