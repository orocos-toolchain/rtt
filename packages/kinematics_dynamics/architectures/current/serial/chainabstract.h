/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 15:59:17 CET 2005  chainabstract.h 

                        chainabstract.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Erwin Aertbelien
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
 * \file  
 *   Routines for the manipulation of a serial kinematic chain.
 *   Forward position and velocity kinematic routines with Frame-objects
 *		 
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
 *		$Id: chainabstract.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef __CHAINABSTRACT_H
#define __CHAINABSTRACT_H

#include <chain/chain_newmat.h>
#include <chain/chainelement.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/*
 * defined here for consistence with rchainabstract etc. 
 * So ChainElement does not have to now about RFrames etc.
 */
/**
 * Additional operator on ChainElement to calculate the transformation
 * that elem represents.
 * @param elem indicates a part of a kinematic chain
 * @param jval indicates the joint value
 * @return a frame 
 */
Frame Tf(const ChainElement& elem,double jval);

/**
 * Forward position and velocity kinematics with Frame-like objects
 * @par Type 
 * Abstract Interface
 */
class ChainAbstract {
public:
	/** execute forward position kinematics and return frame of mounting plate
	 *  wrt base.
	 */
	virtual Frame FwdKin(const NEWMAT::ColumnVector& v)=0;


	/** if [joint] moves with a unit velocity
	 * Twist_base_base returns the velocity of the point
	 * that coincides with the base, but is fixed to the mounting plate,
	 * This velocity is expressed w.r.t. the base.
	 */
	virtual Twist Twist_base_base(int joint) const=0;

	
	/** if [joint] moves with a unit velocity
	 * Twist_base_mp returns the velocity of the point
	 * fixed to the mounting plate and that coincides with the 
	 * mounting plate, w.r.t. the base.
	 * (most common interpretation used)
	 */
	virtual Twist Twist_base_mp(int joint) const=0;

	
	/** if [joint] moves with a unit velocity
	 * Twist_mp_mp returns the velocity of the point
	 * fixed to the mounting plate and that coincides with the 
	 * mounting plate, w.r.t. mounting plate.
	 */
	virtual Twist Twist_mp_mp(int joint) const=0;


	/** 
	 * if [joint] moves with a unit velocity
	 * Twist_base_mp returns the velocity of the point
	 * fixed to mounting plate and that coincides with the 
	 * base, w.r.t. mounting plate.
	 * usefull for "inverting" a chain.
	 * if a Chain I is identical to Chain C, except that
	 * mounting plate is fixed to ground, and base is moving,
	 * then : -C.Twist_mp_base(j) = I.Twist_base_mp(j)
	 */
	virtual Twist Twist_mp_base(int joint) const=0;

	//! returns the number of joints of the chain.
	virtual int NrOfJoints() const=0;
	
	//! returns the actual mounting plate wrt base frame.
	virtual Frame MountingPlate() const=0;
	
	
	virtual ~ChainAbstract() {};

	//! returns the Jacobian with vel. ref point mp wrt base
	virtual void Geometric_Jacobian_base_mp(NEWMAT::Matrix& J);

	//! returns the Jacobian with vel. ref point base wrt base
	virtual void Geometric_Jacobian_base_base(NEWMAT::Matrix& J);

	//! returns the Jacobian with vel. ref point mp wrt mp
	virtual void Geometric_Jacobian_mp_mp(NEWMAT::Matrix& J);

	//! returns the Jacobian with vel. ref point base wrt mp
	virtual void Geometric_Jacobian_mp_base(NEWMAT::Matrix& J);
};


#ifdef USE_NAMESPACE
}
#endif

#endif
