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
 *		$Id: orientation_singleaxis.h,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef ORIENTATION_SINGLEAXIS_H
#define ORIENTATION_SINGLEAXIS_H

#include "frames.h"
#include "frames_io.h"
#include "orientation.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


class Orientation_SingleAxis: public Orientation
	{
		Rotation R_base_start;
		Rotation R_base_end;
		Vector rot_start_end; 
		double angle;
	public:
		Orientation_SingleAxis();
		virtual void SetStartEnd(Rotation start,Rotation end);
		virtual double Angle();
		virtual Rotation Pos(double th) const;
		virtual Vector Vel(double th,double thd) const;
		virtual Vector Acc(double th,double thd,double thdd)   const;
#if HAVE_IOSTREAM
		virtual void Write(ostream& os) const;
#endif
		virtual Orientation* Clone() const;
		virtual ~Orientation_SingleAxis();
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
