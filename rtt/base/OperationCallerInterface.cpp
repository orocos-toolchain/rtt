#include "OperationCallerInterface.hpp"
#include "../internal/GlobalEngine.hpp"

using namespace RTT;
using namespace base;
using namespace internal;

OperationCallerInterface::OperationCallerInterface()
    : myengine(0), caller(0), ownerEngine(0), met(OwnThread)
{}

OperationCallerInterface::OperationCallerInterface(OperationCallerInterface const& orig)
    : myengine(orig.myengine), caller(orig.caller), ownerEngine(orig.ownerEngine), met(orig.met)
{}

OperationCallerInterface::~OperationCallerInterface()
{
}

void OperationCallerInterface::setOwner(ExecutionEngine* ee) {
    ownerEngine = ee;
}

void OperationCallerInterface::setExecutor(ExecutionEngine* ee) {
    if (met == OwnThread && ee != 0 )
        myengine = ee;
    else
        myengine = GlobalEngine::Instance();
}

void OperationCallerInterface::setCaller(ExecutionEngine* ee) {
    if (ee)
        caller = ee;
    else
        caller = GlobalEngine::Instance();
}

bool OperationCallerInterface::setThread(ExecutionThread et, ExecutionEngine* executor) {
    if ( !myengine && !caller && !ownerEngine )
        return false; // detect uninitialized object. This is actually a hack for RemoteOperationCaller.
    met = et;
    setExecutor(executor);
    return true;
}

// report an error if an exception was thrown while calling exec()
void OperationCallerInterface::reportError() {
    // This localOperation was added to a TaskContext or to a Service owned by a TaskContext
    if (this->ownerEngine != 0)
        this->ownerEngine->setExceptionTask();
    // This operation is called through OperationCaller directly
    else if (this->met == OwnThread)
        this->myengine->setExceptionTask();
}

