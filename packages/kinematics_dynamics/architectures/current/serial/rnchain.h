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
 *		$Id: rnchain.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef RNCHAIN_H
#define RNCHAIN_H


#include <rframes/rnframes.h>
#include <rframes/rnframes_io.h>
#include <chain/chainlist.h>
#include <chain/rnchainabstract.h>
#include <utilities/utility_newmat.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

//! RChain implements the RNChainAbstract and the ChainList interface
class RNChain : public RNChainAbstract, public ChainList 
{
public:
	typedef std::vector<RNFrame> RNFramesList;
	RNFramesList           joints;	//!< position of the joints and its derivative to VSIZE variables
	RNFrame				  mp_base;	//!< mounting plate wrt base
public:

	//! Current state of the kin. chain is lost when assigning chains.
	//! FwdKin and variants should be called again.
	RNChain& operator=(const ChainList& C) {
		ChainList::operator =(C);
		return *this;
	}

	//! overload of the method out of ChainList to define an operation
	//! that is called each time that a ChainElement is added to the ChainList.
	virtual void Add(ChainElement* el);

	//! to help the compiler with overload resolution
	virtual void Add(const ChainList* el) {
		ChainList::Add(el);
	}

	

	// --- IMPLEMENTATION OF RChainAbstract INTERFACE :
	virtual RNFrame RNFwdKin(const NEWMAT::ColumnVector& jval,const NEWMAT::Matrix& vel);
	virtual RNFrame RNFwdKin(const NEWMAT::ColumnVector& jval);
	

	virtual RNTwist RNTwist_base_mp(int joint) const;
	virtual RNTwist RNTwist_base_base(int joint) const;
	virtual RNTwist RNTwist_mp_mp(int joint) const;
	virtual RNTwist RNTwist_mp_base(int joint) const;
	virtual int NrOfJoints() const;

	virtual RNFrame RNMountingPlate() const;


	// --- IMPLEMENTATION OF ChainAbstract INTERFACE :
	//! calls RNFwdKin
	virtual Frame FwdKin(const NEWMAT::ColumnVector& v);

	virtual Twist Twist_base_base(int joint) const;
	virtual Twist Twist_base_mp(int joint) const;
	virtual Twist Twist_mp_mp(int joint) const;
	virtual Twist Twist_mp_base(int joint) const;
	virtual Frame MountingPlate() const;

	virtual ~RNChain();
};

#ifdef USE_NAMESPACE
}
#endif



#endif
