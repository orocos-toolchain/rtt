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
 *		$Id: path_point.cpp,v 1.1.2.3 2003/07/24 13:40:49 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/path_point.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

Path_Point::Path_Point(const Frame& startpos)
    :F_base_start(startpos)
   {	
   }

double Path_Point::LengthToS(double length) {
	return length;
}
double Path_Point::PathLength(){
	return 0;
}
Frame Path_Point::Pos(double s) const  {
	return F_base_start;
}

Twist Path_Point::Vel(double s,double sd) const  {
	return Twist::Zero();
}

Twist Path_Point::Acc(double s,double sd,double sdd) const  {
	return Twist::Zero();
}

Path_Point::~Path_Point() {
}

Path* Path_Point::Clone() {
	return new Path_Point( F_base_start	);
}

#if HAVE_IOSTREAM
void Path_Point::Write(ostream& os)  {
	os << "POINT[ "<< F_base_start  << "]"  << endl;
}

#endif

#ifdef USE_NAMESPACE
}
#endif

