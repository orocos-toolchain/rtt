/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:38 CET 2005  fvectorvs.h 

                        fvectorvs.h -  description
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
 *      FVectorVSIZE is a fast and simple class for a vector on which certain
 *      arithmetic functions are defined.  The storage space is
 *      located on the stack. Completely inlined.  Operators and function
 *      only operate on the VSIZE first elements, but always N elements are 
 *      allocated.  (one cannot write portable code that allocates variable
 *      size memory on the stack).
 *       
 *  \author 
 *      Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *      ORO_Geometry V2
 *
 *  \par History
 *      - $log$ 
 *
 *  \par Release
 *      $Id: fvectorvs.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/

#ifndef FVectorVSIZEVS_H
#define FVectorVSIZEVS_H
#include "utility.h"
#include <math.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


/**
 * FVectorVSIZE is a vectortype with following properties :
 *  - fully inlined code.
 *  - memory is allocated on the stack
 *  - math. operators are defined on the vector
 *  - fixed size N is allocated on the stack, but calculations are only
 *    on the first VSIZE elements.
 *  - element type T
 *  - provides a SetToZero and Equal function
 * \par Class Type
 * Concrete implementation
 *
 */
template <class T,int N>
class FVectorVSIZE
{
public:
// = Private members
    T vect[N];

// = Constructors
    FVectorVSIZE() {}
    
    FVectorVSIZE(const T arg[]) {
        int count=VSIZE;
        while (count--) vect[count] = arg[count];
    }

    FVectorVSIZE(typename TI<T>::Arg arg1) {vect[0]=arg1;}
    
    FVectorVSIZE(typename TI<T>::Arg arg1,typename TI<T>::Arg arg2) {vect[0]=arg1;vect[1]=arg2;}
    
    FVectorVSIZE(typename TI<T>::Arg arg1,typename TI<T>::Arg arg2,typename TI<T>::Arg arg3) {vect[0]=arg1;vect[1]=arg2;vect[2]=arg3;}

    // always provide a copy constructor
    FVectorVSIZE(const FVectorVSIZE<T,N>& arg) {
        int count=VSIZE;
        while (count--) vect[count] = arg.vect[count];
    }

    static FVectorVSIZE Zero() {
        FVectorVSIZE<T,N> tmp;
        SetToZero(tmp);
        return tmp;
    }

// = Assignment operators
    FVectorVSIZE<T,N>& operator = (const FVectorVSIZE<T,N>& arg) {
        int count;
        count=VSIZE;
        while (count--) vect[count] = arg.vect[count];
        return *this;
    }
    
    FVectorVSIZE<T,N>& operator +=(const FVectorVSIZE<T,N>& arg) {  
        int count;
        count=VSIZE;
        while (count--) vect[count] += arg.vect[count];
        return *this;
    }
    FVectorVSIZE<T,N>& operator -=(const FVectorVSIZE<T,N>& arg) {
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
    // = Operators between FVectorVSIZE objects 
    friend FVectorVSIZE<T,N> operator + ( const FVectorVSIZE<T,N>& a,const FVectorVSIZE<T,N>& b);
    friend FVectorVSIZE<T,N> operator - ( const FVectorVSIZE<T,N>& a,const FVectorVSIZE<T,N>& b);
    friend FVectorVSIZE<T,N> operator - ( const FVectorVSIZE<T,N>& a);
    friend FVectorVSIZE<T,N> operator * ( const T& a,const FVectorVSIZE<T,N>& b);
    friend FVectorVSIZE<T,N> operator * ( const FVectorVSIZE<T,N>& a,const T& b);
    friend FVectorVSIZE<T,N> operator / ( const FVectorVSIZE<T,N>& a,const T& b);

    // = Functions that operate on FVectorVSIZE
    friend double Norm(const FVectorVSIZE<T,N>& a);
    // returns norm of the vector contained in FVectorVSIZE


    friend FVectorVSIZE<T,N> LinComb(const T& alfa,const FVectorVSIZE<T,N>& a,
        const T& beta,const FVectorVSIZE<T,N>& b );
    
    friend void LinCombR(const T& alfa,const FVectorVSIZE<T,N>& a,
            const T& beta,const FVectorVSIZE<T,N>& b,FVectorVSIZE<T,N>& result );
*/  
};

template <class T,int N>
INLINE FVectorVSIZE<T,N> operator + ( const FVectorVSIZE<T,N>& a,const FVectorVSIZE<T,N>& b) {
    int i;
    FVectorVSIZE<T,N> tmp;
    i=VSIZE;while (i--)
        tmp[i]=a[i]+b[i];
    return tmp;
}

template <class T,int N>
INLINE FVectorVSIZE<T,N> operator - ( const FVectorVSIZE<T,N>& a,const FVectorVSIZE<T,N>& b) {
    int i;
    FVectorVSIZE<T,N> tmp;
    i=VSIZE;while (i--)
        tmp[i]=a[i]-b[i];
    return tmp;
}

template <class T,int N>
INLINE FVectorVSIZE<T,N> operator - ( const FVectorVSIZE<T,N>& a) {
    int i;
    FVectorVSIZE<T,N> tmp;
    i=VSIZE;while (i--)
        tmp[i]=-a[i];
    return tmp;
}

template <class T,int N>
INLINE FVectorVSIZE<T,N> operator * (typename TI<T>::Arg a,const FVectorVSIZE<T,N>& b) {
    int i;
    FVectorVSIZE<T,N> tmp;
    i=VSIZE;while (i--)
        tmp[i]=a*b[i];
    return tmp;
}

template <class T,int N>
INLINE FVectorVSIZE<T,N> operator * ( const FVectorVSIZE<T,N>& a,typename TI<T>::Arg b) {
    int i;
    FVectorVSIZE<T,N> tmp;
    i=VSIZE;while (i--)
        tmp[i]=a[i]*b;
    return tmp;
}

template <class T,int N>
INLINE FVectorVSIZE<T,N> operator / ( const FVectorVSIZE<T,N>& a,typename TI<T>::Arg b) {
    FVectorVSIZE<T,N> tmp;
    int i;
    i=VSIZE;while (i--)
        tmp[i]=a[i]/b;
    return tmp;
}


template <class T,int N>
INLINE double Norm(const FVectorVSIZE<T,N>& a)  {
    double sum;
    int i;
    sum = 0;
    i=VSIZE;while (i--) {
        sum+=sqr(Norm(a.vect[i]));
    }
    return sqrt(sum);
}
// returns norm of the vector contained in FVectorVSIZE

template <class T,int N>
INLINE FVectorVSIZE<T,N> LinComb(typename TI<T>::Arg alfa,const FVectorVSIZE<T,N>& a,
                            typename TI<T>::Arg beta,const FVectorVSIZE<T,N>& b ) {
    FVectorVSIZE<T,N> tmp;
    int i;
    i=VSIZE;while (i--) LinCombR(alfa,a[i],beta,b[i],tmp[i]);
    return tmp;
}

template <class T,int N>
INLINE void LinCombR(typename TI<T>::Arg alfa,const FVectorVSIZE<T,N>& a,
                     typename TI<T>::Arg beta,const FVectorVSIZE<T,N>& b,FVectorVSIZE<T,N>& result ) {
            int i;
            i=VSIZE;while (i--) LinCombR(alfa,a[i],beta,b[i],result[i]);
}



template <class T, int N>
INLINE void SetToZero(FVectorVSIZE<T,N>& value) {
    int i;
    i=VSIZE;while (i--) SetToZero(value[i]);
}



template <class T,int N>
bool Equal(const FVectorVSIZE<T,N>& a,const FVectorVSIZE<T,N>& b,double eps) {
    int i;
    for (i=0;i<VSIZE;i++) {
        if (!Equal(a[i],b[i],eps)) return false;
    }
    return true;
}



#ifdef USE_NAMESPACE
}
#endif

#endif 
