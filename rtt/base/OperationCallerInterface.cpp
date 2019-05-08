/***************************************************************************
  tag: Peter Soetens Thu Nov 22 00:53:20 2012 +0100 OperationCallerInterface.cpp

                        OperationCallerInterface.cpp -  description
                           -------------------
    begin                : Thu Nov 22 2012
    copyright            : (C) 2012 Peter Soetens
    email                : peter@thesourceworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "OperationCallerInterface.hpp"
#include "../internal/GlobalEngine.hpp"

using namespace RTT;
using namespace base;
using namespace internal;

OperationCallerInterface::OperationCallerInterface()
    : myengine(0), caller(0), met(ClientThread)
{}

OperationCallerInterface::OperationCallerInterface(OperationCallerInterface const& orig)
    : myengine(orig.myengine), caller(orig.caller),  met(orig.met)
{}

OperationCallerInterface::~OperationCallerInterface()
{
}

void OperationCallerInterface::setOwner(ExecutionEngine* ee) {
    myengine = ee;
}

void OperationCallerInterface::setCaller(ExecutionEngine* ee) {
    caller = ee;
}

bool OperationCallerInterface::setThread(ExecutionThread et, ExecutionEngine* executor) {
    met = et;
    setOwner(executor);
    return true;
}

ExecutionEngine* OperationCallerInterface::getMessageProcessor() const 
{ 
    ExecutionEngine* ret = (met == OwnThread ? myengine : GlobalEngine::Instance()); 
    if (ret == 0 )
        return GlobalEngine::Instance(); // best-effort for Operations not tied to an EE
    return ret;
}


// report an error if an exception was thrown while calling exec()
void OperationCallerInterface::reportError() {
    // This localOperation was added to a TaskContext or to a Service owned by a TaskContext
    if (this->myengine != 0)
        this->myengine->setExceptionTask();
}

