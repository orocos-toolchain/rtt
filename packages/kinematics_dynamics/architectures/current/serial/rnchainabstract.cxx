/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  rnchainabstract.cxx 

                        rnchainabstract.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
