/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 15:59:17 CET 2005  chain.h 

                        chain.h -  description
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
 *		$Id: chain.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef __CHAIN_H
#define __CHAIN_H

#include <chain/chainelement.h>
#include <chain/chainlist.h>
#include <chain/chainabstract.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


/**
 * Forward position and velocity kinematics with Frame-like objects
 * @par type 
 * Concrete Interface implements ChainAbstract with the help of ChainList
 */
class Chain : public ChainAbstract,public ChainList 
{
public:
	
	typedef std::vector<Frame> FramesList;	

	//! intermediate calculation of the position of the joint axes 
	FramesList joints;

	//! intermediate calculation of the mp wrt base
	Frame				  mp_base;	
public:

	/** Current state of the kin. chain is lost when assigning chains.
	 * FwdKin and variants should be called again.
	 * @param C chainlist or derivate class to assign from
	 * @return the newly assigned chain
	 */
	Chain& operator=(const ChainList& C) {
		ChainList::operator =(C);
		return *this;
	}


	/** overload of the method out of ChainList to define an operation
	 * that is called each time that a ChainElement is added to the ChainList.
	 */
	virtual void Add(ChainElement* el);

	/** I have no idea why this method is necessary 
	 * just to make clear what method to call when overloading
	 */
	virtual void Add(const ChainList* el) {
		ChainList::Add(el);
	}
	

	virtual Frame FwdKin(const NEWMAT::ColumnVector& v);

	virtual Twist Twist_base_base(int joint) const;


	virtual Twist Twist_base_mp(int joint) const;

	virtual Twist Twist_mp_mp(int joint) const;

	virtual Twist Twist_mp_base(int joint) const;
	
	virtual int NrOfJoints() const;
	
	virtual Frame MountingPlate() const;

	virtual ~Chain();
};

/** quick - and - dirty inverse kinematics of a chain : 
 * not very efficient
 * @param C kinematic chain
 * @param goal goal frame
 * @param v    will contain the joint values
 * @param nrofit contains maximal number of iterations.
 */
void InverseKin(ChainAbstract& C,
				const Frame& goal,
				NEWMAT::ColumnVector& v,int nrofit);


#ifdef USE_NAMESPACE
}
#endif

#endif
