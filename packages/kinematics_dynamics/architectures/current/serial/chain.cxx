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
 *		$Id: chain.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <newmat/newmat.h>
#include <newmat/newmatap.h>
#include <chain/chain.h>
#include <utilities/error_stack.h>
#include <utilities/utility_newmat.h>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif

Frame Chain::MountingPlate() const
// returns the actual mounting plate wrt base frame.
{
	return mp_base;
}

Twist Chain::Twist_base_base(int joint)  const{
	return joints[joint] * list[joint]->UnitTwist();
}

Twist Chain::Twist_base_mp(int joint)  const{
	return (joints[joint].M * 
		    list[joint]->UnitTwist()).RefPoint(mp_base.p-joints[joint].p);
}

Twist Chain::Twist_mp_mp(int joint)  const{
	return  mp_base.M.Inverse(
				(joints[joint].M *	list[joint]->UnitTwist()).
					RefPoint(mp_base.p-joints[joint].p)
			);
// you could also write :
//   return  mp_base.Inverse( (joints[joint]*list[joint]->UnitTwist()) );
// but it is not as efficient (position is transformed to base and to mp,
// this is avoided above.
}


Twist Chain::Twist_mp_base(int joint)  const{
	return mp_base.M.Inverse(
		    joints[joint] * list[joint]->UnitTwist()
		);
}

int Chain::NrOfJoints() const {
		return ChainList::NrOfJoints();
}
void Chain::Add(ChainElement* el) {
		ChainList::Add(el);
		joints.insert(joints.end(),Frame());
}

Chain::~Chain() {
	joints.erase(joints.begin(),joints.end());
}

Frame Chain::FwdKin(const NEWMAT::ColumnVector& v) {
	int i;
	int s=NrOfJoints();
	Frame tmp;
	tmp = Frame::Identity();
 	for (i=0;i<s;i++) {
		// tmp = Frame w.r.t base, from axis it
		tmp = tmp * Tf(*list[i],v(i+1));
		joints[i] = tmp;
	}
	tmp = tmp* mp;
	mp_base=tmp;
	return tmp;
}


/*
// quick - and - dirty inverse kinematics of a chain : 
// not very efficient
void InverseKin(ChainAbstract& C,
				const Frame& goal,
				NEWMAT::ColumnVector& v,int nrofit) {
	int n = v.Nrows();
	int m = 6;
	NEWMAT::Matrix J(m,n);
	NEWMAT::Matrix Ji(n,m);
	NEWMAT::Matrix Jproj(n,n);
	NEWMAT::DiagonalMatrix D(m);
	NEWMAT::Matrix U(m,m);
	NEWMAT::Matrix V(m,n);
	NEWMAT::ColumnVector qdot(n);
	Frame current;
	Twist t;


	double K    = 0.1;
	double alpha = 0.1;
	int i,j;

	qdot = 0;
	for (i=0;i<nrofit;i++) {
		current=C.FwdKin(v);
		C.Geometric_Jacobian_base_mp(J);
		t=FrameDifference(goal,current);
		NEWMAT::SVD(J.t(),D,V,U);
		PInvDiagonalMatrix(D);
		Ji= V*D*U.t();
		Jproj = -Ji*J;
		for (j=1;j<=J.Ncols();j++) Jproj(j,j)+=1;
		// some criterium :
		//qdot = 0;
		qdot = -v;
		v = v + K*Ji*CV(t) + alpha*Jproj*qdot;
	}
}
*/


#ifdef USE_NAMESPACE
}
#endif
