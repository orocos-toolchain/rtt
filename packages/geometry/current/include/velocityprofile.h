/*****************************************************************************
 *		 
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
 *		$Id: motionprofile.h,v 1.1.1.1.2.5 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef MOTIONPROFILE_H
#define MOTIONPROFILE_H

#include "utility.h"
#include "utility_io.h"





#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


    /** 
     * A MotionProfile stores the velocity profile that
     * is used within a trajectory. A velocity profile is the function that
     * expresses position, velocity and acceleration of a point on a curve
     * in function of time. It defines the how a point s moves on a path S.
     */
class MotionProfile 
	{
	public:
		// trajectory parameters are set in constructor of
		// derived class

		virtual void SetProfile(double pos1,double pos2) = 0;
		// sets a trajectory from pos1 to pos2 as fast as possible

		virtual void SetProfileDuration(
			double pos1,double pos2,double duration) = 0;
		// Sets a trajectory from pos1 to pos2 in <duration> seconds.
        // @post new.Duration() will not be shorter than the one obtained 
        //       from SetProfile(pos1,pos2).

		
		virtual double Duration() const = 0;
		// returns the duration of the motion in [sec]

		virtual double Pos(double time) const = 0;
		// returns the position at <time> in the units of the input
		// of the constructor of the derived class.

		virtual double Vel(double time) const = 0;
		// returns the velocity at <time> in the units of the input
		// of the constructor of the derived class.

		virtual double Acc(double time) const = 0;
		// returns the acceleration at <time> in the units of the input
		// of the constructor of the derived class.

#if HAVE_IOSTREAM
		virtual void Write(ostream& os) const = 0;
		// Writes object to a stream.

		static MotionProfile* Read(istream& is);
		// reads a MotionProfile object from the stream and returns it.
#endif

		virtual MotionProfile* Clone() = 0;
		// returns copy of current MotionProfile object. (virtual constructor)

		virtual ~MotionProfile() {}
	};


#ifdef USE_NAMESPACE
}
#endif


#endif
