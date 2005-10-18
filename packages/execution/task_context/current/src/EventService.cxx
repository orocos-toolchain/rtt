#include "execution/EventService.hpp"

#include "execution/ExecutionEngine.hpp"
#include <corelib/CompletionProcessor.hpp>

using namespace ORO_CoreLib;

namespace ORO_Execution
{
    EventService EventService::Global(CompletionProcessor::Instance()->getEventProcessor());

    EventService::EventService( ExecutionEngine* ee ) : eeproc(ee), eproc(0) {}
    EventService::EventService( EventProcessor* ep ) : eeproc(0), eproc( ep ) {} // ep may be null.

    ORO_CoreLib::EventProcessor* EventService::getEventProcessor() { return eeproc ? eeproc->getEventProcessor()
                                                                         : (eproc ? eproc : CompletionProcessor::Instance()->getEventProcessor()); }

}
