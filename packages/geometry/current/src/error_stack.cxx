/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  error_stack.cxx 

                        error_stack.cxx -  description
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
 *		$Id: error_stack.cpp,v 1.1.1.1.2.1 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/error_stack.h"
#include <stack>
#include <vector>
#include <string>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

// Trace of the call stack of the I/O routines to help user
// interprete error messages from I/O
typedef std::stack<std::string>  ErrorStack;

ErrorStack errorstack;
// should be in Thread Local Storage if this gets multithreaded one day...


void IOTrace(const std::string& description) {
    errorstack.push(description);   
}


void IOTracePop() {
    errorstack.pop();
}

void IOTraceOutput(std::ostream& os) {
    while (!errorstack.empty()) {
        os << errorstack.top().c_str() << std::endl;
        errorstack.pop();
    }
}


void IOTracePopStr(char* buffer,int size) {
    if (errorstack.empty()) {
        *buffer = 0;
        return;
    }
    strncpy(buffer,errorstack.top().c_str(),size);
    errorstack.pop();
}

#ifdef USE_NAMESPACE
}
#endif
