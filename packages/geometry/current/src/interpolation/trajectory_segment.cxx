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
 *		$Id: trajectory_segment.cpp,v 1.1.1.1.2.7 2003/07/23 16:44:26 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/trajectory_segment.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


Trajectory_Segment::Trajectory_Segment(Geometry* _geom, MotionProfile* _motprof, bool _aggregate):
	motprof(_motprof),geom(_geom), aggregate(_aggregate)
{
    // assume everything is set or at least check if Duration() != 0
}

Trajectory_Segment::Trajectory_Segment(Geometry* _geom, MotionProfile* _motprof, double _duration, bool _aggregate):
	motprof(_motprof),geom(_geom), aggregate(_aggregate)
{
    // the duration was specified so assume motprof not yet set.
    motprof->SetProfileDuration(0, geom->PathLength(), _duration);
}

Geometry* Trajectory_Segment::GetGeometry()
{
    return geom;
}

MotionProfile* Trajectory_Segment::GetProfile()
{
    return motprof;
}

double Trajectory_Segment::Duration() const
{
	return motprof->Duration();
}

Frame Trajectory_Segment::Pos(double time) const
{
	return geom->Pos(motprof->Pos(time));
}

Twist Trajectory_Segment::Vel(double time) const
{
	return geom->Vel(motprof->Pos(time),motprof->Vel(time));
}

Twist Trajectory_Segment::Acc(double time) const
{
	return geom->Acc(motprof->Pos(time),motprof->Vel(time),motprof->Acc(time));
}


#ifdef OROINT_OS_STDIOSTREAM
void Trajectory_Segment::Write(ostream& os) const
{
	os << "SEGMENT[ " << endl;
	os << "  ";geom->Write(os);os << endl;
	os << "  ";motprof->Write(os);os << endl;
	os << "]";
}
#endif

Trajectory_Segment::~Trajectory_Segment()
{
    if (aggregate)
        {
            delete motprof;
            delete geom;
        }
}

#ifdef USE_NAMESPACE
}
#endif
