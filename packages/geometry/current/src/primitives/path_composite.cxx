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
 *		$Id: path_composite.cpp,v 1.1.1.1.2.7 2003/07/24 13:49:16 rwaarsin Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/path_composite.h"
#include "geometry/error.h"
#include <memory>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

// s should be in allowable limits, this is not checked
// simple linear search : TODO : make it binary search
// uses cached_... variables
// returns the relative path length within the segment
// you propably want to use the cached_index variable
double Path_Composite::Lookup(double s) const
{	

	if ( (cached_starts <=s) && ( s <= cached_ends) ) {
		return s - cached_starts;
	}
	double previous_s=0;
	for (unsigned int i=0;i<dv.size();++i) {
		if ((s <= dv[i])||(i == (dv.size()-1) )) {
			cached_index = i;
			cached_starts = previous_s;
			cached_ends   = dv[i];
			return s - previous_s;
		}
		previous_s = dv[i];
	}
	return 0; 
}

Path_Composite::Path_Composite() {
	pathlength    = 0;
	cached_starts = 0;
	cached_ends   = 0;
	cached_index  = 0;
}

void Path_Composite::Add(Path* geom, bool aggregate ) {
	pathlength += geom->PathLength();
	dv.insert(dv.end(),pathlength);
	gv.insert( gv.end(),std::make_pair(geom,aggregate) );	
}

double Path_Composite::LengthToS(double length) {
#ifdef HAVE_EXCEPTIONS
	throw Error_MotionPlanning_Not_Applicable();
#endif
	return 0;
}

double Path_Composite::PathLength() {
	return pathlength;
}

Frame Path_Composite::Pos(double s) const {
	s = Lookup(s);
	return gv[cached_index].first->Pos(s);
}

Twist Path_Composite::Vel(double s,double sd) const {
	s = Lookup(s);
	return gv[cached_index].first->Vel(s,sd);
}

Twist Path_Composite::Acc(double s,double sd,double sdd) const {
	s = Lookup(s);
	return gv[cached_index].first->Acc(s,sd,sdd);
}

Path* Path_Composite::Clone()  {
	std::auto_ptr<Path_Composite> comp( new Path_Composite() );
	for (unsigned int i = 0; i < dv.size(); ++i) {
		comp->Add(gv[i].first->Clone(), gv[i].second);
	}
	return comp.release();
}

#if OROINT_OS_STDIOSTREAM
void Path_Composite::Write(ostream& os)  {
	os << "COMPOSITE[ " << endl;
	os << "   " << dv.size() << endl;
	for (unsigned int i=0;i<dv.size();i++) {
		gv[i].first->Write(os);
	}
	os << "]" << endl;
}
#endif
		
Path_Composite::~Path_Composite() {
	PathVector::iterator it;
	for (it=gv.begin();it!=gv.end();++it) {
		if (it->second)
            delete it->first;
	}
}

#ifdef USE_NAMESPACE
}
#endif
