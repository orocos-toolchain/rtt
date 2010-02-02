#include "Marshalling.hpp"

namespace RTT
{
    Marshalling::Marshalling(TaskContext* owner)
      : interface::ServiceRequester("marshalling",owner)
    {
        addMethod(loadProperties);
        addMethod(readProperties);
        addMethod(updateProperties);
        addMethod(readProperty);
        addMethod(writeProperties);
        addMethod(updateFile);
        addMethod(writeProperty);
    }

    Marshalling::~Marshalling() {}
}
