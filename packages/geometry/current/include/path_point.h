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
 *		$Id: path_point.h,v 1.1.2.3 2003/07/24 13:40:49 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_POINT_H
#define GEOMETRY_POINT_H

#include "path.h"
#include "rotational_interpolation.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif



class Path_Point : public Path
	{
		// Lineair gedeelte
		Frame F_base_start;
	public:
		/** 
		 * Constructs a Point Path
		 */
		Path_Point(const Frame& F_base_start);
		double LengthToS(double length);
		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const ;
		virtual Twist Acc(double s,double sd,double sdd) const;
#if OROINT_OS_STDIOSTREAM
		virtual void Write(ostream& os);
#endif
		virtual Path* Clone();
		virtual ~Path_Point();
	};


#ifdef USE_NAMESPACE
}
#endif


#endif
