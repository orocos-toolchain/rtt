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
 *		$Id: trajectory.h,v 1.1.1.1.2.5 2003/07/23 16:44:25 psoetens Exp $
 *		$Name:  $ 
 *  \todo
 *     Peter's remark : should seperate I/O from other routines in the
 *     motion/chain directories
 *     The problem is that the I/O uses virtual inheritance to write
 *     the trajectories/geometries/velocityprofiles/...
 *     Have no good solution for this, perhaps
 *          * #ifdef's
 *          * declaring dummy ostream/istream and change implementation file .cpp
 *          * declaring some sort of VISITOR object (containing an ostream) , 
 *            the classes contain code to pass this object around along its children
 *            a subroutine can then be called with overloading.
 *     PROBLEM : if you declare a friend you have to fully declare it  ==> exposing I/O with ostream/istream decl
 *     CONSEQUENCE : everything has to be declared public.
 ****************************************************************************/

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "frames.h"
#include "frames_io.h"
#include "path.h"
#include "velocityprofile.h"



#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif




	class Trajectory 
	// An abstract class that implements
	// a trajectory contains a cartesian space trajectory and an underlying
	// velocity profile.
	{
	public:
        virtual Path*      GetPath() = 0;
        // The underlying Path

        virtual VelocityProfile* GetProfile() = 0;
        // The velocity profile
       
		virtual double Duration() const = 0;
		// The duration of the trajectory

		virtual Frame Pos(double time) const = 0;
		// Position of the trajectory at <time>.

		virtual Twist Vel(double time) const = 0;
		// The velocity of the trajectory at <time>.
		virtual Twist Acc(double time) const = 0;
		// The acceleration of the trajectory at <time>.

#if HAVE_IOSTREAM
		virtual void Write(ostream& os) const = 0;
		static Trajectory* Read(istream& is);
#endif
		virtual ~Trajectory() {}
		// note : you cannot declare this destructor abstract
		// it is always called by the descendant's destructor !
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
