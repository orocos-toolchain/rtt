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
 *		$Id: path_line.cpp,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/path_line.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

Path_Line::Path_Line(const Frame& startpos,
		   const Frame& endpos,
		   RotationalInterpolation* _orient,
		   double _eqradius,
           bool _aggregate ):
			   orient(_orient),
			   V_base_start(startpos.p),
			   V_base_end(endpos.p),
			   eqradius(_eqradius),
               aggregate(_aggregate)
   {	
	   	V_start_end = V_base_end - V_base_start;
	   	double dist = V_start_end.Normalize();
		orient->SetStartEnd(startpos.M,endpos.M);
		double alpha = orient->Angle();

		// See what has the slowest eq. motion, and adapt 
		// the other to this slower motion
		// use eqradius to transform between rot and transl.
		if (alpha*eqradius > dist) {
			// rotational_interpolation is the limitation
			pathlength = alpha*eqradius;
			scalerot   = 1/eqradius;
			scalelin   = dist/pathlength;
		} else {
			// translation is the limitation
			pathlength = dist;
			scalerot   = alpha/pathlength;
			scalelin   = 1;
		}

   }

double Path_Line::LengthToS(double length) {
	return length/scalelin;
}
double Path_Line::PathLength(){
	return pathlength;
}
Frame Path_Line::Pos(double s) const  {
	return Frame(orient->Pos(s*scalerot),V_base_start + V_start_end*s*scalelin );
}

Twist Path_Line::Vel(double s,double sd) const  {
	return Twist( V_start_end*sd*scalelin, orient->Vel(s*scalerot,sd*scalerot) );
}

Twist Path_Line::Acc(double s,double sd,double sdd) const  {
	return Twist( V_start_end*sdd*scalelin, orient->Acc(s*scalerot,sd*scalerot,sdd*scalerot) );
}


Path_Line::~Path_Line() {
    if (aggregate)
        delete orient;
}

Path* Path_Line::Clone() {
	return new Path_Line(
		Frame(orient->Pos(0),V_base_start),
		Frame(orient->Pos(pathlength*scalerot),V_base_end),
		orient->Clone(),
		eqradius, 
        aggregate
	);
}

#if HAVE_IOSTREAM
void Path_Line::Write(ostream& os)  {
	os << "LINE[ ";
	os << "  " << Frame(orient->Pos(0),V_base_start) << endl;
	os << "  " << Frame(orient->Pos(pathlength*scalerot),V_base_end) << endl;
	os << "  ";orient->Write(os);
	os << "  " << eqradius;
	os << "]"  << endl;
}

#endif

#ifdef USE_NAMESPACE
}
#endif

