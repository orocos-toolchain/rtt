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
 *		$Id: path_circle.h,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_CIRCLE_H
#define GEOMETRY_CIRCLE_H

#include "path.h"
#include "rotational_interpolation.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


	/**
	 * A circular Path with 'open ends'. Path_Arc would
	 * have been a better name though.
	 */
class Path_Circle : public Path 
	{

		// Orientatie gedeelte
		RotationalInterpolation* orient; 

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
		Path_Circle(const Frame& F_base_start,const Vector& V_base_center,
			const Vector& V_base_p,
			const Rotation& R_base_end,
			double alpha,
			RotationalInterpolation* otraj,
			double eqradius,
			bool _aggregate=true);
		
		double LengthToS(double length);

		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const;
		virtual Twist Acc(double s,double sd,double sdd) const;
		virtual Path* Clone();
#if OROINT_OS_STDIOSTREAM
		virtual void Write(ostream& os);
#endif
		virtual ~Path_Circle();
	};


#ifdef USE_NAMESPACE
}
#endif


#endif
