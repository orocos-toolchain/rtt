/*****************************************************************************
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
 *		$Id: integrator.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


/* Integrates ODE's with Runga Kutta - adaptive time step algorithme */
#include <integrator/integrator.h>

#ifdef USE_NAMESPACE
namespace LRL {
#endif


double Integrator_Base::SAFETY=0.9;
double Integrator_Base::PGROW=-0.2;
double Integrator_Base::PSHRNK=-0.25;
double Integrator_Base::ERRCON=1.89e-4;

double Integrator_Base::a2=0.2;
double Integrator_Base::a3=0.3;
double Integrator_Base::a4=0.6;
double Integrator_Base::a5=1.0;
double Integrator_Base::a6=0.875;
double Integrator_Base::b21=0.2;
double Integrator_Base::b31=3.0/40.0;
double Integrator_Base::b32=9.0/40.0;
double Integrator_Base::b41=0.3;	
double Integrator_Base::b42 = -0.9;		
double Integrator_Base::b43=1.2;
double Integrator_Base::b51 = -11.0/54.0;
double Integrator_Base::b52=2.5;
double Integrator_Base::b53 = -70.0/27.0;
double Integrator_Base::b54=35.0/27.0;
double Integrator_Base::b61=1631.0/55296.0;
double Integrator_Base::b62=175.0/512.0;
double Integrator_Base::b63=575.0/13824.0;
double Integrator_Base::b64=44275.0/110592.0;
double Integrator_Base::b65=253.0/4096.0;
double Integrator_Base::c1=37.0/378.0;
double Integrator_Base::c3=250.0/621.0;	
double Integrator_Base::c4=125.0/594.0;	
double Integrator_Base::c6=512.0/1771.0;
double Integrator_Base::dc5 = -277.00/14336.0;
double Integrator_Base::dc1=c1-2825.0/27648.0;
double Integrator_Base::dc3=c3-18575.0/48384.0;
double Integrator_Base::dc4=c4-13525.0/55296.0;
double Integrator_Base::dc6=c6-0.25;

#ifdef USE_NAMESPACE
}
#endif
