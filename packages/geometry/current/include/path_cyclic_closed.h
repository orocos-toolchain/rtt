 /*****************************************************************************
 *  \author 
 *  	Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *		ORO_Geometry V2
 *
 *	\par History
 *		- $log$
 *
 *	\par Release
 *		$Id: path_cyclic_closed.h,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_CYCLIC_CLOSED_H
#define GEOMETRY_CYCLIC_CLOSED_H

#include "frames.h"
#include "frames_io.h"
#include "path.h"
#include <vector>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * The specification of the path of a trajectory.
 */
	class Path_Cyclic_Closed : public Path
	{
		int times;
		Path* geom;
		bool aggregate;
	public:
		Path_Cyclic_Closed(Path* _geom,int _times, bool _aggregate=true);
		virtual double LengthToS(double length);		
		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const;
		virtual Twist Acc(double s,double sd,double sdd) const;

#if OROINT_OS_STDIOSTREAM
		virtual void Write(ostream& os);
		static Path* Read(istream& is);
#endif
		virtual Path* Clone();
		virtual ~Path_Cyclic_Closed();
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
