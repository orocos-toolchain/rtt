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
 *		$Id: rchain.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef RCHAIN_H
#define RCHAIN_H

#include <chain/chainlist.h>
#include <chain/rchainabstract.h>
#include <rframes/rframes.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif



//! RChain implements the RChainAbstract and the ChainList interface
class RChain : public RChainAbstract, public ChainList 
{
public:
	typedef std::vector<RFrame> RFramesList;
	RFramesList           joints;	//!< stores the RFrame description of each joint
	RFrame				  mp_base;	//!< mounting plate wrt base
public:

	//! Current state of the kin. chain is lost when assigning chains.
	//! FwdKin and variants should be called again.
	RChain& operator=(const ChainList& C) {
		ChainList::operator =(C);
		return *this;
	}


	//! overload of the method out of ChainList to define an operation
	//! that is called each time that a ChainElement is added to the ChainList.
	virtual void Add(ChainElement* el);
		
	//! to help the compiler with the overloading.
	virtual void Add(const ChainList* el) {
		ChainList::Add(el);
	}


	// --- IMPLEMENTATION OF RChainAbstract INTERFACE :

	virtual RFrame RFwdKin(const NEWMAT::ColumnVector& jval,const NEWMAT::ColumnVector& vel);
	virtual RTwist RTwist_base_mp(int joint) const;
	virtual RTwist RTwist_base_base(int joint) const;
	virtual RTwist RTwist_mp_mp(int joint) const;
	virtual RTwist RTwist_mp_base(int joint) const;
	virtual int NrOfJoints() const;
	virtual RFrame RMountingPlate() const;

	// --- IMPLEMENTATION OF ChainAbstract INTERFACE :
	//! calls RFwdKin
	virtual Frame FwdKin(const NEWMAT::ColumnVector& v);
	virtual Twist Twist_base_base(int joint) const;
	virtual Twist Twist_base_mp(int joint) const;
	virtual Twist Twist_mp_mp(int joint) const;
	virtual Twist Twist_mp_base(int joint) const;
	virtual Frame MountingPlate() const;

	virtual ~RChain();
};


#ifdef CHAIN_INLINE
	#include <kinematics/rchain.inl>
#endif

#ifdef USE_NAMESPACE
}
#endif


#endif
