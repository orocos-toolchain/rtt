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
 *		$Id: geometry_cyclic_closed.h,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef GEOMETRY_CYCLIC_CLOSED_H
#define GEOMETRY_CYCLIC_CLOSED_H

#include "frames.h"
#include "frames_io.h"
#include "geometry.h"
#include <vector>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * The specification of the geometry of a trajectory.
 */
	class Geometry_Cyclic_Closed : public Geometry
	{
		int times;
		Geometry* geom;
		bool aggregate;
	public:
		Geometry_Cyclic_Closed(Geometry* _geom,int _times, bool _aggregate=true);
		virtual double LengthToS(double length);		
		virtual double PathLength();
		virtual Frame Pos(double s) const;
		virtual Twist Vel(double s,double sd) const;
		virtual Twist Acc(double s,double sd,double sdd) const;

#if HAVE_IOSTREAM
		virtual void Write(ostream& os);
		static Geometry* Read(istream& is);
#endif
		virtual Geometry* Clone();
		virtual ~Geometry_Cyclic_Closed();
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
