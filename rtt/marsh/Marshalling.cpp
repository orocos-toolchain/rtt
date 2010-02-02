#include "Marshalling.hpp"

namespace RTT
{
    Marshalling::Marshalling(TaskContext* owner)
      : interface::ServiceRequester("marshalling",owner),
        loadProperties("loadProperties"),
        readProperties("readProperties"),
        updateProperties("updateProperties"),
        readProperty("readProperty"),
        writeProperties("writeProperties"),
        updateFile("updateFile"),
        writeProperty("writeProperty")
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
