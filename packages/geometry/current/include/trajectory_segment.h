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
 *		$Id: trajectory_segment.h,v 1.1.1.1.2.5 2003/07/23 16:44:26 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef TRAJECTORY_SEGMENT_H
#define TRAJECTORY_SEGMENT_H

#include "frames.h"
#include "frames_io.h"
#include "trajectory.h"
#include "path.h"
#include "velocityprofile.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


/**
 * Trajectory_Segment combines a VelocityProfile and a Path to a 
 * trajectory
 */

	class Trajectory_Segment :  public Trajectory 
	{
		VelocityProfile* motprof;
		Path*      geom;
		bool aggregate;
	public:
		/**
		 * This constructor assumes that <geom> and <_motprof> are initialised correctly.
		 */
		Trajectory_Segment(Path* geom, VelocityProfile* _motprof, bool _aggregate=true);
		/**
		 * This constructor assumes that <geom> is initialised and <_motprof> needs to be
		 * set according to <duration>.
		 */
		Trajectory_Segment(Path* geom, VelocityProfile* _motprof, double duration, bool _aggregate=true);

		virtual Path* GetPath();
		virtual VelocityProfile* GetProfile();
		virtual double Duration() const;
		// The duration of the trajectory

		virtual Frame Pos(double time) const;
		// Position of the trajectory at <time>.

		virtual Twist Vel(double time) const;
		// The velocity of the trajectory at <time>.
		virtual Twist Acc(double time) const;
		// The acceleration of the trajectory at <time>.

#if OROINT_OS_STDIOSTREAM
		virtual void Write(ostream& os) const;
#endif

		virtual ~Trajectory_Segment();
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
