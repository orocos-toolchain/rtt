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
 *		$Id: geometry_point.cpp,v 1.1.2.3 2003/07/24 13:40:49 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/geometry_point.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

Geometry_Point::Geometry_Point(const Frame& startpos)
    :F_base_start(startpos)
   {	
   }

double Geometry_Point::LengthToS(double length) {
	return length;
}
double Geometry_Point::PathLength(){
	return 0;
}
Frame Geometry_Point::Pos(double s) const  {
	return F_base_start;
}

Twist Geometry_Point::Vel(double s,double sd) const  {
	return Twist::Zero();
}

Twist Geometry_Point::Acc(double s,double sd,double sdd) const  {
	return Twist::Zero();
}

Geometry_Point::~Geometry_Point() {
}

Geometry* Geometry_Point::Clone() {
	return new Geometry_Point( F_base_start	);
}

#if HAVE_IOSTREAM
void Geometry_Point::Write(ostream& os)  {
	os << "POINT[ "<< F_base_start  << "]"  << endl;
}

#endif

#ifdef USE_NAMESPACE
}
#endif

