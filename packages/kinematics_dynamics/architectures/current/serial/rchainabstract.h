/*****************************************************************************
 * \file  
 *    Routines for the manipulation of a serial kinematic chain.
 *    Forward position and velocity kinematic routines with RFrame-objects
 *    i.e. acceleration is calculated ( with the Jacobian routine that also
 *    returns the derivative).
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
 *		$Id: rchainabstract.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef RCHAINABSTRACT_H
#define RCHAINABSTRACT_H


#include <rframes/rframes.h>
#include <chain/chainabstract.h>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * Additional operator on ChainElement to calculate the transformation
 * that elem represents.
 * @param elem indicates a part of a kinematic chain
 * @param jval indicates the joint value and its derivative
 * @return a RFrame 
 */
RFrame RTf(const ChainElement& elem,const RDouble& jval);

/**
 * Forward position and velocity kinematics with RFrame-like objects
 * \par Type 
 * Abstract Interface
 */
class RChainAbstract:public ChainAbstract {
public:
	//! Forward position kinematics with RFrame objects
	virtual RFrame RFwdKin(
		const NEWMAT::ColumnVector& jval,
		const NEWMAT::ColumnVector& vel
	)=0;
	
	/** RFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_mp returns the velocity of the point
	 * fixed to the mounting plate and that coincides with the 
	 * mounting plate, w.r.t. the base.
	 * (most common interpretation used)
	 */
	virtual RTwist RTwist_base_mp(int joint) const=0;

	/** RFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_base returns the velocity of the point
	 * that coincides with the base, put is fixed to the mounting plate,
	 * w.r.t. the base.
	 */
	virtual RTwist RTwist_base_base(int joint) const=0;

	/** RFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_mp_mp returns the velocity of the point
	 * fixed to the mounting plate and that coincides with the 
	 * mounting plate, w.r.t. mounting plate.
	 */
	virtual RTwist RTwist_mp_mp(int joint) const=0;

	/** RFwdKin should be called.
	 * if [joint] moves with a unit velocity
	 * Twist_base_mp returns the velocity of the point
	 * fixed to mounting plate and that coincides with the 
	 * base, w.r.t. mounting plate.
	 * normally not often used, but usefull when 
	 * "inverting" a chain.
	 */
	virtual RTwist RTwist_mp_base(int joint) const=0;

	//! returns the actual mounting plate wrt base frame.
	virtual RFrame RMountingPlate() const=0;
	
	//! returns the number of joints of the chain.
	virtual int NrOfJoints() const=0;


	//! returns the Jacobian with vel. ref point mp wrt base and 1st derivative of the Jacobian
	virtual void RGeometric_Jacobian_base_mp(NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot);
	//! returns the Jacobian with vel. ref point base wrt base and 1st derivative of the Jacobian
	virtual void RGeometric_Jacobian_base_base(NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot);
	//! returns the Jacobian with vel. ref point mp wrt mp and 1st derivative of the Jacobian
	virtual void RGeometric_Jacobian_mp_mp(NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot);
	//! returns the Jacobian with vel. ref point base wrt mp and 1st derivative of the Jacobian
	virtual void RGeometric_Jacobian_mp_base(NEWMAT::Matrix& J,NEWMAT::Matrix& Jdot);

	virtual ~RChainAbstract() {};
};



#ifdef USE_NAMESPACE
}
#endif


#endif
