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
 *		$Id: geometry_cyclic_closed.cpp,v 1.1.1.1.2.5 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/geometry_cyclic_closed.h"
#include "geometry/error.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

Geometry_Cyclic_Closed::Geometry_Cyclic_Closed(Geometry* _geom,int _times, bool _aggregate):
times(_times),geom(_geom), aggregate(_aggregate) {}

double Geometry_Cyclic_Closed::LengthToS(double length) {
#ifdef HAVE_EXCEPTIONS
	throw Error_MotionPlanning_Not_Applicable();
#endif
	return 0;
}

double Geometry_Cyclic_Closed::PathLength(){
	return geom->PathLength()*times;
}

Frame Geometry_Cyclic_Closed::Pos(double s) const  {
	return geom->Pos( fmod(s,geom->PathLength()) );
}

Twist Geometry_Cyclic_Closed::Vel(double s,double sd) const  {
	return geom->Vel( fmod(s,geom->PathLength()),sd );
}

Twist Geometry_Cyclic_Closed::Acc(double s,double sd,double sdd) const  {
	return geom->Acc( fmod(s,geom->PathLength()),sd,sdd );
}


Geometry_Cyclic_Closed::~Geometry_Cyclic_Closed() {
    if (aggregate)
        delete geom;
}

Geometry* Geometry_Cyclic_Closed::Clone() {
	return new Geometry_Cyclic_Closed(geom->Clone(),times, aggregate);
}

#if HAVE_IOSTREAM
void Geometry_Cyclic_Closed::Write(ostream& os)  {
	os << "CYCLIC_CLOSED[ ";
	os << "  ";geom->Write(os);os << endl;
	os << "  " << times << endl;
	os << "]"  << endl;
}
#endif

#ifdef USE_NAMESPACE
}
#endif

