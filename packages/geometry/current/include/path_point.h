/*****************************************************************************
*   ALTERNATIVE FOR trajectory_stationary.h/cpp 
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
 *		$Id: geometry_point.h,v 1.1.2.3 2003/07/24 13:40:49 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_POINT_H
#define GEOMETRY_POINT_H

#include "geometry.h"
#include "orientation.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif



class Geometry_Point : public Geometry
	{
		// Lineair gedeelte
		Frame F_base_start;
	public:
		/** 
		 * Constructs a Point Geometry
		 */
		Geometry_Point(const Frame& F_base_start);
		double LengthToS(double length);
		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const ;
		virtual Twist Acc(double s,double sd,double sdd) const;
#if HAVE_IOSTREAM
		virtual void Write(ostream& os);
#endif
		virtual Geometry* Clone();
		virtual ~Geometry_Point();
	};


#ifdef USE_NAMESPACE
}
#endif


#endif
