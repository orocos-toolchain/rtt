/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 15:59:17 CET 2005  rnchainabstract.h 

                        rnchainabstract.h -  description
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
 *		$Id: rnchainabstract.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef RNCHAINABSTRACT_H
#define RNCHAINABSTRACT_H


#include <rframes/rnframes.h>
#include <rframes/rnframes_io.h>
#include <chain/chainabstract.h>
#include <utilities/utility_newmat.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * Additional operator on ChainElement to calculate the transformation
 * that elem represents.
 * @param elem indicates a part of a kinematic chain
 * @param jval indicates the joint value and its derivatives towards VSIZE variables
 * @return a RNFrame 
 */
RNFrame RNTf(const ChainElement& elem,const RNDouble& jval);



/**
 * Forward position and velocity kinematics with RNFrame-like objects
 * \par Type 
 * Abstract Interface
 */
class RNChainAbstract: public ChainAbstract {
public:
	/** 
	 * @param jval joint values of each joint
	 * @param vel contains the derivatives of jval
	 *        the columns of vel contain the derivatives towards 1 variable
	 *        the rows belong to a certain joint.
	 * @return RNFrame of the end effector position
	 */
	virtual RNFrame RNFwdKin(const NEWMAT::ColumnVector& jval,const NEWMAT::Matrix& vel)=0;

	//! The following is identical to RNFwdKin(const NEWMAT::ColumnVector& jval,const NEWMAT::Matrix& vel), 
	//! with vel is a unity matrix.
	virtual RNFrame RNFwdKin(const NEWMAT::ColumnVector& jval)=0;
	
	/** RNFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_mp returns the velocity of the point
	 * fixed to the mounting plate and that coincides with the 
	 * mounting plate, w.r.t. the base.
	 * (most common interpretation used)
	 */
	virtual RNTwist RNTwist_base_mp(int joint) const=0;

	/** RNFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_base returns the velocity of the point
	 * that coincides with the base, put is fixed to the mounting plate,
	 * w.r.t. the base.
	 */
	virtual RNTwist RNTwist_base_base(int joint) const=0;
	
	/** RNFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_mp_mp returns the velocity of the point
	 * fixed to the mounting plate and that coincides with the 
	 * mounting plate, w.r.t. mounting plate.
	 */
	virtual RNTwist RNTwist_mp_mp(int joint) const=0;

	/** RNFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_mp returns the velocity of the point
	 * fixed to mounting plate and that coincides with the 
	 * base, w.r.t. mounting plate.
	 * normally not often used, but usefull when 
	 * "inverting" a chain.
	 */
	virtual RNTwist RNTwist_mp_base(int joint) const=0;
	
	//! returns the number of joints of the chain.
	virtual int NrOfJoints() const=0;

	//! returns the actual mounting plate wrt base frame.
	virtual RNFrame RNMountingPlate() const=0;

	//! returns the Jacobian with vel. ref point mp wrt base
	virtual void RNGeometric_Jacobian_base_mp(NEWMAT::Matrix& J,MatrixVector& Jdot);
	//! returns the Jacobian with vel. ref point base wrt base	
	virtual void RNGeometric_Jacobian_base_base(NEWMAT::Matrix& J,MatrixVector& Jdot);
	//! returns the Jacobian with vel. ref point mp wrt mp	
	virtual void RNGeometric_Jacobian_mp_mp(NEWMAT::Matrix& J,MatrixVector& Jdot);
	//! returns the Jacobian with vel. ref point base wrt mp	
	virtual void RNGeometric_Jacobian_mp_base(NEWMAT::Matrix& J,MatrixVector& Jdot);

	virtual ~RNChainAbstract() {};
};



#ifdef USE_NAMESPACE
}
#endif



#endif
