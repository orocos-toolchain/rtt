/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 15:59:17 CET 2005  rrchainabstract.h 

                        rrchainabstract.h -  description
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
 *		$Id: rrchainabstract.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef RRCHAINABSTRACT_H
#define RRCHAINABSTRACT_H

#include <rframes/rrframes.h>
#include <rframes/rrframes_io.h>
#include <chain/rchainabstract.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * Additional operator on ChainElement to calculate the transformation
 * that elem represents.
 * @param elem indicates a part of a kinematic chain
 * @param jval indicates the joint value and its derivative and 2nd derivative
 * @return a RRFrame 
 */
RRFrame RRTf(const ChainElement& elem,const RRDouble& jval);

/**
 * Forward position and velocity kinematics with RRFrame-like objects
 * \par Type 
 * Abstract Interface
 */
class RRChainAbstract:public RChainAbstract {
public:
	//! Forward position kinematics with RR-frame like objects.
	virtual RRFrame RRFwdKin(const NEWMAT::ColumnVector& jval,
		                     const NEWMAT::ColumnVector& vel,
							 const NEWMAT::ColumnVector& acc
							 )=0;
	
	/** RRFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_mp returns the velocity of the point
	 * fixed to the mounting plate and that coincides with the 
	 * mounting plate, w.r.t. the base.
	 * (most common interpretation used)
	 */
	virtual RRTwist RRTwist_base_mp(int joint) const=0;

	/** RRFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_base returns the velocity of the point
	 * that coincides with the base, put is fixed to the mounting plate,
	 * w.r.t. the base.
	 */
	virtual RRTwist RRTwist_base_base(int joint) const=0;

	/** RRFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_mp_mp returns the velocity of the point
	 * fixed to the mounting plate and that coincides with the 
	 * mounting plate, w.r.t. mounting plate.
	 */
	virtual RRTwist RRTwist_mp_mp(int joint) const=0;

	/** RRFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_mp returns the velocity of the point
	 * fixed to mounting plate and that coincides with the 
	 * base, w.r.t. mounting plate.
	 * normally not often used, but usefull when 
	 * "inverting" a chain.
	 */
	virtual RRTwist RRTwist_mp_base(int joint) const=0;
	
	//! returns the number of joints of the chain.
	virtual int NrOfJoints() const=0;

	//! returns the actual mounting plate wrt base frame.
	virtual RRFrame RRMountingPlate() const=0;
	
	//! returns the Jacobian with vel. ref point mp wrt base and 1st and 2nd derivative of the Jacobian
	virtual void RRGeometric_Jacobian_base_mp(NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot,NEWMAT::Matrix& Jdotdot);
	//! returns the Jacobian with vel. ref point base wrt base and 1st and 2nd derivative of the Jacobian
	virtual void RRGeometric_Jacobian_base_base(NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot,NEWMAT::Matrix& Jdotdot);
	//! returns the Jacobian with vel. ref point mp wrt mp and 1st and 2nd derivative of the Jacobian
	virtual void RRGeometric_Jacobian_mp_mp(NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot,NEWMAT::Matrix& Jdotdot);
	//! returns the Jacobian with vel. ref point base wrt mp and 1st and 2nd derivative of the Jacobian
	virtual void RRGeometric_Jacobian_mp_base(NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot,NEWMAT::Matrix& Jdotdot);

	virtual ~RRChainAbstract() {};
};

#ifdef USE_NAMESPACE
}
#endif

#endif
