#ifndef ORO_RTT_FWD_HPP
#define ORO_RTT_FWD_HPP

#include "rtt-detail-fwd.hpp"
#include "os/rtt-os-fwd.hpp"
#include "base/rtt-base-fwd.hpp"
#include "internal/rtt-internal-fwd.hpp"
#include "interface/rtt-interface-fwd.hpp"
#include "plugin/rtt-plugin-fwd.hpp"
#include "types/rtt-types-fwd.hpp"

namespace RTT
{

    class Activity;
    class CleanupHandle;
    class ExecutionEngine;
    class Handle;
    class Logger;
    class PropertyBag;
    class ScopedHandle;
    class TaskContext;
    template<typename T>
    class Alias;
    template<typename T>
    class Attribute;
    template<typename T>
    class Constant;
    template<typename CommandT>
    class Command;
    template<typename SignatureT>
    class Event;
    template<typename T>
    class InputPort;
    template<typename FunctionT>
    class Method;
    template<typename T>
    class OutputPort;
    template<typename T>
    class Property;

}
#endif
