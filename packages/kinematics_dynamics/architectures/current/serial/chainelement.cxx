/*****************************************************************************
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
 *		$Id: chainelement.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <utilities/utility.h>
#include <utilities/error.h>
#include <utilities/error_stack.h>
#include <chain/chainelement.h>

 

#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif

void ChainElement_RotX::Write(ostream& os) const {
	os << "ROTX[ " << scale <<endl<< tf <<"]"<< endl;
}
void ChainElement_RotY::Write(ostream& os) const {
	os << "ROTY[" << scale <<endl<< tf <<"]"<<endl;
}
void ChainElement_RotZ::Write(ostream& os) const {
	os << "ROTZ[" << scale << endl<< tf <<"]"<< endl;
}
void ChainElement_TransX::Write(ostream& os) const {
	os << "TRANSX[" << scale << endl<< tf<< "]"<< endl;
}
void ChainElement_TransY::Write(ostream& os) const {
	os << "TRANSY[" << scale << endl<< tf <<"]"<< endl;
}

void ChainElement_TransZ::Write(ostream& os) const {
	os << "TRANSZ[" << scale << endl<< tf <<"]"<< endl;
}

//---------------------------------------------------------------------
//   IMPLEMENTATION OF CHAINELEMENT_XXXX
//---------------------------------------------------------------------
ChainElement* ChainElement::Read(istream& is) {
	IOTrace("ChainElement::Read");
	Frame f;
	double scale;
	char storage[10];
	EatWord(is,"[",storage,10);
	Eat(is,'[');
	is >> scale;
	is >> f;
	Eat(is,']');
	IOTracePop();
	if (strncmp(storage,"ROT",3)==0) {
		switch (storage[3]) {
		case 'X' :
			return new ChainElement_RotX(f,scale);
		case 'Y':
			return new ChainElement_RotY(f,scale);
		case 'Z' :
			return new ChainElement_RotZ(f,scale);
		}
	} 
	if (strncmp(storage,"TRANS",5)==0) {
		switch (storage[5]) {
		case 'X' :
			return new ChainElement_TransX(f,scale);
		case 'Y':
			return new ChainElement_TransY(f,scale);
		case 'Z' :
			return new ChainElement_TransZ(f,scale);
		}
	} 
	throw Error_Chain_Unexpected_id();
}





Twist ChainElement_TransX::UnitTwist() const {
	return Twist(Vector(scale,0,0),Vector(0,0,0));
}

Twist ChainElement_TransY::UnitTwist() const {
	return Twist(Vector(0,scale,0),Vector(0,0,0));
}

Twist ChainElement_TransZ::UnitTwist() const {
	return Twist(Vector(0,0,scale),Vector(0,0,0));
}

Twist ChainElement_RotX::UnitTwist() const {
	return Twist(Vector(0,0,0),Vector(scale,0,0));
}

Twist ChainElement_RotY::UnitTwist() const {
	return Twist(Vector(0,0,0),Vector(0,scale,0));
}

Twist ChainElement_RotZ::UnitTwist() const {
	return Twist(Vector(0,0,0),Vector(0,0,scale));
}



#ifdef USE_NAMESPACE
}
#endif
