
namespace RTT
{

    class Handle;
    class PropertyBag;
    template<class T>
    class Attribute;
    template<class T>
    class Constant;
    template<class T>
    class Alias;
    template<class T>
    class Property;
    template<class T>
    class Event;
    template<class T>
    class Method;
    template<class T>
    class Command;
    template<class T>
    class InputPort;
    template<class T>
    class OutputPort;
    class TaskContext;

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
