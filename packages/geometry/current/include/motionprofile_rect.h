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
 *		$Id: motionprofile_rect.h,v 1.1.1.1.2.4 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef MOTIONPROFILE_RECT_H
#define MOTIONPROFILE_RECT_H

#include "motionprofile.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

	class MotionProfile_Rectangular : public MotionProfile 
		// Defines a rectangular motionprofile.
		// (i.e. constant velocity)
	{
		double d,p,v;
	public:
		double maxvel;
	
		MotionProfile_Rectangular(double _maxvel):
		  maxvel(_maxvel) {}
		// constructs motion profile class with <maxvel> as parameter of the
		// trajectory.

		void SetProfile(double pos1,double pos2);
		virtual void SetProfileDuration(
			double pos1,double pos2,double duration);
		virtual double Duration() const;
		virtual double Pos(double time) const;
		virtual double Vel(double time) const;
		virtual double Acc(double time) const;
#if HAVE_IOSTREAM
		virtual void Write(ostream& os) const;
#endif
		virtual MotionProfile* Clone() {
			return new MotionProfile_Rectangular(maxvel);
		}
		// returns copy of current MotionProfile object. (virtual constructor)
		virtual ~MotionProfile_Rectangular() {}
	};


#ifdef USE_NAMESPACE
}
#endif


#endif
