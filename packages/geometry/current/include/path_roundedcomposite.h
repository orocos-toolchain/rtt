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
 *		$Id: path_roundedcomposite.h,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_ROUNDEDCOMPOSITE_H
#define GEOMETRY_ROUNDEDCOMPOSITE_H

#include "path.h"
#include "path_composite.h"
#include "rotational_interpolation.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * The specification of the geom
 etry of a trajectory.
 */
class Path_RoundedComposite : public Path
	{
		/** a Path_Composite is aggregated to hold the rounded trajectory
		 * with circles and lines
		 */
		Path_Composite* comp;


		double radius;
		double eqradius;
		Orientation* orient;
		// cached from underlying path objects for generating the rounding :
		Frame F_base_start;
		Frame F_base_via;
		//Frame F_base_end;
		int nrofpoints;

		bool aggregate;
	public:

		/**
		 * @param radius : radius of the rounding circles
		 * @param eqradius : equivalent radius to compare rotations/velocities
		 * @param orient   : method of rotational_interpolation interpolation
		 */
		Path_RoundedComposite(double radius,double _eqradius,Orientation* _orient, bool _aggregate=true);





		/**
		 * Adds a point to this rounded composite, between to adjecent points
		 * a Path_Line will be created, between two lines there will be
		 * rounding with the given radius with a Path_Circle
		 * Can throw Error_MotionPlanning_Not_Feasible object
		 */
		void Add(const Frame& F_base_point);

		/**
		 * to be called after the last line is added to finish up 
		 * the work
		 */
		void Finish();


		virtual double LengthToS(double length);

		/**
		 * Returns the total path length of the trajectory
		 * (has dimension LENGTH)
		 * This is not always a physical length , ie when dealing with rotations
		 * that are dominant.
		 */
		virtual double PathLength();

		/**
		 * Returns the Frame at the current path length s
		 */
		virtual Frame Pos(double s) const;

		/**
		 * Returns the velocity twist at path length s theta and with
		 * derivative of s == sd
		 */
		virtual Twist Vel(double s,double sd) const;

		/**
		 * Returns the acceleration twist at path length s and with
		 * derivative of s == sd, and 2nd derivative of s == sdd
		 */
		virtual Twist Acc(double s,double sd,double sdd) const;

		/**
		 * virtual constructor, constructing by copying.
		 * In this case it returns the Clone() of the aggregated Path_Composite
		 * because this is all one ever will need.
		 */
		virtual Path* Clone();

#if HAVE_IOSTREAM		
		/**
		 * Writes one of the derived objects to the stream
		 */ 
		virtual void Write(ostream& os);
#endif
		
		virtual ~Path_RoundedComposite();
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
