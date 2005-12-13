
#include "os/RunnableInterface.hpp"
#include "os/ThreadInterface.hpp"

namespace ORO_OS
{
  RunnableInterface::RunnableInterface(ThreadInterface* t)
    : rthread(0)
  {
    if (t)
      t->run(this);
  }

  RunnableInterface::~RunnableInterface()
  {
  }

  void RunnableInterface::loop() { this->step(); }

  bool RunnableInterface::breakLoop() { return false; }

  void RunnableInterface::setThread(ThreadInterface* t)
  {
    rthread = t;
  }

  ThreadInterface* RunnableInterface::getThread() const
  {
    return rthread;
  }
}
