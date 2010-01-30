/**
 * StateInterface.cpp
 *
 *  Created on: Jan 31, 2010
 *      Author: kaltan
 */

#include "StateInterface.hpp"

using namespace RTT;
using namespace RTT::detail;

StateInterface::~StateInterface()
{}


void StateInterface::loaded(ExecutionEngine* ee) {
    if ( this->getEntryProgram() )
        getEntryProgram()->loaded(ee);
    if ( this->getRunProgram() )
        getRunProgram()->loaded(ee);
    if ( this->getHandleProgram() )
        getHandleProgram()->loaded(ee);
    if ( this->getExitProgram() )
        getExitProgram()->loaded(ee);
}
