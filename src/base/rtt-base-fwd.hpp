#ifndef ORO_RTT_base_FWD_HPP
#define ORO_RTT_base_FWD_HPP

namespace RTT {
    namespace base {
        class ActionInterface;
        class ActivityInterface;
        class AttributeBase;
        class BufferBase;
        class ChannelElementBase;
        class ConditionInterface;
        class DataSourceBase;
        class DispatchInterface;
        template<
            typename Signature
        >
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
        template< class T >
        class DataObject;
        template< class T, class ReadPolicy, class WritePolicy >
        class Buffer;
        template <class T>
        class DataObjectInterface;
        template< class T, class ReadPolicy , class WritePolicy >
        class BufferLockFree;
        template<class F>
        struct CommandBase;
        template<class F>
        struct MethodBase;
        template<class T, class ReadPolicy , class WritePolicy >
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
    namespace detail {
        using namespace base;
    }
}
#endif
