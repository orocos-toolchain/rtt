/*****************************************************************************
 * \file  
 * 		The classes in this file belong to an implementation of an adaptive timestep algorithm 
 * 		for integrating ODE's with Runga Kutta method.
 * 		The function to be integrated can be any type, as long as
 * 		it has assignment, scalar multiplicatio, and addition defined
 * 		(e.g. valarray or Vector,Twist,Wrench out of frames.h) 
 *		 
 *  \author 
 *  	Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *		LRL V0.2
 *
 *	\par History
 *		- $log$ 
 *
 *	\par Release
 *		$Id: integrator.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef INTEGRATOR_H
#define INTEGRATOR_H
#include <math.h>
#include <utilities/utility.h>
#include <utilities/error.h>
#ifdef USE_NAMESPACE
namespace LRL {
#endif


/**
 * Function is an abstract class to encapsulated ODE to be integrated.  This
 * function can have an arbitrary Range type R, as long as this type has 
 * scalar multiplication, addition operators, assignment operators, and a
 * max_abs(..) function that takes the maximum of the absolute value of the elements
 * in the range type (e.g. a Vector, ColumnVector, Twist, Wrench, STL-valarray,...)
 * \par Type
 * Abstract base class
*/
template <class R>
class Function {
public:
	typedef R Range;
public:
	Function() {}

	/** 
	 * Called to evaluate the ODE (ordinary differential equation)
	 * @param x the variable over which integration takes place.
	 * @param y a Range-variable. 
	 * @return dy_dx the derivative of y with respect to x.
	 */
	virtual Range operator()(double x,const Range& y)  = 0;
	
	virtual ~Function() {}
};
 
/**
 * Contains the part that is common to
 * all template instantiations of Adaptive_Integrator<R>.
 * Contains mostly numerical constants for the Runga Kutta integration
 */ 
class Integrator_Base 

{
protected:
	static double SAFETY,PGROW,PSHRNK,ERRCON;
	static double 
		a2,	a3,	a4,	a5,	a6,	
		b21,b31,b32,	
		b41,b42,b43,
		b51,b52,b53,b54,
		b61,b62,b63,b64,b65,
		c1,c3,c4,c6,
		dc5,dc1,dc3,dc4,dc6;
	
public:
};


/**
 * Adaptive_Integrator is a class for adaptive integration with
 * a 5th order Runga Kuta method. 
 * @see Book : Numerical Recipes
 */
template <class R>
class Adaptive_Integrator : public Integrator_Base

{
	//! Range defines the type of the Range of the function.  It is always a good
	//! idea to typedef template parameters.  It allows future classes to reason with
	//! the types.
	typedef R Range;                
	
	//! Baseclass of function to be integrated.
	typedef Function<Range> Fun;
	
	//! Reference to function to be integrated.
	Fun& F;
	
	
	double x;
	Range y;
	Range dydx;

	double eps,h1,hmin;
	int maxstep;

	Range ak2,ak3,ak4,ak5,ak6,yerr,yout;
		//!< to avoid allocation when Range contains heap-allocated components
	double h;
public:
	int nok,nbad;

	/** 
	 * Constructor an ODE-integrator for a specifique ODE 
	 * and with specifique integration parameters.
	 * @param _F      the ODE-function dy/dx=f(x,y) to be integrated.
	 * @param eps     the desired accuracy for the integration
	 * @param h1      the initial stepsize
	 * @param hmin    the minimal stepsize 
	 * @param maxstep the maximum number of steps.
	 */
	Adaptive_Integrator(Fun& F_,
						double eps_=1E-6,
						double h1_=1.0,
						double hmin_=0.0,
						int maxstep_=10000) :
		F(F_),eps(eps_),
		nok(0),nbad(0),
		h1(h1_),hmin(hmin_),maxstep(maxstep_)
		{}

	/**
	 * Fifth-order Runge-Kutta step with monitoring of local truncation error 
	 * to ensure accuracy and adjust stepsize. Uses
	 * the independent variable x,the dependent variable vector y.
	 * @param htry input. the stepsize to be attempted
	 * @param hdid output. the stepsize that was actually accomplished
	 * @param hnext output. the estimated next stepsize.
	 */
	void rkqs(double htry, double &hdid,double &hnext);

	/**
	 * Runge-Kutta driver with adaptive stepsize control. 
	 * Variables for integration are specified by constructor. 
	 * \par Side effects
	 * Side effects on members of this object that are relevant to the user :
	 * - The integration parameters and function given to the constructor
	 * - nok and nbad are the number of good and bad (but retried and fixed) steps taken by
	 *   this routine.
	 * @param ystart integrate starting with this state.  is replaced by 
	 *               the state at the end of the integration interval. 
	 * @param x1 integrate starting with this value for the independent variable (e.g. time).
	 * @param x2 integrate until this value for the independent variable (e.g. time)
	 */
	void odeint(Range& ystart, double x1, double x2);
};


template <class R>
inline void Adaptive_Integrator<R>::odeint(Range& ystart, double x1, double x2)
	{
		int nstp;
		double hnext,hdid,h,sh;

		x=x1;
		sh = (x2-x1 > 0)?1:-1;
		h=sh*fabs(h1);
		nok = nbad = 0;
		y=ystart;
		for (nstp=1;nstp<=maxstep;nstp++) { // Take at most maxstep steps.
			dydx=F(x,y);
			if (sh*(x+h-x2)>0) {h=x2-x;}
			rkqs(h,hdid,hnext);
			if (hdid == h) 
				++nok; 
			else 
				++nbad;
			if ((x-x2)*sh >= 0.0) { 
				// Are we done?
				ystart=y;
				return; //Normal exit.
			}
			if (fabs(hnext) <= hmin) 
				throw Error_Stepsize_To_Small();
			h=hnext;
		}
		throw Error_To_Many_Steps();
	}


	
template <class R>
inline void Adaptive_Integrator<R>::rkqs(double htry, double &hdid,double &hnext)
	{
	double errmax,htemp,xnew;

	h=htry; //Set stepsize to the initial trial value.
	for (;;) {
		// *** Take a step ***
		ak2=F(x+a2*h,y+b21*h*dydx); // Second step.
		ak3=F(x+a3*h,y+h*(b31*dydx+b32*ak2)); // Third step.
		ak4=F(x+a4*h,y+h*(b41*dydx+b42*ak2+b43*ak3)); // Fourth step.
		ak5=F(x+a5*h,y+h*(b51*dydx+b52*ak2+b53*ak3+b54*ak4)); // Fifth step.
		ak6=F(x+a6*h,y+h*(b61*dydx+b62*ak2+b63*ak3+b64*ak4+b65*ak5)); // Sixth step.
		yout=y+h*(c1*dydx+c3*ak3+c4*ak4+c6*ak6);
		yerr=h*(dc1*dydx+dc3*ak3+dc4*ak4+dc5*ak5+dc6*ak6);

		errmax=max_abs(yerr)/eps;
		if (errmax <= 1.0) 
			break; // Step succeeded. Compute size of next step.
		htemp=SAFETY*h*::pow(errmax,PSHRNK);
		//Truncation error too large, reduce stepsize.
		h=(h >= 0.0 ? max(htemp,0.1*h) : min(htemp,0.1*h));
		//No more than a factor of 10.
		xnew=x+h;
		if (xnew == x) throw Error_Stepsize_Underflow();
		}
	if (errmax > ERRCON) 
		hnext=SAFETY*h*::pow(errmax,PGROW);
	else 
		hnext=5.0*h; //No more than a factor of 5 increase.
	hdid=h;
	x += hdid;
	y=yout;
	}


#ifdef USE_NAMESPACE
}
#endif

#endif