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
 *		$Id: geometry_composite.h,v 1.1.1.1.2.5 2003/07/24 13:49:16 rwaarsin Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_COMPOSITE_H
#define GEOMETRY_COMPOSITE_H

#include "frames.h"
#include "frames_io.h"
#include "geometry.h"
#include <vector>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * The specification of the geometry of a trajectory.
 */
class Geometry_Composite : public Geometry
	{
		typedef std::vector< std::pair<Geometry*,bool> > GeometryVector;
		typedef std::vector<double>    DoubleVector;

		GeometryVector gv;
		DoubleVector   dv;
		double pathlength;

		// lookup mechanism : 
		mutable double cached_starts;
		mutable double cached_ends;
		mutable int    cached_index;
		double Lookup(double s) const;
	public:

		
		Geometry_Composite();

		/**
		 * Adds a Geometry* to this composite
		 */
		void Add(Geometry* geom, bool aggregate=true);


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

		virtual Geometry* Clone();

#if HAVE_IOSTREAM
		/**
		 * Writes one of the derived objects to the stream
		 */ 
		virtual void Write(ostream& os);
#endif
		
		virtual ~Geometry_Composite();
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
