#ifndef MOTIONPROFILE_DIRAC_H
#define MOTIONPROFILE_DIRAC_H

#include "velocityprofile.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif
	/**
	 * A Dirac VelocityProfile generates an infinite velocity
	 * so that the position jumps from A to B in in infinite short time.
	 * In practice, this means that the maximum values are ignored and
	 * for any t : Vel(t) == 0 and Acc(t) == 0.
	 * Further Pos( -0 ) = pos1 and Pos( +0 ) = pos2.
	 *
	 * However, if a duration is given, it will create an unbound
	 * rectangular velocity profile for that duration, otherwise,
	 * Duration() == 0;
	 */
	class VelocityProfile_Dirac : public VelocityProfile 
	{
		double p1,p2,t;
	public:
		void SetProfile(double pos1,double pos2);
		virtual void SetProfileDuration(double pos1,double pos2,double duration);
		virtual double Duration() const;
		virtual double Pos(double time) const;
		virtual double Vel(double time) const;
		virtual double Acc(double time) const;
#if OROINT_OS_STDIOSTREAM
		virtual void Write(ostream& os) const;
#endif
		virtual VelocityProfile* Clone() {
			return new VelocityProfile_Dirac();
		}

		virtual ~VelocityProfile_Dirac() {}
	};


#ifdef USE_NAMESPACE
}
#endif


#endif
