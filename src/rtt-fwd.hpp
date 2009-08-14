#ifndef ORO_RTT_FWD_HPP
#define ORO_RTT_FWD_HPP

#include "os/rtt-os-fwd.hpp"
#include "base/rtt-base-fwd.hpp"
#include "internal/rtt-internal-fwd.hpp"
#include "interface/rtt-interface-fwd.hpp"
#include "plugin/rtt-plugin-fwd.hpp"
#include "types/rtt-types-fwd.hpp"

namespace RTT
{

    class CleanupHandle;
    class Event;
    class ExecutionEngine;
    class Handle;
    class Logger;
    class PropertyBag;
    class ScopedHandle;
    class TaskContext;
    template<class CommandT>
    class Command;
    template<class FunctionT>
    class Method;
    template<typename T>
    class Alias;
    template<typename T>
    class Attribute;
    template<typename T>
    class Constant;
    template<typename T>
    class InputPort;
    template<typename T>
    class OutputPort;
    template<typename T>
    class Property;

    namespace base {}
    namespace internal {}
    namespace interface {}
    namespace extras {}
    namespace marsh {}
    namespace os {}
    namespace scripting {}
    namespace types {}
    namespace plugin {}

    /**
     * Implementation specific classes, not intended for
     * users.
     */
    namespace detail {
        using namespace base;
        using namespace internal;
        using namespace interface;
        using namespace extras;
        using namespace marsh;
        using namespace os;
        using namespace scripting;
        using namespace types;
        using namespace plugin;
    }

}
#endif
