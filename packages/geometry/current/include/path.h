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
 *		$Id: geometry.h,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "frames.h"

#ifdef HAVE_RTSTL
#include <rtstl/rtstl.hpp>
#else
#include <vector>
#endif

#if HAVE_IOSTREAM
#include "frames_io.h"
#endif

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * The specification of the geometry of a trajectory.
 */
class Geometry
	{
	public:

		/**
		 * LengthToS() converts a physical length along the trajectory
		 * to the parameter s used in Pos, Vel and Acc.  This is used because
		 * in cases with large rotations the parameter s does NOT correspond to
		 * the lineair length along the trajectory.
		 * User should be sure that the lineair distance travelled by this 
		 * geometry object is NOT zero, when using this method !
		 * (e.g. the case of only rotational change)
		 * throws Error_MotionPlanning_Not_Applicable if used on composed
		 * geometry objects.
		 */
		virtual double LengthToS(double length)  = 0;
		
		/**
		 * Returns the total path length of the trajectory
		 * (has dimension LENGTH)
		 * This is not always a physical length , ie when dealing with rotations
		 * that are dominant.
		 */
		virtual double PathLength() = 0;

		/**
		 * Returns the Frame at the current path length s
		 */
		virtual Frame Pos(double s) const = 0;

		/**
		 * Returns the velocity twist at path length s theta and with
		 * derivative of s == sd
		 */
		virtual Twist Vel(double s,double sd) const  = 0;

		/**
		 * Returns the acceleration twist at path length s and with
		 * derivative of s == sd, and 2nd derivative of s == sdd
		 */
		virtual Twist Acc(double s,double sd,double sdd) const  = 0;

#if HAVE_IOSTREAM
		/**
		 * Writes one of the derived objects to the stream
		 */ 
		virtual void Write(ostream& os)  = 0;

		/**
		 * Reads one of the derived objects from the stream and returns a pointer
		 * (factory method)
		 */
		static Geometry* Read(istream& is);
#endif

		/**
		 * Virtual constructor, constructing by copying,
		 * Returns a deep copy of this Geometry Object
		 */
		virtual Geometry* Clone() = 0;

		virtual ~Geometry() {}
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
