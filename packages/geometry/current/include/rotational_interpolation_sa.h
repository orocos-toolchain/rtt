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
 *		$Id: rotational_interpolation_singleaxis.h,v 1.1.1.1.2.3 2003/07/24 13:26:15 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef ORIENTATION_SINGLEAXIS_H
#define ORIENTATION_SINGLEAXIS_H

#include "frames.h"
#include "frames_io.h"
#include "rotational_interpolation.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


class RotationalInterpolation_SingleAxis: public RotationalInterpolation
	{
		Rotation R_base_start;
		Rotation R_base_end;
		Vector rot_start_end; 
		double angle;
	public:
		RotationalInterpolation_SingleAxis();
		virtual void SetStartEnd(Rotation start,Rotation end);
		virtual double Angle();
		virtual Rotation Pos(double th) const;
		virtual Vector Vel(double th,double thd) const;
		virtual Vector Acc(double th,double thd,double thdd)   const;
#if OROINT_OS_STDIOSTREAM
		virtual void Write(ostream& os) const;
#endif
		virtual RotationalInterpolation* Clone() const;
		virtual ~RotationalInterpolation_SingleAxis();
	};



#ifdef USE_NAMESPACE
}
#endif


#endif
