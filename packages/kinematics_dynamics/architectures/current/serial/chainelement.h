/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 15:59:17 CET 2005  chainelement.h 

                        chainelement.h -  description
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
 *		To describe a kinematic chain. Defines ChainElement_XXX objects.
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
 *		$Id: chainelement.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/
#ifndef CHAINELEMENT_H
#define CHAINELEMENT_H

#include <frames/frames.h>
#include <frames/frames_io.h>
#include <vector>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif




//---------------------------------------------------------------------
//   DEFINITION OF CHAINELEMENT AND DERIVATES
//---------------------------------------------------------------------

/**
 * ChainElement describes one joint axis in a kinematic chain.
 * only translations and rotations are currently implemented, but this is not 
 * limited to simple rotations and translations.  Each ChainElement is a frame 
 * transformation, followed by a translation or a rotation.  
 * 
 * There are
 * some functions(Tf,RTf,RRTf,RNTf) defined that take a ChainElement as argument and these
 * functions have to know the exact type of the ChainElement, therefore, a 
 * GetType() operation is provided.  These functions are not defined as methods of this
 * class to avoid unneccessary coupling.  Not all the rframes files have to be used, if one
 * only needs  simple forward position kinematics
 * \par Type
 * Abstract base class
 */
class ChainElement 
{
public:
	double scale; //!< sign of the joint axis;
	Frame tf;     //!< transformation of the next axis wrt the frame of the _previousaxis

	/**
	 * Constructs a ChainElement
	 * @param _t transformation of the next axis wrt the frame of the _previousaxis
	 * @param _scale scale factor, mostly containing -1 or 1 to indicate the positive
	 *               direction of the axis.
	 */
	ChainElement(const Frame& _t,double _scale):tf(_t),scale(_scale) {}
	/**
	 * A descriptor for the type of the ChainElement derivate
	 */
	typedef enum {RotX=1,RotY,RotZ,TransX,TransY,TransZ} ElementType;
	
	/**
	 * @return a twist corresponding to this ChainElement.  This twist
	 * is expressed in the ChainElement itself, so it is always a unittwist.
	 */
	virtual Twist UnitTwist() const = 0;
	
	/**
	 * Writes the ChainElement to a stream.
	 */
	virtual void Write(ostream& os) const = 0;
	
	/**
	 * Factory method (instead of a seperate factory class)
	 * Static routine that reads a ChainElement from a stream
	 * and returns a ChainElement* of the correct derivate class
	 */
	static ChainElement* Read(istream& is);

	/**
	 * @return the descriptor of the ChainElement derivate class.
	 */
	virtual int Type() const  = 0;

	/**
	 * Creates a new copy of the element on the heap and gives back
	 * a pointer to it.  Used as VIRTUAL COPY OPERATOR.
	 */
	virtual ChainElement* Copy() const = 0;

	/**
	 * Creates a new copy of the element on the heap and gives back
	 * a pointer to it.  Used as VIRTUAL COPY OPERATOR.
	 */
	virtual ChainElement* Copy(const Frame& _tf,double scale) const = 0;

	virtual ~ChainElement() {}
};



class ChainElement_RotX : public ChainElement 
{
public:
		ChainElement_RotX(const Frame& tf_prev_next,double scale): 
			ChainElement(tf_prev_next,scale){}
		virtual Twist UnitTwist() const;
		virtual void Write(ostream& os) const;
		virtual int Type() const  { return RotX;}
		virtual ChainElement* Copy() const {return new ChainElement_RotX(tf,scale);}
		virtual ChainElement* Copy(const Frame& _tf,double scale) const {return new ChainElement_RotX(_tf,scale);}
		virtual ~ChainElement_RotX() {}
};

class ChainElement_TransX : public ChainElement 
{
	public:
		ChainElement_TransX(const Frame& tf_prev_next,double scale): ChainElement(tf_prev_next,scale){}
		virtual Twist UnitTwist() const;
		virtual void Write(ostream& os) const;
		virtual int Type() const  { return TransX;}
		virtual ChainElement* Copy() const {return new ChainElement_TransX(tf,scale);}
		virtual ChainElement* Copy(const Frame& _tf,double scale) const {return new ChainElement_TransX(_tf,scale);}
		virtual ~ChainElement_TransX() {}
};

class ChainElement_RotY : public ChainElement 
{
	public:
		ChainElement_RotY(const Frame& tf_prev_next,double scale): ChainElement(tf_prev_next,scale){}
		virtual Twist UnitTwist() const;
		virtual void Write(ostream& os) const;
		virtual int Type() const  { return RotY;}
		virtual ChainElement* Copy() const {return new ChainElement_RotY(tf,scale);}
		virtual ChainElement* Copy(const Frame& _tf,double scale) const {return new ChainElement_RotY(_tf,scale);}
		virtual ~ChainElement_RotY() {}
};

class ChainElement_TransY : public ChainElement 
{
	public:
		ChainElement_TransY(const Frame& tf_prev_next,double scale): ChainElement(tf_prev_next,scale){}
		virtual Twist UnitTwist() const;
		virtual void Write(ostream& os) const;
		virtual int Type() const  { return TransY;}
		virtual ChainElement* Copy() const {return new ChainElement_TransY(tf,scale);}
		virtual ChainElement* Copy(const Frame& _tf,double scale) const {return new ChainElement_TransY(_tf,scale);}
		virtual ~ChainElement_TransY() {}
};


class ChainElement_RotZ : public ChainElement 
{
	public:
		ChainElement_RotZ(const Frame& tf_prev_next,double scale): ChainElement(tf_prev_next,scale){}
		virtual Twist UnitTwist() const;
		virtual void Write(ostream& os) const;
		virtual int Type() const  { return RotZ;}
		virtual ChainElement* Copy() const {return new ChainElement_RotZ(tf,scale);}
		virtual ChainElement* Copy(const Frame& _tf,double scale) const {return new ChainElement_RotZ(_tf,scale);}
		virtual ~ChainElement_RotZ() {}
};

class ChainElement_TransZ : public ChainElement 
{
	public:
		ChainElement_TransZ(const Frame& tf_prev_next,double scale): ChainElement(tf_prev_next,scale){}
		virtual Twist UnitTwist() const;
		virtual void Write(ostream& os) const;
		virtual int Type() const  { return TransZ;}
		virtual ChainElement* Copy() const {return new ChainElement_TransZ(tf,scale);}
		virtual ChainElement* Copy(const Frame& _tf,double scale) const {return new ChainElement_TransZ(_tf,scale);}
		virtual ~ChainElement_TransZ() {}
};






#ifdef USE_NAMESPACE
}
#endif


#endif
