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

bool OperationCallerInterface::isSend()
{
    if ( met == ClientThread ) return false;
    // OwnThread case:
    if ( myengine->isSelf() ) return false;
    return true; // all other OwnThread cases
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

