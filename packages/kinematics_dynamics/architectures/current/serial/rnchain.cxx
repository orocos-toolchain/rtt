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
 *		$Id: rnchain.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <chain/rnchain.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif

RNFrame RNChain::RNFwdKin(const NEWMAT::ColumnVector& jval,const NEWMAT::Matrix& vel) {
	int i;
	int s=NrOfJoints();
	RNFrame tmp(RNFrame::Identity() );
	for (i=0;i<s;i++) {
		// tmp = Frame w.r.t base, from axis it
		RNDouble value;
		value.t = jval(i+1);
		for (int j=0;j<VSIZE;j++) {
			value.grad[j] = vel(i+1,j+1);
		}
		tmp = tmp * RNTf(*list[i],value);
		joints[i] = tmp;
	}
	mp_base = tmp* mp;
	return mp_base;
}

RNFrame RNChain::RNFwdKin(const NEWMAT::ColumnVector& jval) {
	int i;
	int s=NrOfJoints();
	RNFrame tmp(RNFrame::Identity() );
	for (i=0;i<s;i++) {
		// tmp = Frame w.r.t base, from axis it
		RNDouble value;
		value.t = jval(i+1);
		for (int j=0;j<VSIZE;j++) {
			if (i==j) {
				value.grad[j] = 1.0;
			} else {
				value.grad[j] = 0;
			}			
		}
		tmp = tmp * RNTf(*list[i],value);
		joints[i] = tmp;
	}
	mp_base = tmp* mp;
	return mp_base;
}


Frame RNChain::FwdKin(const NEWMAT::ColumnVector& v) {
	RNFrame tmp(RNFwdKin(v));
	return Frame(tmp.M.R,tmp.p.p);
}

RNFrame RNChain::RNMountingPlate() const
// returns the actual mounting plate wrt base frame.
{
	return mp_base;
}


RNTwist RNChain::RNTwist_base_mp(int joint)  const {
	return (joints[joint].M * 
		    list[joint]->UnitTwist()).RefPoint(mp_base.p-joints[joint].p);
}

RNTwist RNChain::RNTwist_base_base(int joint)  const {
	return joints[joint] * list[joint]->UnitTwist();
}


RNTwist RNChain::RNTwist_mp_mp(int joint)  const {
	return  mp_base.M.Inverse(
				(joints[joint].M *	list[joint]->UnitTwist()).
					RefPoint(mp_base.p-joints[joint].p)
			);
// you could also write :
//   return  mp_base.Inverse( (joints[joint]*list[joint]->UnitTwist()) );
// but it is not as efficient (position is transformed to base and to mp,
// this is avoided above.
}


RNTwist RNChain::RNTwist_mp_base(int joint)  const{
	return mp_base.M.Inverse(
		    joints[joint] * list[joint]->UnitTwist()
		);
}


int RNChain::NrOfJoints() const {
		return ChainList::NrOfJoints();
}

void RNChain::Add(ChainElement* el) {
		ChainList::Add(el);
		joints.insert(joints.end(),RNFrame());
}

RNChain::~RNChain() {
	joints.erase(joints.begin(),joints.end());
}

Twist RNChain::Twist_base_base(int joint)  const{
	return joints[joint].GetFrame() * list[joint]->UnitTwist();
}

Twist RNChain::Twist_base_mp(int joint)  const{
	return (joints[joint].M.R * 
		    list[joint]->UnitTwist()).RefPoint(mp_base.p.p-joints[joint].p.p);
}

Twist RNChain::Twist_mp_mp(int joint)  const{
	return  mp_base.M.R.Inverse(
				(joints[joint].M.R *	list[joint]->UnitTwist()).
					RefPoint(mp_base.p.p-joints[joint].p.p)
			);
}

Twist RNChain::Twist_mp_base(int joint)  const{
	return mp_base.M.R.Inverse(
		    joints[joint].GetFrame() * list[joint]->UnitTwist()
		);
}
	
Frame RNChain::MountingPlate() const{
	return mp_base.GetFrame();
}


#ifdef USE_NAMESPACE
}
#endif
