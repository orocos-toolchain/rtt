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
 *		$Id: trajectory.cpp,v 1.1.1.1.2.4 2003/07/18 14:58:36 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/error.h"
#include "geometry/error_stack.h"
#include "geometry/trajectory.h"
#include "geometry/geometry.h"
#include "geometry/trajectory_segment.h"

//#ifdef HAVE_RTSTL
//#include <rtstl/rtstl.hpp>
//#else
#include <memory>
//#endif


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

using namespace std;

#ifdef OROINT_OS_STDIOSTREAM
Trajectory* Trajectory::Read(istream& is) {
	// auto_ptr because exception can be thrown !
	IOTrace("Trajectory::Read");
	char storage[64];
	EatWord(is,"[",storage,sizeof(storage));
	Eat(is,'[');
	if (strcmp(storage,"SEGMENT")==0) {
		IOTrace("SEGMENT");
		auto_ptr<Geometry>      geom(    Geometry::Read(is)       );
		auto_ptr<MotionProfile> motprof( MotionProfile::Read(is)  );
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return new  Trajectory_Segment(geom.release(),motprof.release());
	} else {
#ifdef HAVE_EXCEPTIONS
		throw Error_MotionIO_Unexpected_Traj();
#endif
	}
	return NULL; // just to avoid the warning;
}
#endif



#ifdef USE_NAMESPACE
}
#endif

