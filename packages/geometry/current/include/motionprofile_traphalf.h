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
 *		$Id: motionprofile_traphalf.h,v 1.1.1.1.2.4 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 *  \par Status
 *      Experimental
 ****************************************************************************/

#ifndef MOTIONPROFILE_TRAPHALF_H
#define MOTIONPROFILE_TRAPHALF_H

#include "motionprofile.h"




#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif



class MotionProfile_TrapHalf : public MotionProfile 
	{
		// For "running" a motion profile :
		double a1,a2,a3; // coef. from ^0 -> ^2 of first part
		double b1,b2,b3; // of 2nd part
		double c1,c2,c3; // of 3th part
		double duration;
		double t1,t2;

		double startpos;
		double endpos;
		
		// Persistent state :
		double maxvel;
		double maxacc;
		bool   starting;

		void PlanProfile1(double v,double a);
		void PlanProfile2(double v,double a);
	public:

		/**
		 * \param maxvel maximal velocity of the motion profile (positive)
		 * \param maxacc maximal acceleration of the motion profile (positive)
		 * \param starting this value is true when initial velocity is zero
		 *        and ending velocity is maxvel, is false for the reverse
		 */
		MotionProfile_TrapHalf(double _maxvel,double _maxacc,bool _starting);

		/**
		 * Can throw a Error_MotionPlanning_Not_Feasible
		 */
		virtual void SetProfile(double pos1,double pos2);

		virtual void SetProfileDuration(
			double pos1,double pos2,double newduration
		);

		virtual double Duration() const;
		virtual double Pos(double time) const;
		virtual double Vel(double time) const;
		virtual double Acc(double time) const;
#if HAVE_IOSTREAM
		virtual void Write(ostream& os) const;
#endif
		virtual MotionProfile* Clone();		

		virtual ~MotionProfile_TrapHalf();
	};


	


#ifdef USE_NAMESPACE
}
#endif


#endif
