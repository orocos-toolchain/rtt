/***************************************************************************
  tag: Erwin Aertbelien  Mon May 10 19:10:35 CEST 2004  rrchain.cxx 

                        rrchain.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Erwin Aertbelien
    email                : erwin.aertbelien@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
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
 *		$Id: rrchain.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <chain/rrchain.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif


// RRChainAbstract methods
RRFrame RRChain::RRMountingPlate() const
// returns the actual mounting plate wrt base frame.
{
	return mp_base;
}


RRTwist RRChain::RRTwist_base_mp(int joint)  const {
	return (joints[joint].M * 
		    list[joint]->UnitTwist()).RefPoint(mp_base.p-joints[joint].p);
}

RRTwist RRChain::RRTwist_base_base(int joint)  const {
	return joints[joint] * list[joint]->UnitTwist();
}


RRTwist RRChain::RRTwist_mp_mp(int joint)  const {
	return  mp_base.M.Inverse(
				(joints[joint].M *	list[joint]->UnitTwist()).
					RefPoint(mp_base.p-joints[joint].p)
			);
// you could also write :
//   return  mp_base.Inverse( (joints[joint]*list[joint]->UnitTwist()) );
// but it is not as efficient (position is transformed to base and to mp,
// this is avoided above.
}


RRTwist RRChain::RRTwist_mp_base(int joint)  const{
	return mp_base.M.Inverse(
		    joints[joint] * list[joint]->UnitTwist()
		);
}


int RRChain::NrOfJoints() const {
		return ChainList::NrOfJoints();
}

void RRChain::Add(ChainElement* el) {
		ChainList::Add(el);
		joints.insert(joints.end(),RRFrame());
}


RRChain::~RRChain() {
	joints.erase(joints.begin(),joints.end());
}



// RChainAbstract methods
RFrame RRChain::RFwdKin(
		const NEWMAT::ColumnVector& jval,
		const NEWMAT::ColumnVector& vel
) {
	NEWMAT::ColumnVector zero(jval.Nrows());
	for (int i=1;i<=jval.Nrows();i++) zero(i)=0.0;
	RRFrame result = RRFwdKin(jval,vel,zero);
	return RFrame(result.GetFrame(),result.GetTwist());
}

RFrame RRChain::RMountingPlate() const
// returns the actual mounting plate wrt base frame.
{
	//return RFrame(mp_base.GetFrame(),mp_base.GetTwist());
	return RFrame(
		RRotation(mp_base.M.R,mp_base.M.w),
		RVector(mp_base.p.p,mp_base.p.v)
	);
}


RTwist RRChain::RTwist_base_mp(int joint)  const {
	RRTwist t( RRTwist_base_mp(joint) );
	return RTwist(RVector(t.vel.p,t.vel.v),RVector(t.rot.p,t.rot.v));
}

RTwist RRChain::RTwist_base_base(int joint)  const {
	RRTwist t( RRTwist_base_base(joint) );
	return RTwist(RVector(t.vel.p,t.vel.v),RVector(t.rot.p,t.rot.v));
}


RTwist RRChain::RTwist_mp_mp(int joint)  const {
	RRTwist t( RRTwist_mp_mp(joint) );
	return RTwist(RVector(t.vel.p,t.vel.v),RVector(t.rot.p,t.rot.v));
}


RTwist RRChain::RTwist_mp_base(int joint)  const{
	RRTwist t( RRTwist_mp_base(joint) );
	return RTwist(RVector(t.vel.p,t.vel.v),RVector(t.rot.p,t.rot.v));
}


// ChainAbstract methods	
Frame RRChain::FwdKin(const NEWMAT::ColumnVector& v) {
	NEWMAT::ColumnVector zero(v.Nrows());
	for (int i=1;i<=v.Nrows();i++) zero(i)=0.0;
	return RRFwdKin(v,zero,zero).GetFrame();
}

Twist RRChain::Twist_base_base(int joint)  const{
	return joints[joint].GetFrame() * list[joint]->UnitTwist();
}

Twist RRChain::Twist_base_mp(int joint)  const{
	return (joints[joint].M.R * 
		    list[joint]->UnitTwist()).RefPoint(mp_base.p.p-joints[joint].p.p);
}

Twist RRChain::Twist_mp_mp(int joint)  const{
	return  mp_base.M.R.Inverse(
				(joints[joint].M.R *	list[joint]->UnitTwist()).
					RefPoint(mp_base.p.p-joints[joint].p.p)
			);
}

Twist RRChain::Twist_mp_base(int joint)  const{
	return mp_base.M.R.Inverse(
		    joints[joint].GetFrame() * list[joint]->UnitTwist()
		);
}

	
Frame RRChain::MountingPlate() const{
	return mp_base.GetFrame();
}

RRFrame RRChain::RRFwdKin(
	const NEWMAT::ColumnVector& jval,
	const NEWMAT::ColumnVector& vel,
	const NEWMAT::ColumnVector& acc
) {
	int i;
	int s=NrOfJoints();
	RRFrame tmp(RRFrame::Identity() );
	for (i=0;i<s;i++) {
		// tmp = Frame w.r.t base, from axis it
		tmp = tmp * RRTf(*list[i],RRDouble(jval(i+1),vel(i+1),acc(i+1) )   );
		joints[i] = tmp;
	}
	mp_base = tmp* mp;
	return mp_base;
}



#ifdef USE_NAMESPACE
}
#endif

