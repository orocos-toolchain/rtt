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


#include "geometry/rotational_interpolation_singleaxis.h"
#include "geometry/trajectory.h"
 
#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


Orientation_SingleAxis::Orientation_SingleAxis()
	{};

void Orientation_SingleAxis::SetStartEnd(Rotation start,Rotation end) {
	R_base_start = start;
	R_base_end   = end;	
	Rotation R_start_end = R_base_start.Inverse()*R_base_end;
	angle = R_start_end.GetRotAngle(rot_start_end);
}

Rotation Orientation_SingleAxis::Pos(double theta) const {
	return R_base_start* Rotation::Rot2(rot_start_end,theta);	
}

Vector Orientation_SingleAxis::Vel(double theta,double thetad) const {
	return R_base_start * ( rot_start_end*thetad );
}

Vector Orientation_SingleAxis::Acc(double theta,double thetad,double thetadd) const {
	return R_base_start * ( rot_start_end* thetadd);
}

double Orientation_SingleAxis::Angle() {
	return angle;
}

#ifdef OROINT_OS_STDIOSTREAM
void Orientation_SingleAxis::Write(ostream& os) const {
	os << "SingleAxis[] " << endl;
}
#endif

Orientation_SingleAxis::~Orientation_SingleAxis() {
}


Orientation* Orientation_SingleAxis::Clone() const {
	return new Orientation_SingleAxis();
}

#ifdef USE_NAMESPACE
}
#endif

