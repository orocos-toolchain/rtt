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
 *		$Id: rnchainabstract.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <chain/rnchainabstract.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif


RNFrame RNTf(const ChainElement& elem,const RNDouble& jval) {
	switch(elem.Type()) {
	case ChainElement::TransX: {
		RNVector p;
		p.p = Vector(elem.tf.p(0)+jval.t*elem.scale,elem.tf.p(1),elem.tf.p(2));
		for (int i=0;i<VSIZE;i++)
			p.v[i]=Vector(jval.grad[i]*elem.scale,0,0);
		return RNFrame(RNRotation(elem.tf.M),p);
	}
	case ChainElement::TransY: {
		RNVector p;
		p.p = Vector(elem.tf.p(0),elem.tf.p(1)+jval.t*elem.scale,elem.tf.p(2));
		for (int i=0;i<VSIZE;i++)
			p.v[i]=Vector(0,jval.grad[i]*elem.scale,0);
		return RNFrame(RNRotation(elem.tf.M),p);
	}
	case ChainElement::TransZ: {
		RNVector p;
		p.p = Vector(elem.tf.p(0),elem.tf.p(1),elem.tf.p(2)+jval.t*elem.scale);
		for (int i=0;i<VSIZE;i++)
			p.v[i]=Vector(0,0,jval.grad[i]*elem.scale);
		return RNFrame(RNRotation(elem.tf.M),p);
	}
	case ChainElement::RotX: {
		RNRotation tmp(elem.tf.M);
		tmp.DoRotX(jval*elem.scale);
		return RNFrame(tmp,RNVector(elem.tf.p));
	}
	case ChainElement::RotY: {
		RNRotation tmp(elem.tf.M);
		tmp.DoRotY(jval*elem.scale);
		return RNFrame(tmp,RNVector(elem.tf.p));
	}
	case ChainElement::RotZ: {
		RNRotation tmp(elem.tf.M);
		tmp.DoRotZ(jval*elem.scale);
		return RNFrame(tmp,RNVector(elem.tf.p));
		}
	}
	assert(false); // error in program logic.
	return RNFrame();
}



void RNChainAbstract::RNGeometric_Jacobian_base_mp(
	NEWMAT::Matrix& J,MatrixVector& Jdot
) {
   int s=NrOfJoints();
   int i;
   for (i=0;i<s;i++) {
		RNTwist t( RNTwist_base_mp(i) );
		J(1,i+1)=t.vel.p(0);
		J(2,i+1)=t.vel.p(1);
		J(3,i+1)=t.vel.p(2);
		J(4,i+1)=t.rot.p(0);
		J(5,i+1)=t.rot.p(1);
		J(6,i+1)=t.rot.p(2);
		for (int j=0;j<VSIZE;j++) {
			Jdot[j](1,i+1)=t.vel.v[j](0);
			Jdot[j](2,i+1)=t.vel.v[j](1);
			Jdot[j](3,i+1)=t.vel.v[j](2);
			Jdot[j](4,i+1)=t.rot.v[j](0);
			Jdot[j](5,i+1)=t.rot.v[j](1);
			Jdot[j](6,i+1)=t.rot.v[j](2);
		}
   }
}


void RNChainAbstract::RNGeometric_Jacobian_base_base(
	NEWMAT::Matrix& J,MatrixVector& Jdot
) {
   int s=NrOfJoints();
   int i;
   for (i=0;i<s;i++) {
		RNTwist t( RNTwist_base_base(i) );
		J(1,i+1)=t.vel.p(0);
		J(2,i+1)=t.vel.p(1);
		J(3,i+1)=t.vel.p(2);
		J(4,i+1)=t.rot.p(0);
		J(5,i+1)=t.rot.p(1);
		J(6,i+1)=t.rot.p(2);
		for (int j=0;j<VSIZE;j++) {
			Jdot[j](1,i+1)=t.vel.v[j](0);
			Jdot[j](2,i+1)=t.vel.v[j](1);
			Jdot[j](3,i+1)=t.vel.v[j](2);
			Jdot[j](4,i+1)=t.rot.v[j](0);
			Jdot[j](5,i+1)=t.rot.v[j](1);
			Jdot[j](6,i+1)=t.rot.v[j](2);
		}
   }
}

void RNChainAbstract::RNGeometric_Jacobian_mp_mp(
	NEWMAT::Matrix& J,MatrixVector& Jdot
) {
   int s=NrOfJoints();
   int i;
   for (i=0;i<s;i++) {
		RNTwist t( RNTwist_mp_mp(i) );
		J(1,i+1)=t.vel.p(0);
		J(2,i+1)=t.vel.p(1);
		J(3,i+1)=t.vel.p(2);
		J(4,i+1)=t.rot.p(0);
		J(5,i+1)=t.rot.p(1);
		J(6,i+1)=t.rot.p(2);
		for (int j=0;j<VSIZE;j++) {
			Jdot[j](1,i+1)=t.vel.v[j](0);
			Jdot[j](2,i+1)=t.vel.v[j](1);
			Jdot[j](3,i+1)=t.vel.v[j](2);
			Jdot[j](4,i+1)=t.rot.v[j](0);
			Jdot[j](5,i+1)=t.rot.v[j](1);
			Jdot[j](6,i+1)=t.rot.v[j](2);
		}
   }
}

void RNChainAbstract::RNGeometric_Jacobian_mp_base(
	NEWMAT::Matrix& J,MatrixVector& Jdot
) {
   int s=NrOfJoints();
   int i;
   for (i=0;i<s;i++) {
		RNTwist t( RNTwist_mp_base(i) );
		J(1,i+1)=t.vel.p(0);
		J(2,i+1)=t.vel.p(1);
		J(3,i+1)=t.vel.p(2);
		J(4,i+1)=t.rot.p(0);
		J(5,i+1)=t.rot.p(1);
		J(6,i+1)=t.rot.p(2);
		for (int j=0;j<VSIZE;j++) {
			Jdot[j](1,i+1)=t.vel.v[j](0);
			Jdot[j](2,i+1)=t.vel.v[j](1);
			Jdot[j](3,i+1)=t.vel.v[j](2);
			Jdot[j](4,i+1)=t.rot.v[j](0);
			Jdot[j](5,i+1)=t.rot.v[j](1);
			Jdot[j](6,i+1)=t.rot.v[j](2);
		}
   }
}



#ifdef USE_NAMESPACE
}
#endif
