/*****************************************************************************
 * \file  
 * This file contains the definition of the I/O operations on classes 
 * for counting the number of operations on a float.
 * 
 *       
 *  \author Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *  \version 
 *      ORO_Geometry V2
 *  \par History
 *      - $log$
 * 
 *  \par Release
 *      $Id: countingnumbers_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $   
 ****************************************************************************/
#ifndef COUNTINGNUMBERS_IO_H
#define COUNTINGNUMBERS_IO_H

#include <utilities/utility_io.h>
#include <algebra/countingnumbers.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

inline ostream& operator << (ostream& os,const Counting& v)
            {
            os << "Counting(" << v.value << ")";
            return os;
            }

#ifdef USE_NAMESPACE
}
#endif

#endif
