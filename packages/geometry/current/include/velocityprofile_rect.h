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
 *		$Id: velocityprofile_rect.h,v 1.1.1.1.2.4 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef MOTIONPROFILE_RECT_H
#define MOTIONPROFILE_RECT_H

#include "velocityprofile.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

	class VelocityProfile_Rectangular : public VelocityProfile 
		// Defines a rectangular velocityprofile.
		// (i.e. constant velocity)
	{
		double d,p,v;
	public:
		double maxvel;
	
		VelocityProfile_Rectangular(double _maxvel):
		  maxvel(_maxvel) {}
		// constructs motion profile class with <maxvel> as parameter of the
		// trajectory.

		void SetMax( double _maxvel );
		void SetProfile(double pos1,double pos2);
		virtual void SetProfileDuration(
			double pos1,double pos2,double duration);
		virtual double Duration() const;
		virtual double Pos(double time) const;
		virtual double Vel(double time) const;
		virtual double Acc(double time) const;
#if OROINT_OS_STDIOSTREAM
		virtual void Write(ostream& os) const;
#endif
		virtual VelocityProfile* Clone() {
			return new VelocityProfile_Rectangular(maxvel);
		}
		// returns copy of current VelocityProfile object. (virtual constructor)
		virtual ~VelocityProfile_Rectangular() {}
	};


#ifdef USE_NAMESPACE
}
#endif


#endif
