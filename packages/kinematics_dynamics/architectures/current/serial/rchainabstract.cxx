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
 *		$Id: rchainabstract.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <chain/rchainabstract.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif

RFrame RTf(const ChainElement& elem,const RDouble& jval) {
	switch(elem.Type()) {
	case ChainElement::TransX:
		return RFrame(Frame( elem.tf.M,Vector( elem.tf.p(0)+jval.t*elem.scale, elem.tf.p(1), elem.tf.p(2))),
		          Twist(Vector(jval.grad*elem.scale,0,0),Vector(0,0,0))
				  );
	case ChainElement::TransY:
		return RFrame(Frame( elem.tf.M,Vector( elem.tf.p(0), elem.tf.p(1)+jval.t*elem.scale, elem.tf.p(2))),
		          Twist(Vector(0,jval.grad*elem.scale,0),Vector(0,0,0))
				  );
	case ChainElement::TransZ:
		return RFrame(Frame( elem.tf.M,Vector( elem.tf.p(0), elem.tf.p(1), elem.tf.p(2)+jval.t*elem.scale)),
		          Twist(Vector(0,0,jval.grad*elem.scale),Vector(0,0,0))
				  );		
	case ChainElement::RotX: {
		Frame tmp(elem.tf);
		tmp.M.DoRotX(jval.t);
		return RFrame(tmp,Twist(Vector(0,0,0),Vector(jval.grad*elem.scale,0,0)));
		}
	case ChainElement::RotY: {
		Frame tmp(elem.tf);
		tmp.M.DoRotY(jval.t*elem.scale);
		return RFrame(tmp,Twist(Vector(0,0,0),Vector(0,jval.grad*elem.scale,0)));
		}
	case ChainElement::RotZ: {
		Frame tmp(elem.tf);
		tmp.M.DoRotZ(jval.t*elem.scale);
		return RFrame(tmp,Twist(Vector(0,0,0),Vector(0,0,jval.grad*elem.scale)));
		}
	}
	assert(false); // error in program logic.
	return RFrame();
}


//---------------------------------------------------------------------
//   IMPLEMENTATION OF RCHAINABSTRACT
//---------------------------------------------------------------------
void RChainAbstract::RGeometric_Jacobian_base_mp(
	NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot
) {
   int s=NrOfJoints();
   int i;
   for (i=0;i<s;i++) {
		RTwist t( RTwist_base_mp(i) );
		J(1,i+1)=t.vel.p(0);
		J(2,i+1)=t.vel.p(1);
		J(3,i+1)=t.vel.p(2);
		J(4,i+1)=t.rot.p(0);
		J(5,i+1)=t.rot.p(1);
		J(6,i+1)=t.rot.p(2);
		Jdot(1,i+1)=t.vel.v(0);
		Jdot(2,i+1)=t.vel.v(1);
		Jdot(3,i+1)=t.vel.v(2);
		Jdot(4,i+1)=t.rot.v(0);
		Jdot(5,i+1)=t.rot.v(1);
		Jdot(6,i+1)=t.rot.v(2);
   }
}

void RChainAbstract::RGeometric_Jacobian_base_base(
	NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot
) {
   int s=NrOfJoints();
   int i;
   for (i=0;i<s;i++) {
		RTwist t( RTwist_base_base(i) );
		J(1,i+1)=t.vel.p(0);
		J(2,i+1)=t.vel.p(1);
		J(3,i+1)=t.vel.p(2);
		J(4,i+1)=t.rot.p(0);
		J(5,i+1)=t.rot.p(1);
		J(6,i+1)=t.rot.p(2);
		Jdot(1,i+1)=t.vel.v(0);
		Jdot(2,i+1)=t.vel.v(1);
		Jdot(3,i+1)=t.vel.v(2);
		Jdot(4,i+1)=t.rot.v(0);
		Jdot(5,i+1)=t.rot.v(1);
		Jdot(6,i+1)=t.rot.v(2);
   }
}

void RChainAbstract::RGeometric_Jacobian_mp_mp(
	NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot
) {
   int s=NrOfJoints();
   int i;
   for (i=0;i<s;i++) {
		RTwist t( RTwist_mp_mp(i) );
		J(1,i+1)=t.vel.p(0);
		J(2,i+1)=t.vel.p(1);
		J(3,i+1)=t.vel.p(2);
		J(4,i+1)=t.rot.p(0);
		J(5,i+1)=t.rot.p(1);
		J(6,i+1)=t.rot.p(2);
		Jdot(1,i+1)=t.vel.v(0);
		Jdot(2,i+1)=t.vel.v(1);
		Jdot(3,i+1)=t.vel.v(2);
		Jdot(4,i+1)=t.rot.v(0);
		Jdot(5,i+1)=t.rot.v(1);
		Jdot(6,i+1)=t.rot.v(2);
   }
}

void RChainAbstract::RGeometric_Jacobian_mp_base(
	NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot
) {
   int s=NrOfJoints();
   int i;
   for (i=0;i<s;i++) {
		RTwist t( RTwist_mp_base(i) );
		J(1,i+1)=t.vel.p(0);
		J(2,i+1)=t.vel.p(1);
		J(3,i+1)=t.vel.p(2);
		J(4,i+1)=t.rot.p(0);
		J(5,i+1)=t.rot.p(1);
		J(6,i+1)=t.rot.p(2);
		Jdot(1,i+1)=t.vel.v(0);
		Jdot(2,i+1)=t.vel.v(1);
		Jdot(3,i+1)=t.vel.v(2);
		Jdot(4,i+1)=t.rot.v(0);
		Jdot(5,i+1)=t.rot.v(1);
		Jdot(6,i+1)=t.rot.v(2);
   }
}


#ifdef USE_NAMESPACE
}
#endif
