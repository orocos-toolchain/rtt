/***************************************************************************
  tag: Erwin Aertbelien  Mon May 10 19:10:35 CEST 2004  chainabstract.cxx 

                        chainabstract.cxx -  description
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
 *		$Id: chainabstract.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <newmat/newmat.h>
#include <newmat/newmatap.h>
#include <chain/chainabstract.h>
#include <utilities/error_stack.h>
#include <utilities/utility_newmat.h>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif


Frame Tf(const ChainElement& elem,double jval) {
	switch(elem.Type()) {
	case ChainElement::TransX:
		return Frame( elem.tf.M,
			Vector( elem.tf.p(0)+jval*elem.scale, elem.tf.p(1), elem.tf.p(2))
		);
	case ChainElement::TransY:
		return Frame( elem.tf.M,
			Vector( elem.tf.p(0), elem.tf.p(1)+jval*elem.scale, elem.tf.p(2))
		);
	case ChainElement::TransZ:
		return Frame( elem.tf.M,
			Vector( elem.tf.p(0), elem.tf.p(1), elem.tf.p(2)+jval*elem.scale)
		);
	case ChainElement::RotX: {
		Frame tmp(elem.tf);
		tmp.M.DoRotX(jval*elem.scale);
		return tmp;
		}
	case ChainElement::RotY: {
		Frame tmp(elem.tf);
		tmp.M.DoRotY(jval*elem.scale);
		return tmp;
		}
	case ChainElement::RotZ: {
		Frame tmp(elem.tf);
		tmp.M.DoRotZ(jval*elem.scale);
		return tmp;
		}
	}
	assert(false); // error in program logic.
	return Frame();
}








//---------------------------------------------------------------------
//   IMPLEMENTATION OF CHAINABSTRACT
//---------------------------------------------------------------------

// Geometric_Jacobian_base_mp returns the Jacobian of the point
// fixed to the mounting plate and that coincides with the 
// mounting plate, w.r.t. the base.
// (most common interpretation used)
void ChainAbstract::Geometric_Jacobian_base_mp(NEWMAT::Matrix& J)
{
   int s=NrOfJoints();
   for (int i=0;i<s;i++) {
		Twist t( Twist_base_mp(i) );
		J(1,i+1) = t.vel(0);
		J(2,i+1) = t.vel(1);
		J(3,i+1) = t.vel(2);
		J(4,i+1) = t.rot(0);
		J(5,i+1) = t.rot(1);
		J(6,i+1) = t.rot(2);
   }
}

// Geometric_Jacobian_base_base returns the Jacobian of the point
// that coincides with the base, but is fixed to the mounting plate,
// w.r.t. the base.
void ChainAbstract::Geometric_Jacobian_base_base(NEWMAT::Matrix& J)
{
   int s=NrOfJoints();
   for (int i=0;i<s;i++) {
		Twist t (Twist_base_base(i));
		J(1,i+1) = t.vel(0);
		J(2,i+1) = t.vel(1);
		J(3,i+1) = t.vel(2);
		J(4,i+1) = t.rot(0);
		J(5,i+1) = t.rot(1);
		J(6,i+1) = t.rot(2);
   }
}


// Geometric_Jacobian_mp_mp returns the Jacobian of the point
// fixed to the mounting plate and that coincides with the 
// mounting plate, w.r.t. mounting plate.
void ChainAbstract::Geometric_Jacobian_mp_mp(NEWMAT::Matrix& J)
{
   int s=NrOfJoints();
   for (int i=0;i<s;i++) {
		Twist t(Twist_mp_mp(i) );
		J(1,i+1) = t.vel(0);
		J(2,i+1) = t.vel(1);
		J(3,i+1) = t.vel(2);
		J(4,i+1) = t.rot(0);
		J(5,i+1) = t.rot(1);
		J(6,i+1) = t.rot(2);
   }
}


// Geometric_Jacobian_base_mp returns the Jacobian of the point
// fixed to mounting plate and that coincides with the 
// base, w.r.t. mounting plate.
// usefull for "inverting" a chain.
// if a Chain I is identical to Chain C, except that
// mounting plate is fixed to ground, and base is moving,
// then : -C.Twist_mp_base(j) = I.Twist_base_mp(j)
void ChainAbstract::Geometric_Jacobian_mp_base(NEWMAT::Matrix& J)
{
   int s=NrOfJoints();
   for (int i=0;i<s;i++) {
		Twist t (Twist_mp_base(i) );
		J(1,i+1) = t.vel(0);
		J(2,i+1) = t.vel(1);
		J(3,i+1) = t.vel(2);
		J(4,i+1) = t.rot(0);
		J(5,i+1) = t.rot(1);
		J(6,i+1) = t.rot(2);
   }
}

#ifdef USE_NAMESPACE
}
#endif
