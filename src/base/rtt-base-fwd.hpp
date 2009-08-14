#ifndef ORO_RTT_base_FWD_HPP
#define ORO_RTT_base_FWD_HPP

namespace RTT {
    namespace base {
        class ActionInterface;
        class ActivityInterface;
        class AttributeBase;
        class BufferBase;
        class ChannelElementBase;
        class DataSourceBase;
        class DispatchInterface;
        class EventBase;
        class InputPortInterface;
        class OutputPortInterface;
        class PortID;
        class PortInterface;
        class ProgramInterface;
        class PropertyBagVisitor;
        class PropertyBase;
        class PropertyIntrospection;
        class RunnableInterface;
        class TaskCore;
        struct BlockingPolicy;
        struct DataBuf;
        struct NonBlockingPolicy;
        struct Status;
        template <class T>
        class DataObjectInterface;
        template< class T, class ReadPolicy = NonBlockingPolicy, class WritePolicy = NonBlockingPolicy >
        class BufferLockFree;
        template<class F>
        struct CommandBase;
        template<class F>
        struct MethodBase;
        template<class T, class ReadPolicy = NonBlockingPolicy, class WritePolicy = NonBlockingPolicy>
        class BufferLocked;
        template<class T>
        class BufferInterface;
        template<class T>
        class DataObjectLockFree;
        template<class T>
        class DataObjectLocked;
        template<class T>
        class ReadInterface;
        template<class T>
        class WriteInterface;
        template<typename T>
        class ChannelElement;
    }
}
#endif
