/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 15:59:17 CET 2005  rrchain.h 

                        rrchain.h -  description
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
 *		$Id: rrchain.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef RRCHAIN_H
#define RRCHAIN_H

#include <rframes/rrframes.h>
#include <rframes/rrframes_io.h>
#include <chain/chainlist.h>
#include <chain/rrchainabstract.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

//! RRChain implements the RRChainAbstract, RChainAbstract and the ChainList interface
class RRChain : 
		public RRChainAbstract,
		public ChainList 
{
public:
	typedef std::vector<RRFrame> RRFramesList;
	RRFramesList           joints;	//!< position of the joints and 1st and 2nd derivative
	RRFrame				  mp_base;	//!< mounting plate wrt base
public:
	//! Current state of the kin. chain is lost when assigning chains.
	//! FwdKin and variants should be called again.
	RRChain& operator=(const ChainList& C) {
		ChainList::operator =(C);
		return *this;
	}


	//! overload of the method out of ChainList to define an operation
	//! that is called each time that a ChainElement is added to the ChainList.
	virtual void Add(ChainElement* el);

	//! to help compiler resolve the overloading of Add(...)
	virtual void Add(const ChainList* el) {
		ChainList::Add(el);
	}
	
	// --- IMPLEMENTATION OF RRChainAbstract INTERFACE :

	virtual RRFrame RRFwdKin(
		const NEWMAT::ColumnVector& jval,
		const NEWMAT::ColumnVector& vel,
		const NEWMAT::ColumnVector& acc
	);

	virtual RRTwist RRTwist_base_mp(int joint) const;
	virtual RRTwist RRTwist_base_base(int joint) const;
	virtual RRTwist RRTwist_mp_mp(int joint) const;
	virtual RRTwist RRTwist_mp_base(int joint) const;
	virtual int NrOfJoints() const;
	virtual RRFrame RRMountingPlate() const;

	// --- IMPLEMENTATION OF RChainAbstract INTERFACE :
	//! calls RRFwdKin
	virtual RFrame RFwdKin(const NEWMAT::ColumnVector& jval,const NEWMAT::ColumnVector& vel);
	virtual RTwist RTwist_base_mp(int joint) const;
	virtual RTwist RTwist_base_base(int joint) const;
	virtual RTwist RTwist_mp_mp(int joint) const;
	virtual RTwist RTwist_mp_base(int joint) const;
	virtual RFrame RMountingPlate() const;

	// --- IMPLEMENTATION OF ChainList INTERFACE :
	//! calls RRFwdKin
	virtual Frame FwdKin(const NEWMAT::ColumnVector& v);
	virtual Twist Twist_base_base(int joint) const;
	virtual Twist Twist_base_mp(int joint) const;
	virtual Twist Twist_mp_mp(int joint) const;
	virtual Twist Twist_mp_base(int joint) const;
	virtual Frame MountingPlate() const;
	virtual ~RRChain();
};


#ifdef USE_NAMESPACE
}
#endif

#endif
