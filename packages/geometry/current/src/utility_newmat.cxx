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
 *		$Id: utility_newmat.cpp,v 1.1.1.1.2.2 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/utility_newmat.h"
#include "geometry/utility_io.h"
#include "geometry/error_stack.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

//------------------------------------------------------------------------
//	IMPLEMENTATION OF :		Adj
//------------------------------------------------------------------------

void Adj(const NEWMAT::DiagonalMatrix& v,NEWMAT::DiagonalMatrix& r) {
	double s;
	int i;
	int n(v.Ncols());
	r(1) = 1;
	for (i=1;i<n;i++) {
		r(i+1) = r(i)*v(i);
		//  r(i) contains prod( v(1) ... v(i-1) )
	}
	s=v(n);
	for (i=n-1;i>=1;i--) {
		// s    contains prod( v(i+1) .. v(n) );
		r(i) *= s;
		s*=v(i);
		// r(i) contains prod( v(1) ..v(i-1).v(i+1)...v(n) )
	}

}

//------------------------------------------------------------------------
//	IMPLEMENTATION OF :		SumOfPrd
// Takes the sum over all the elements of the elementwise 
// product of A-transpose and B
//------------------------------------------------------------------------
double SumOfPrd(const NEWMAT::Matrix& A,const NEWMAT::Matrix& B) {
	double sum=0;
	for (int j=1;j<=6;j++)
		for (int k=1;k<=6;k++) {
			sum += A(k,j)*B(j,k);
	}
	return sum;
}




bool Equal(const NEWMAT::Matrix& a,const NEWMAT::Matrix& b,double eps) {
    if (a.Nrows()!=b.Nrows()) return false;
    if (a.Ncols()!=b.Ncols()) return false;
    for (int i=1;i<=a.Nrows();i++) {
        for (int j=1;j<=a.Ncols();j++) {
            if (!Equal(a(i,j),b(i,j),eps)) return false;
        }
    }
    return true;
}

const int MAXSTRINGSIZE=65536;

void ReadNamedValue(const char*name,istream& is,double& value) {
    IOTrace(std::string("ReadNamedValue : ")+name);
    char *storage= new char[MAXSTRINGSIZE];
    Eat(is,name);
    Eat(is,"[");
    is >> value;
    EatEnd(is,']');
    delete storage;
    IOTracePop();
}

void WriteNamedValue(const char* name,ostream& os,const double value) {
    os << name << "[" << value << "]";
}

void ReadNamedString(const char*name,istream& is,std::string& str) {
    IOTrace(std::string("ReadNamedString : ")+name);
    char *storage= new char[MAXSTRINGSIZE];
    Eat(is,name);
    Eat(is,"[");
    EatWord(is,"]",storage,MAXSTRINGSIZE);
    str = storage;
    EatEnd(is,']');
    delete storage;
    IOTracePop();
}

void WriteNamedString(const char* name,ostream& os,const std::string& str) {
    os << name << "[" << str.c_str() << "]";
}


void ReadNamedVector(const char*name,istream& is,NEWMAT::ColumnVector& vect,int size) {
    IOTrace(std::string("ReadNamedVector : ")+name);
    char storage[4];
    Eat(is,name);
    Eat(is,"[");
    int i;
    vect.ReSize(size);
    for (i=1;i<size;i++) {          
        is >> vect(i);
        EatWord(is,",",storage,4);
        if (strcmp(storage,"DEG")==0) {
            vect(i) *= deg2rad;
        }
        Eat(is,',');
    }
    is >> vect(size);
    EatWord(is,"]",storage,4);
    if (strcmp(storage,"DEG")==0) {
        vect(i) *= deg2rad;
    }       
    EatEnd(is,']');
    IOTracePop();
}

void WriteNamedVector(const char* name,ostream& os,const NEWMAT::ColumnVector& vect) {
    os << name << "[";
    for (int i=1;i<vect.Nrows();i++) {
        os << vect(i) << " ,\t";
    }
    os << vect(vect.Nrows());
    os << " ]";
}


void ReadNamedDiagMatrix(const char*name,istream& is,NEWMAT::DiagonalMatrix& vect,int size) {
    IOTrace(std::string("ReadNamedDiagMatrix : ")+name);
    char storage[4];
    Eat(is,name);
    Eat(is,"[");
    int i;
    vect.ReSize(size);
    for (i=1;i<size;i++) {          
        is >> vect(i);
        EatWord(is,",",storage,4);
        if (strcmp(storage,"DEG")==0) {
            vect(i) *= deg2rad;
        }
        Eat(is,',');
    }
    is >> vect(size);
    EatWord(is,"]",storage,4);
    if (strcmp(storage,"DEG")==0) {
        vect(i) *= deg2rad;
    }       
    EatEnd(is,']');
    IOTracePop();
}

void WriteNamedDiagMatrix(const char* name,ostream& os,const NEWMAT::DiagonalMatrix& vect) {
    os << name << "[";
    for (int i=1;i<vect.Nrows();i++) {
        os << vect(i) << " ,\t";
    }
    os << vect(vect.Nrows());
    os << " ]";
}

void PInvDiagonalMatrix(NEWMAT::DiagonalMatrix& D) {
    int i;
    double eps=1E-10;
    for (i=1;i<=D.Ncols();i++)
        if (fabs(D(i))<eps)
            D(i)=0;
        else
            D(i)=1/D(i);
}


#ifdef USE_NAMESPACE
}
#endif
