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
 *		$Id: trajectory_composite.cpp,v 1.1.1.1.2.3 2003/04/04 11:56:57 jrutgeer Exp $
 *		$Name:  $ 
 ****************************************************************************/

#include "geometry/trajectory_composite.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


Trajectory_Composite::Trajectory_Composite():duration(0.0) {
}

double Trajectory_Composite::Duration() const {
	return duration;
}

Frame Trajectory_Composite::Pos(double time) const {
	// not optimal, could be done in log(#elem)
	// or one could buffer the last segment and start looking from there.
	unsigned int i;
	double previoustime;
	Trajectory* traj;
	if (time < 0) {
		return vt[0]->Pos(0);
	}
	previoustime = 0;
	for (i=0;i<vt.size();i++) {
		if (time < vd[i]) {
			return vt[i]->Pos(time-previoustime);
		}
		previoustime = vd[i];
	}
	traj = vt[vt.size()-1];
	return traj->Pos(traj->Duration());
}


Twist Trajectory_Composite::Vel(double time) const {
	// not optimal, could be done in log(#elem)
	unsigned int i;
	Trajectory* traj;
	double previoustime;
	if (time < 0) {
		return vt[0]->Vel(0);
	}
	previoustime = 0;
	for (i=0;i<vt.size();i++) {
		if (time < vd[i]) {
			return vt[i]->Vel(time-previoustime);
		}
		previoustime = vd[i];
	}
	traj = vt[vt.size()-1];
	return traj->Vel(traj->Duration());
}

Twist Trajectory_Composite::Acc(double time) const {
	// not optimal, could be done in log(#elem)
	unsigned int i;
	Trajectory* traj;
	double previoustime;
	if (time < 0) {
		return vt[0]->Acc(0);
	}
	previoustime = 0;
	for (i=0;i<vt.size();i++) {
		if (time < vd[i]) {
			return vt[i]->Acc(time-previoustime);
		}
		previoustime = vd[i];
	}
	traj = vt[vt.size()-1];
	return traj->Acc(traj->Duration());
}

void Trajectory_Composite::Add(Trajectory* elem) {
	vt.insert(vt.end(),elem);
	duration += elem->Duration();
	vd.insert(vd.end(),duration);
}

void Trajectory_Composite::Destroy() {
	VectorTraj::iterator it;
	for (it=vt.begin();it!=vt.end();it++) {
		delete *it;
	}
	vt.erase(vt.begin(),vt.end());
	vd.erase(vd.begin(),vd.end());
}

Trajectory_Composite::~Trajectory_Composite() {
	Destroy();
}


#ifdef OROINT_OS_STDIOSTREAM
void Trajectory_Composite::Write(ostream& os) const {
	os << "COMPOSITE[ " << vt.size() << endl;
	unsigned int i;
	for (i=0;i<vt.size();i++) {
		vt[i]->Write(os);
	}
	os << "]" << endl;
}
#endif



#ifdef USE_NAMESPACE
}
#endif

