/** 
 * \file 
 * \author Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 * \version 
 *      ORO_Geometry V0.2
 *   
 * \par history
 *   - changed layout of the comments to accomodate doxygen
 */
#ifndef ERROR_STACK_H
#define ERROR_STACK_H

#include "utility.h"

#include "utility_io.h"
#include <string>


//#ifdef USE_NAMESPACE
namespace ORO_Geometry {
  //#endif

/*
 * \todo
 *    IOTrace-routines store in static memory, should be in thread-local memory.
 * pushes a description of the current routine on the IO-stack trace
 */
void IOTrace(const std::string& description);

//! pops a description of the IO-stack
void IOTracePop();


//! outputs the IO-stack to a stream to provide a better errormessage.
#ifdef WANT_STD_IOSTREAM
    void IOTraceOutput(std::ostream& os);
#else
    void IOTraceOutput(ostream& os);
#endif

//! outputs one element of the IO-stack to the buffer (maximally size chars)
//! returns empty string if no elements on the stack.
void IOTracePopStr(char* buffer,int size);


#ifdef USE_NAMESPACE
}
#endif

#endif

