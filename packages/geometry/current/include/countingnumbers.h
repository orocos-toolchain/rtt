/** \file  
 *      This file contains the definition of classes for counting the number
 *      of operations on a float.
 * 
 * \warning
 *      The user should include an iostreams header before including this header.
 *      Output is provided via the iostreams operator.
 *       
 *  \author 
 *      Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *  \version 
 *      ORO_Geometry V0.2
 *   
 *  \par History
 *      - $log$ 
 *
 *  \par Release
 *      $Id: countingnumbers.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 *
 *  \todo
 *     static members are used.  This is not thread-safe : thread local storage 
 *     should be used.
 */
#ifndef COUNTINGNUMBERS
#define COUNTINGNUMBERS

#include "utility.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * Counting provides the same functionality as a double
 * but it also counts the number of operations.
 */
class Counting 
{
public:
    static int sum;     //!< The number of summations
    static int prd;     //!< The number of multiplications
    static int expc;    //!< The number of exponential function calls   
    static int trig;    //!< The number of trigonometric function calls
    static int fun;     //!< The number of other function calls
    static int assign;  //!< The number of assignments
    static int copy;    //!< The number of copies
public:
    // = Public member variables
    double value;
    // = Construction and assignment
    Counting() {}
    Counting(double val)        : value(val) {}

    Counting(const Counting& arg):value(arg.value) {copy++;}

    Counting& operator = (double val) {
        assign++;
        value = val;
        return *this;
    }

    Counting& operator = ( const Counting& arg) {
        assign++;
        value = arg.value;
        return *this;
    }
    
    INLINE static Counting Zero() {
            Counting tmp(0.0);
            return tmp;
        }
    INLINE static Counting Identity() {
            Counting tmp(1.0);
            return tmp;
    }

    //! Managing the counting process : reset the count
    static void ResetCount() {
        copy=assign=sum=prd=trig=expc=fun=0;
    }

    //! Managing the counting process : display the count
    static void DisplayCount();


    static int CountSum() {return sum;}

    static int CountPrd() {return prd;}

    static int CountTrig()  {return trig;}

    static int CountFunc()  {return fun;}

    static int CountExpLog()  {return expc;}

    static int CountAssign() {return assign;}

    static  int CountCopy()    {return copy;}

    Counting& operator *=(const Counting& arg) {
        prd++;      assign++;
        value *= arg.value;
        return *this;
    }


    Counting& operator /=(const Counting& arg) {
        prd++;      assign++;
        value /= arg.value;
        return *this;
    }

    Counting& operator +=(const Counting& arg) {
        sum++;      assign++;
        value += arg.value;
        return *this;
    }
    Counting& operator -=(const Counting& arg) {
        sum++;      assign++;
        value -= arg.value;
        return *this;
    }

    friend Counting operator *(const Counting& a,const Counting& b);
    friend Counting operator *(const Counting& a,const double& b);
    friend Counting operator *(const double& a,const Counting& b);
    friend Counting operator /(const Counting& a,const Counting& b);
    friend Counting operator /(const Counting& a,const double& b);
    friend Counting operator /(const double& a,const Counting& b);
    friend Counting operator +(const Counting& a,const Counting& b);
    friend Counting operator +(const Counting& a,const double& b);
    friend Counting operator +(const double& a,const Counting& b);
    friend Counting operator -(const Counting& a,const Counting& b);
    friend Counting operator -(const Counting& a,const double& b) ;
    friend Counting operator -(const double& a,const Counting& b) ;
    friend Counting operator -(const Counting& a);
    friend Counting exp(const Counting& a);
    friend Counting log(const Counting& a);
    friend Counting sin(const Counting& a);
    friend Counting cos(const Counting& a);
    friend Counting sinh(const Counting& a);
    friend Counting cosh(const Counting& a);
    friend Counting asin(const Counting& a);
    friend Counting asinh(const Counting& a);
    friend Counting tan(const Counting& a);
    friend Counting tanh(const Counting& a);
    friend Counting atan(const Counting& a);
    friend Counting atan2(const Counting& x,const Counting& y) ;
    friend Counting hypot(const Counting& x,const Counting& y);
    friend Counting pow(const Counting& a,double n);
    friend Counting acos(const Counting& a);
    friend Counting acosh(const Counting& a);
    friend Counting sqrt(const Counting& a);
    friend Counting sqr(const Counting& a);
    friend double Norm(const Counting& a) ;
    friend bool operator < (const Counting& a, const Counting& b) ;
    friend bool operator > (const Counting& a, const Counting& b) ;
    friend bool operator <= (const Counting& a, const Counting& b);
    friend bool operator >= (const Counting& a, const Counting& b);
    friend Counting LinComb(const Counting& alfa,const Counting& a,
            const Counting& beta,const Counting& b );

    friend void LinCombR(const Counting& alfa,const Counting& a,
            const Counting& beta,const Counting& b,Counting& result );
    
};











INLINE Counting operator *(const Counting& a,const Counting& b) {
     Counting::prd++;
    return a.value*b.value;
}

INLINE Counting operator *(const Counting& a,const double& b) {
     Counting::prd++;
    return a.value*b;
}

INLINE  Counting operator *(const double& a,const Counting& b) {
    Counting::prd++;
    return a*b.value;
}

 INLINE Counting operator /(const Counting& a,const Counting& b) {
    Counting::prd++;
    return a.value/b.value;
}

 INLINE Counting operator /(const Counting& a,const double& b) {
    Counting::prd++;
    return a.value/b;
}

 INLINE Counting operator /(const double& a,const Counting& b) {
    Counting::prd++;
    return a/b.value;
}


 INLINE Counting operator +(const Counting& a,const Counting& b) {
    Counting::sum++;
    return a.value+b.value;
}

 INLINE Counting operator +(const Counting& a,const double& b) {
    Counting::sum++;
    return a.value+b;
}

 INLINE Counting operator +(const double& a,const Counting& b) {
    Counting::sum++;
    return a+b.value;
}


 INLINE Counting operator -(const Counting& a,const Counting& b) {
    Counting::sum++;
    return a.value-b.value;
}

 INLINE Counting operator -(const Counting& a,const double& b) {
    Counting::sum++;
    return a.value-b;
}

 INLINE Counting operator -(const double& a,const Counting& b) {
    Counting::sum++;
    return a-b.value;
}

 INLINE Counting operator -(const Counting& a) {
    Counting::sum++;
    return -a.value;
}



 INLINE Counting exp(const Counting& a) {
    Counting::expc++;
    return Counting(exp(a.value));
}

 INLINE Counting log(const Counting& a) {
    Counting::expc++;
    return Counting(log(a.value));
}

 INLINE Counting sin(const Counting& a) {
    Counting::trig++;
    return Counting(sin(a.value));
}

 INLINE Counting cos(const Counting& a) {
    Counting::trig++;
    return Counting(cos(a.value));
}   

 INLINE Counting sinh(const Counting& a) {
    Counting::trig++;
    return Counting(sinh(a.value));
}

 INLINE Counting cosh(const Counting& a) {
    Counting::trig++;
    return Counting(cosh(a.value));
}

 INLINE Counting asin(const Counting& a) {
    Counting::trig++;
    return Counting(asin(a.value));
}

 INLINE Counting asinh(const Counting& a) {
    Counting::trig++;
    return Counting(asin(a.value));
}

 INLINE Counting tan(const Counting& a) {
    Counting::trig++;
    return Counting(tan(a.value));
}

 INLINE Counting tanh(const Counting& a) {
    Counting::trig++;
    return Counting(tanh(a.value));
}

 INLINE Counting atan(const Counting& a) {
    Counting::trig++;
    return Counting(atan(a.value));
}

 INLINE Counting atan2(const Counting& x,const Counting& y) {
    Counting::trig++;
    return Counting(atan2(x.value,y.value));
}

 INLINE Counting hypot(const Counting& x,const Counting& y) {
    Counting::trig++;
    return Counting(hypot(x.value,y.value));
}

 INLINE Counting pow(const Counting& a,double n) {
    Counting::expc++;
    return Counting(pow(a.value,n));
}

 INLINE Counting acos(const Counting& a) {
    Counting::trig++;
    return Counting(acos(a.value));
}

 INLINE Counting acosh(const Counting& a) {
    Counting::trig++;
    return Counting(acosh(a.value));
}

 INLINE Counting sqrt(const Counting& a) {
    Counting::fun++;
    return Counting(sqrt(a.value));
}

 INLINE Counting sqr(const Counting& a) {
    Counting::prd++;
    return Counting(a.value*a.value);
}

 INLINE double Norm(const Counting& a) {
    Counting::fun++;
    return Norm(a.value);
}


 INLINE bool operator < (const Counting& a, const Counting& b) {
    return (a.value < b.value);
}

 INLINE bool operator > (const Counting& a, const Counting& b) {
    return (a.value > b.value);
}

 INLINE bool operator <= (const Counting& a, const Counting& b) {
    return (a.value <= b.value);
}

 INLINE bool operator >= (const Counting& a, const Counting& b) {
    return (a.value >= b.value);
}

 INLINE Counting LinComb(const Counting& alfa,const Counting& a,
        const Counting& beta,const Counting& b ) {
            Counting::prd+=2;
            Counting::sum++;
            return Counting(LinComb(alfa.value,a.value,beta.value,b.value));
    }

 INLINE void LinCombR(const Counting& alfa,const Counting& a,
        const Counting& beta,const Counting& b,Counting& result ) {
            Counting::prd+=2;
            Counting::sum++;
            Counting::assign++;
            LinCombR(alfa.value, a.value,       beta.value, b.value,      result.value);
    }


INLINE  bool Equal(const Counting& y,const Counting& x,double eps=epsilon)
{
    return Equal(x.value,y.value,eps);
}

INLINE  void SetToZero(Counting& value)
    {
    SetToZero(value.value);
    }

INLINE  void SetToIdentity(Counting& value)
    {
    SetToIdentity(value.value);
    }




#ifdef USE_NAMESPACE
}
#endif



#endif
