/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 15:59:17 CET 2005  chainlist.h 

                        chainlist.h -  description
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
 *		$Id: chainlist.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/
#ifndef CHAINLIST_H
#define CHAINLIST_H

#include <frames/frames.h>
#include <frames/frames_io.h>
#include <chain/chainelement.h>
#include <vector>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/*
 * The ChainList class manages a list of ChainElements and thus a serial
 * kinematic chain, but does not provide any operations on it except for file I/O
 * and assignment/copy operations.  The assignment/copy and file operations
 * will only take into account data from the ChainList classes, even when
 * they are called from derivate classes.  This takes into account the fact
 * that the derivate classes only contain cached values as a state, to avoid
 * duplicate calculations.  The Add(ChainElement*) class can be overridden by
 * the derivates to provide an initialisation of their temporary state when reading
 * from a stream or constructing a chain from code.
 * \par Type
 * Concrete implementation
 */
class ChainList 
{
public:
	typedef std::vector<ChainElement*> ChainElementList;
	ChainElementList list;              //!< list of descriptions of a joint axis.
	Frame                 mp;           //!< last axis to mounting plate transformation
public:
	/** 
	 * The empty constructor creates a valid ChainList with no joints
	*/
	ChainList(){mp=Frame::Identity();}	

	/**
	 * ChainList and derivates can be copied, but the extra information contained
	 * in the derivates will not be copied.  This allows us to define copy operations
	 * between different types of derivates of ChainList.
	 */
	ChainList(const ChainList& c);

	//! see the remarks for the copy constructor
	ChainList& operator = ( const ChainList& c);

	//! Adds the inverse of chain L to this chain (including mp-definitions)
	virtual void Inverse(const ChainList* L);

	//! Adds chain L to this chain (including mp-definitions)
	virtual void Add(const ChainList* L);
	
	//! Sets the mounting plate for this chain 
	virtual void SetMountingPlate(const Frame& _mp) {mp=_mp;}

	//! Add one axis definition el to this chain.
	virtual void Add(ChainElement* el);
	
	//! @return number of joints of this ChainList object.
	virtual int NrOfJoints() const {
		return list.size();
	}

	/** Writes ChainList or its derivate from a stream
	 *  The stream does not contain any information regarding the derivate classes.
	 */
	virtual void Write(ostream& os) const;
	
	/** Reads ChainList or its derivate from a stream
	 *  The stream does not contain any information regarding the derivate classes.
	 */
	virtual void Read(istream& is);
	
	//! Destroys contents and reinitializes the ChainList
	virtual void Destroy();
	

	virtual ~ChainList();
};


#ifdef USE_NAMESPACE
}
#endif


#endif
