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
 *		$Id: velocityprofile_trap.h,v 1.1.1.1.2.5 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef MOTIONPROFILE_TRAP_H
#define MOTIONPROFILE_TRAP_H

#include "velocityprofile.h"




#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif



class VelocityProfile_Trap : public VelocityProfile 
	{
		// For "running" a motion profile :
		double a1,a2,a3; // coef. from ^0 -> ^2 of first part
		double b1,b2,b3; // of 2nd part
		double c1,c2,c3; // of 3th part
		double duration;
		double t1,t2;

		// specification of the motion profile :
		double maxvel;
		double maxacc;
		double startpos;
		double endpos;
	public:
	
		VelocityProfile_Trap(double _maxvel,double _maxacc);
		// constructs motion profile class with <maxvel> and <maxacc> as parameters of the
		// trajectory.

		virtual void SetProfile(double pos1,double pos2);

		virtual void SetProfileDuration(
			double pos1,double pos2,double newduration
		);

        virtual void SetMax(double _maxvel,double _maxacc);
		virtual double Duration() const;
		virtual double Pos(double time) const;
		virtual double Vel(double time) const;
		virtual double Acc(double time) const;
#if HAVE_IOSTREAM
		virtual void Write(ostream& os) const;
#endif
		virtual VelocityProfile* Clone();
		// returns copy of current VelocityProfile object. (virtual constructor)
		virtual ~VelocityProfile_Trap();
	};


	



/* Niet OK
	class VelocityProfile_Trap : public VelocityProfile {
		double maxvel;
		double maxacc;
		double _t1,_t2,_T,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10;
		
		void PrepTraj(double p1,double v1,double p2,double v2,
		double acc,double vel,double t1,double t2,double T);
		// Internal method. Sets the parameters <_t1>,..<c10> with the given 
		// arguments. 
	public:
		VelocityProfile_Trap(double _maxvel,double _maxacc):
		  maxvel(_maxvel),maxacc(_maxacc) {}
		// constructs motion profile class with max velocity <maxvel>,
		// and max acceleration <maxacc> as parameter of the
		// trajectory.

		void SetProfile(double pos1,double pos2);
		virtual void SetProfileDuration(double pos1,double pos2,double duration);
		virtual double Duration() ;
		virtual double Pos(double time);
		virtual double Vel(double time);
	};
*/

#ifdef USE_NAMESPACE
}
#endif


#endif
