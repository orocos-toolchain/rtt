/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  rchainabstract.cxx 

                        rchainabstract.cxx -  description
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
