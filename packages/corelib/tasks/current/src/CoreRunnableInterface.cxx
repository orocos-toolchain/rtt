
#include "corelib/RunnableInterface.hpp"
#include "corelib/Logger.hpp"

namespace ORO_CoreLib
{

    RunnableInterface::~RunnableInterface() {
        if ( this->owner_task && this->owner_task->isRunning() ) {
            Logger::In in("~RunnableInterface()");
            Logger::log() << Logger::Critical
                    <<"Task still running, but RunnableInterface destroyed! Stop the task"
                    " before deleting this object. Crash may be imminent."<<Logger::endl;
        }
        if ( this->owner_task )
            this->owner_task->run(0);
    }
    
  RunnableInterface::RunnableInterface() : owner_task(0) {}

  TaskInterface* RunnableInterface::getTask() const { return owner_task; }

  void RunnableInterface::setTask( TaskInterface* task ) {
    owner_task = task;
  }
 
}
