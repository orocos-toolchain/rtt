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
 *		$Id: rotational_interpolation.h,v 1.1.1.1.2.2 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "frames.h"

#if HAVE_IOSTREAM
#include "frames_io.h"
#endif

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * Orientation specifies the rotational part of a geometric trajectory
 *    The different derived objects specify different methods for interpolating
 *    rotations.
 * * SetStartEnd should be called before 
 *   using the other methods
 * * The start and end position do NOT belong to the persistent state !  The owner of this
 *   object is responsible for setting these each time
 */
class Orientation
	{
	public:
		/**
		 * Set the start and end rotational_interpolation
		 */
		virtual void SetStartEnd(Rotation start,Rotation end) = 0;
		
		/**
		 * - Returns the angle value to move from start to end.
		 * This should have units radians, 
		 * - With Single Axis interp corresponds to the angle rotation
		 * - With Three Axis interp corresponds to the slowest of the three
		 * rotations.
		 */
		virtual double Angle() = 0;

		/**
		 * Returns the rotation matrix at angle theta
		 */
		virtual Rotation Pos(double theta) const = 0;

		/**
		 * Returns the rotational velocity at angle theta and with
		 * derivative of theta == thetad
		 */
		virtual Vector Vel(double theta,double thetad) const = 0;

		/**
		 * Returns the rotational acceleration at angle theta and with
		 * derivative of theta == thetad, and 2nd derivative of theta == thdd
		 */
		virtual Vector Acc(double theta,double thetad,double thetadd) const = 0;

#if HAVE_IOSTREAM
		/**
		 * Writes one of the derived objects to the stream
		 */ 
		virtual void Write(ostream& os) const = 0;

		/**
		 * Reads one of the derived objects from the stream and returns a pointer
		 * (factory method)
		 */
		static Orientation* Read(istream& is);
#endif

		/**
		 * virtual constructor,  construction by copying ..
		 */
		virtual Orientation* Clone() const = 0;
		
		virtual ~Orientation() {}
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
