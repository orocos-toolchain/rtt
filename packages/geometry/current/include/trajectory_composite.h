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
 *		$Id: trajectory_composite.h,v 1.1.1.1.2.2 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#ifndef TRAJECTORY_COMPOSITE_H
#define TRAJECTORY_COMPOSITE_H

#include "trajectory.h"

#include <vector>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

	/*
	 * Trajectory_Composite implements a trajectory that is composed
	 * of underlying trajectoria.  Call Add to add a trajectory.
     *
     * @deprecated Use Path_Composite or Path_RoundedComposite with Trajectory_Segment.
	 */
	class Trajectory_Composite: public Trajectory 
    {
        typedef std::vector<Trajectory*> VectorTraj;
        typedef std::vector<double>      VectorDouble;
        VectorTraj vt;      // contains the element Trajectories
        VectorDouble  vd;      // contains end time for each Trajectory
        double duration;    // total duration of the composed Trajectory
    public:
        Trajectory_Composite();
        // Constructs an empty composite

        virtual double Duration() const;
        virtual Frame Pos(double time) const;
        virtual Twist Vel(double time) const;
        virtual Twist Acc(double time) const;

        virtual void Add(Trajectory* elem);
        // Adds trajectory <elem> to the end of the sequence.

        virtual void Destroy();

        virtual Trajectory* Clone() const
        {
            Trajectory_Composite* tmp =  new Trajectory_Composite();
            VectorTraj::const_iterator it;
            for (it=vt.begin();it!=vt.end();it++) {
                tmp->Add( (*it)->Clone() );
            }
            return tmp;
        }

#if OROINT_OS_STDIOSTREAM
        virtual void Write(ostream& os) const;
#endif
        virtual ~Trajectory_Composite();
    };


#ifdef USE_NAMESPACE
}
#endif


#endif
