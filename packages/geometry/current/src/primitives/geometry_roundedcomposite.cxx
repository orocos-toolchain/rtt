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
 *		$Id: geometry_roundedcomposite.cpp,v 1.1.1.1.2.5 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/geometry_roundedcomposite.h"
#include "geometry/geometry_line.h"
#include "geometry/geometry_circle.h"
#include "geometry/error.h"
#include <memory>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

Geometry_RoundedComposite::Geometry_RoundedComposite(double _radius,double _eqradius,Orientation* _orient, bool _aggregate) :
	comp( new Geometry_Composite()), radius(_radius),eqradius(_eqradius), orient(_orient), aggregate(_aggregate)
{
		nrofpoints = 0;
}

void Geometry_RoundedComposite::Add(const Frame& F_base_point) {
	if (nrofpoints==0) {
		F_base_start = F_base_point;
	} else if (nrofpoints==1) {
		F_base_via   = F_base_point;
	} else {
		// calculate rounded segment : line + circle,
		// determine the angle between the line segments :			
			Vector ab = F_base_via.p - F_base_start.p;
			Vector bc = F_base_point.p - F_base_via.p;
			double abdist = ab.Normalize();
			double alpha  = ::acos(dot(ab,bc));
			double d      = radius/tan((PI-alpha)/2);
#ifdef HAVE_EXCEPTIONS
            double bcdist = bc.Normalize();
			if (d >= abdist) 
				throw Error_MotionPlanning_Not_Feasible();
			if (d >= bcdist) 
				throw Error_MotionPlanning_Not_Feasible();
#endif
			std::auto_ptr<Geometry> line1 (
				new Geometry_Line(F_base_start,F_base_via,orient->Clone(),eqradius)
			);
			std::auto_ptr<Geometry> line2 (
				new Geometry_Line(F_base_via,F_base_point,orient->Clone(),eqradius)
			);
			Frame F_base_circlestart     = line1->Pos(line1->LengthToS(abdist-d));
			Frame F_base_circleend       = line2->Pos(line2->LengthToS(d));  
				// end of circle segment, beginning of next line
			Vector V_base_t  = ab*(ab*bc);
			V_base_t.Normalize();
			comp->Add(new Geometry_Line(
						F_base_start,F_base_circlestart,orient->Clone(),eqradius
						)
					);
			comp->Add(new Geometry_Circle(
					F_base_circlestart,
					F_base_circlestart.p - V_base_t * radius,
					F_base_circleend.p,
					F_base_circleend.M,
					PI-alpha,orient->Clone(),eqradius
				)
			);
		// shift for next line
		F_base_start = F_base_circleend;  // end of the circle segment
		F_base_via   = F_base_point;
	}
	nrofpoints++;
}

void Geometry_RoundedComposite::Finish() {
	if (nrofpoints>=1) {
		comp->Add(new Geometry_Line(F_base_start,F_base_via,orient->Clone(),eqradius));
	}
}

double Geometry_RoundedComposite::LengthToS(double length)  {
	return comp->LengthToS(length);
}

double Geometry_RoundedComposite::PathLength() {
	return comp->PathLength();
}

Frame Geometry_RoundedComposite::Pos(double s) const {
	return comp->Pos(s);
}

Twist Geometry_RoundedComposite::Vel(double s,double sd) const {
	return comp->Vel(s,sd);
}

Twist Geometry_RoundedComposite::Acc(double s,double sd,double sdd) const {
	return comp->Acc(s,sd,sdd);
}

#if HAVE_IOSTREAM
void Geometry_RoundedComposite::Write(ostream& os)  {
	comp->Write(os);
}
#endif
		
Geometry_RoundedComposite::~Geometry_RoundedComposite() {
    if (aggregate)
        delete orient;
	delete comp;
}


Geometry* Geometry_RoundedComposite::Clone() {
	return comp->Clone();
}

#ifdef USE_NAMESPACE
}
#endif
