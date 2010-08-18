#include "Marshalling.hpp"

namespace RTT
{
    Marshalling::Marshalling(TaskContext* owner)
      : ServiceRequester("marshalling",owner),
        loadProperties("loadProperties"),
        readProperties("readProperties"),
        updateProperties("updateProperties"),
        readProperty("readProperty"),
        writeProperties("writeProperties"),
        updateFile("updateFile"),
        writeProperty("writeProperty")
    {
        addOperationCaller(loadProperties);
        addOperationCaller(readProperties);
        addOperationCaller(updateProperties);
        addOperationCaller(readProperty);
        addOperationCaller(writeProperties);
        addOperationCaller(updateFile);
        addOperationCaller(writeProperty);
    }

    Marshalling::~Marshalling() {}
}
