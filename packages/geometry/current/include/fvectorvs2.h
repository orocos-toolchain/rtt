/*****************************************************************************
 * \file  
 *      FVectorVSIZE is a fast and simple class for a vector on which certain
 *      arithmetic functions are defined.  The storage space is
 *      located on the stack. Completely inlined.  Operators and function
 *      only operate on the VSIZE first elements, but always N elements are 
 *      allocated.  (one cannot write portable code that allocates variable
 *      size memory on the stack).
 *      Also provides a scalar type S.
 * \todo
 *    Write a proper vector class that only allocates the desired number of elements
 *    It should not use the heap.  Could perhaps be done by writing a memory allocator
 *    for stl.
 *       
 *  \author 
 *      Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *      ORO_Geometry V0.2
 *
 *  \par History
 *      - $log$ 
 *
 *  \par Release
 *      $Id: fvectorvs2.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef FVectorVSIZE2_H
#define FVectorVSIZE2_H
#include "utility.h"
#include <math.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


/**
 * FVector2 is a vectortype with following properties :
 *  - fully inlined code.
 *  - memory is allocated on the stack
 *  - math. operators are defined on the vector
 *  - fixed size N is allocated on the stack, but calculations are only
 *    on the first VSIZE elements.
 *  - element type T
 *  - SCALAR TYPE S is explicitly specified. Scalar multiplication should exist
 *    between T and S.
 *  - provides a SetToZero and Equal function
 * \par Class Type
 * Concrete implementation
 *
 */
template <class T,int N,class S>
class FVectorVSIZE2 
{
public:
// = Private members
    T vect[N];

// = Constructors
    FVectorVSIZE2() {}
    
    FVectorVSIZE2(const T arg[]) {
        int count=VSIZE;
        while (count--) vect[count] = arg[count];
    }

    FVectorVSIZE2(const T& arg1) {vect[0]=arg1;}
    
    FVectorVSIZE2(const T& arg1,const T& arg2) {vect[0]=arg1;vect[1]=arg2;}
    
    FVectorVSIZE2(const T& arg1,const T& arg2,const T& arg3) {vect[0]=arg1;vect[1]=arg2;vect[2]=arg3;}

    // always provide a copy constructor
    FVectorVSIZE2(const FVectorVSIZE2<T,N,S>& arg) {
        int count=VSIZE;
        while (count--) vect[count] = arg.vect[count];
    }

    static FVectorVSIZE2 Zero() {
        FVectorVSIZE2<T,N,S> tmp;
        SetToZero(tmp);
        return tmp;
    }

// = Assignment operators
    FVectorVSIZE2<T,N,S>& operator = (const FVectorVSIZE2<T,N,S>& arg) {
        int count;
        count=VSIZE;
        while (count--) vect[count] = arg.vect[count];
        return *this;
    }
    
    FVectorVSIZE2<T,N,S>& operator +=(const FVectorVSIZE2<T,N,S>& arg) {    
        int count;
        count=VSIZE;
        while (count--) vect[count] += arg.vect[count];
        return *this;
    }
    FVectorVSIZE2<T,N,S>& operator -=(const FVectorVSIZE2<T,N,S>& arg) {
        int count=VSIZE;
        while (count--) vect[count] -= arg.vect[count];
        return *this;
    }

    // = additional operators for easy use :
    T& operator[] (int i) {
        return vect[i];
    }

    const T& operator[] (int i) const {
        return vect[i];
    }

    // = Utility functions
    
    double max_abs() {
        double tmp(0.0);
        int i;
        i=VSIZE;while (i--) {
            double el=fabs(vect[i]);
            if (el > tmp) tmp=el;
        }
        return tmp; 
    }
    // max. of the abs. value of each el
/*  
    // = Operators between FVectorVSIZE2 objects    
    friend FVectorVSIZE2<T,N,S> operator + ( const FVectorVSIZE2<T,N,S>& a,const FVectorVSIZE2<T,N,S>& b);
    friend FVectorVSIZE2<T,N,S> operator - ( const FVectorVSIZE2<T,N,S>& a,const FVectorVSIZE2<T,N,S>& b);
    friend FVectorVSIZE2<T,N,S> operator - ( const FVectorVSIZE2<T,N,S>& a);
    friend FVectorVSIZE2<T,N,S> operator * ( const T& a,const FVectorVSIZE2<T,N,S>& b);
    friend FVectorVSIZE2<T,N,S> operator * ( const FVectorVSIZE2<T,N,S>& a,const T& b);
    friend FVectorVSIZE2<T,N,S> operator / ( const FVectorVSIZE2<T,N,S>& a,const T& b);

    // = Functions that operate on FVectorVSIZE2
    friend double Norm(const FVectorVSIZE2<T,N,S>& a);
    // returns norm of the vector contained in FVectorVSIZE2


    friend FVectorVSIZE2<T,N,S> LinComb(const T& alfa,const FVectorVSIZE2<T,N,S>& a,
        const T& beta,const FVectorVSIZE2<T,N,S>& b );
    
    friend void LinCombR(const T& alfa,const FVectorVSIZE2<T,N,S>& a,
            const T& beta,const FVectorVSIZE2<T,N,S>& b,FVectorVSIZE2<T,N,S>& result );
*/  
};

template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> operator + ( const FVectorVSIZE2<T,N,S>& a,const FVectorVSIZE2<T,N,S>& b) {
    int i;
    FVectorVSIZE2<T,N,S> tmp;
    i=VSIZE;while (i--)
        tmp[i]=a[i]+b[i];
    return tmp;
}

template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> operator - ( const FVectorVSIZE2<T,N,S>& a,const FVectorVSIZE2<T,N,S>& b) {
    int i;
    FVectorVSIZE2<T,N,S> tmp;
    i=VSIZE;while (i--)
        tmp[i]=a[i]-b[i];
    return tmp;
}

template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> operator - ( const FVectorVSIZE2<T,N,S>& a) {
    int i;
    FVectorVSIZE2<T,N,S> tmp;
    i=VSIZE;while (i--)
        tmp[i]=-a[i];
    return tmp;
}

template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> operator * (const T& a,const FVectorVSIZE2<T,N,S>& b) {
    int i;
    FVectorVSIZE2<T,N,S> tmp;
    i=VSIZE;while (i--)
        tmp[i]=a*b[i];
    return tmp;
}

template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> operator * ( const FVectorVSIZE2<T,N,S>& a,const T& b) {
    int i;
    FVectorVSIZE2<T,N,S> tmp;
    i=VSIZE;while (i--)
        tmp[i]=a[i]*b;
    return tmp;
}
/* ? voor GNU ok, voor visual niet OK ? 
template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> operator * (TI<S>::Arg a,const FVectorVSIZE2<T,N,S>& b) {
    int i;
    FVectorVSIZE2<T,N,S> tmp;
    i=N;while (i--)
        tmp[i]=a*b[i];
    return tmp;
}

template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> operator * ( const FVectorVSIZE2<T,N,S>& a,TI<S>::Arg b) {
    int i;
    FVectorVSIZE2<T,N,S> tmp;
    i=N;while (i--)
        tmp[i]=a[i]*b;
    return tmp;
}
*/



template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> operator / ( const FVectorVSIZE2<T,N,S>& a,const T& b) {
    FVectorVSIZE2<T,N,S> tmp;
    int i;
    i=VSIZE;while (i--)
        tmp[i]=a[i]/b;
    return tmp;
}


template <class T,int N,class S>
INLINE double Norm(const FVectorVSIZE2<T,N,S>& a)  {
    double sum;
    int i;
    sum = 0;
    i=VSIZE;while (i--) {
        sum+=sqr(Norm(a.vect[i]));
    }
    return sqrt(sum);
}
// returns norm of the vector contained in FVectorVSIZE2

template <class T,int N,class S>
INLINE FVectorVSIZE2<T,N,S> LinComb(const T& alfa,const FVectorVSIZE2<T,N,S>& a,
                            const T& beta,const FVectorVSIZE2<T,N,S>& b ) {
    FVectorVSIZE2<T,N,S> tmp;
    int i;
    i=VSIZE;while (i--) LinCombR(alfa,a[i],beta,b[i],tmp[i]);
    return tmp;
}

template <class T,int N,class S>
INLINE void LinCombR(const T& alfa,const FVectorVSIZE2<T,N,S>& a,
                     const T&,const FVectorVSIZE2<T,N,S>& b,FVectorVSIZE2<T,N,S>& result ) {
            int i;
            i=VSIZE;while (i--) LinCombR(alfa,a[i],beta,b[i],result[i]);
}



template <class T, int N,class S>
INLINE void SetToZero(FVectorVSIZE2<T,N,S>& value) {
    int i;
    i=VSIZE;while (i--) SetToZero(value[i]);
}


template <class T,int N,class S>
bool Equal(const FVectorVSIZE2<T,N,S>& a,const FVectorVSIZE2<T,N,S>& b,double eps) {
    int i;
    for (i=0;i<VSIZE;i++) {
        if (!Equal(a[i],b[i],eps)) return false;
    }
    return true;
}

template <class T,int N,class S>
bool Equal(const T& a,const FVectorVSIZE2<T,N,S>& b,double eps) {
    int i;
    for (i=0;i<VSIZE;i++) {
        if (!Equal(a,b[i],eps)) return false;
    }
    return true;
}

template <class T,int N,class S>
bool Equal(const FVectorVSIZE2<T,N,S>& a,const T& b,double eps) {
    int i;
    for (i=0;i<VSIZE;i++) {
        if (!Equal(a[i],b,eps)) return false;
    }
    return true;
}


#ifdef USE_NAMESPACE
}
#endif

#endif 
