/*****************************************************************************
*   ALTERNATIVE FOR trajectory_circle.h/cpp 
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
 *		$Id: geometry_circle.h,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_CIRCLE_H
#define GEOMETRY_CIRCLE_H

#include "geometry.h"
#include "orientation.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif



class Geometry_Circle : public Geometry 
	{

		// Orientatie gedeelte
		Orientation* orient; 

		// Circular gedeelte
		double radius;
		Frame  F_base_center;

		// equivalent radius
		double eqradius;  

		// verdeling baanlengte over pos/rot
		double pathlength;
		double scalelin;
		double scalerot;

		bool aggregate;

	public:

		/**
		 *
		 * CAN THROW Error_MotionPlanning_Circle_ToSmall 
		 * CAN THROW Error_MotionPlanning_Circle_No_Plane
		 */
		Geometry_Circle(const Frame& F_base_start,const Vector& V_base_center,
			const Vector& V_base_p,
			const Rotation& R_base_end,
			double alpha,
			Orientation* otraj,
			double eqradius,
			bool _aggregate=true);
		
		double LengthToS(double length);

		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const;
		virtual Twist Acc(double s,double sd,double sdd) const;
		virtual Geometry* Clone();
#if HAVE_IOSTREAM
		virtual void Write(ostream& os);
#endif
		virtual ~Geometry_Circle();
	};


#ifdef USE_NAMESPACE
}
#endif


#endif
