/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  chainlist.cxx 

                        chainlist.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 *		$Id: chainlist.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <utilities/utility.h>
#include <utilities/error.h>
#include <utilities/error_stack.h>
#include <chain/chainlist.h>



#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif


//---------------------------------------------------------------------
//   IMPLEMENTATION OF CHAINLIST
//---------------------------------------------------------------------

void ChainList::Add(ChainElement* el) {
	list.insert(list.end(),el);
}

void ChainList::Inverse(const ChainList* L) {
	int i;
	int n = L->NrOfJoints();
	if (n!=0) {
		Add(  L->list[n-1]->Copy(mp*L->mp.Inverse(),-L->list[n-1]->scale ) );
		mp = L->list[0]->tf.Inverse();
	} else {
		mp = L->mp.Inverse();
	}
	for (i=n-1;i>0;i--) {
		Add( L->list[i-1]->Copy(L->list[i]->tf.Inverse(),-L->list[i-1]->scale) );
	}	
}

void ChainList::Add(const ChainList* L) {
	int i;
	int n = L->NrOfJoints();
	if (n!=0) {
		Add(  L->list[0]->Copy(mp*L->list[0]->tf,L->list[0]->scale));
		mp = L->mp;
	} else {
		mp = mp*L->mp;
	}
	for (i=1;i<n;i++) {
		Add( L->list[i]->Copy() );
	}	
}

void ChainList::Write(ostream& os) const {
	os << "CHAIN [" << endl;
	os << "   NrOfJoints " << NrOfJoints() << endl;
	int i;
	for (i=0;i<NrOfJoints();i++) {
		list[i]->Write(os);
	}
	os << "MOUNTINGPLATE[" << endl;
	os << mp << "]" <<endl;
	os << "]" << endl;
}

void ChainList::Read(istream& is) {
	IOTrace("ChainList::Read");
	int nr;
	Eat(is,"CHAIN [");
	Eat(is,"NrOfJoints");
	is >> nr;
	for (int i=0;i<nr;i++) {
		Add(ChainElement::Read(is));
	}
	Eat(is,"MOUNTINGPLATE");
	Eat(is,"[");
	is >> mp;
	Eat(is,"]");
	Eat(is,"]");
	IOTracePop();
}
ChainList::ChainList(const ChainList& c):mp(c.mp) {
		for (int i=0;i<c.NrOfJoints();i++) {
			Add(c.list[i]->Copy());
		}
	}

ChainList& ChainList::operator = ( const ChainList& c) {
		for (int i=0;i<c.NrOfJoints();i++) {
			Add(c.list[i]->Copy());
		}
		mp = c.mp;
		return *this;
	}

ChainList::~ChainList() {
	Destroy();
}

void ChainList::Destroy() {
	ChainElementList::iterator it;
	for (it=list.begin();it!=list.end();it++) {
		delete *it;
	}
	list.erase(list.begin(),list.end());

}

#ifdef USE_NAMESPACE
}
#endif
