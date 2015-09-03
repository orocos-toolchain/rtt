#ifndef ORO_RTT_base_FWD_HPP
#define ORO_RTT_base_FWD_HPP

namespace RTT {
    namespace base {
        class ActionInterface;
        class ActivityInterface;
        class AttributeBase;
        class BufferBase;
        class ChannelElementBase;
        class MultipleInputsChannelElementBase;
        class MultipleOutputsChannelElementBase;
        class MultipleInputsMultipleOutputsChannelElementBase;
        class DataSourceBase;
        class DisposableInterface;
        class ExecutableInterface;
        class InputPortInterface;
        class OperationBase;
        class OutputPortInterface;
        class PortInterface;
        class PropertyBagVisitor;
        class PropertyBase;
        class PropertyIntrospection;
        class RunnableInterface;
        class TaskCore;
        struct DataBuf;
        struct OperationCallerBaseInvoker;
        template <class T>
        class DataObjectInterface;
        template< class T >
        class DataObject;
        template< class T>
        class Buffer;
        template< class T>
        class BufferLockFree;
        template<class F>
        struct OperationCallerBase;
        template<class T>
        class BufferInterface;
        template<class T>
        class BufferLocked;
        template<class T>
        class BufferUnSync;
        template<class T>
        class DataObjectLockFree;
        template<class T>
        class DataObjectLocked;
        template<class T>
        class DataObjectUnSync;
        template<typename T>
        class ChannelElement;
        template<typename T>
        class MultipleInputsChannelElement;
        template<typename T>
        class MultipleOutputsChannelElement;
        template<typename T>
        class MultipleInputsMultipleOutputsChannelElement;
    }
    namespace detail {
        using namespace base;
    }
}
#endif
