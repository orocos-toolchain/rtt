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
 *		$Id: rotational_interpolation_singleaxis.cpp,v 1.1.1.1.2.2 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/rotational_interpolation_sa.h"
#include "geometry/trajectory.h"
 
#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


RotationalInterpolation_SingleAxis::RotationalInterpolation_SingleAxis()
	{};

void RotationalInterpolation_SingleAxis::SetStartEnd(Rotation start,Rotation end) {
	R_base_start = start;
	R_base_end   = end;	
	Rotation R_start_end = R_base_start.Inverse()*R_base_end;
	angle = R_start_end.GetRotAngle(rot_start_end);
}

Rotation RotationalInterpolation_SingleAxis::Pos(double theta) const {
	return R_base_start* Rotation::Rot2(rot_start_end,theta);	
}

Vector RotationalInterpolation_SingleAxis::Vel(double theta,double thetad) const {
	return R_base_start * ( rot_start_end*thetad );
}

Vector RotationalInterpolation_SingleAxis::Acc(double theta,double thetad,double thetadd) const {
	return R_base_start * ( rot_start_end* thetadd);
}

double RotationalInterpolation_SingleAxis::Angle() {
	return angle;
}

#ifdef OROINT_OS_STDIOSTREAM
void RotationalInterpolation_SingleAxis::Write(ostream& os) const {
	os << "SingleAxis[] " << endl;
}
#endif

RotationalInterpolation_SingleAxis::~RotationalInterpolation_SingleAxis() {
}


RotationalInterpolation* RotationalInterpolation_SingleAxis::Clone() const {
	return new RotationalInterpolation_SingleAxis();
}

#ifdef USE_NAMESPACE
}
#endif

