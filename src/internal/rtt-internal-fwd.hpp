#ifndef ORO_RTT_internal_FWD_HPP
#define ORO_RTT_internal_FWD_HPP

namespace RTT {
    namespace internal {
        class BlockingEventProcessor;
        class CommandC;
        class CommandFunction;
        class CommandProcessor;
        class CompletionProcessor;
        class ConditionFunction;
        class ConnFactory;
        class ConnPolicy;
        class ConnectionBase;
        class ConnectionC;
        class ConnectionManager;
        class DispatchAction;
        class EventC;
        class EventProcessor;
        class MethodC;
        class SignalBase;
        class TaskObject;
        struct ArgumentDescription;
        struct EventCallBack;
        struct EventCatcher;
        struct EventHookBase;
        struct GenerateDataSource;
        struct IntrusiveStorage;
        struct OnlyFirstCont;
        struct OnlyLastCont;
        struct UnknownType;
        template <typename function>
        class UnaryDataSource;
        template<
            typename Signature
        >
        class LocalEvent;
        template<
            typename Signature 
        >
        class SubscriberBase;
        template<
        typename Signature,
        typename SlotFunction>
        class Signal;
        template< class FunctionT>
        struct Functor;
        template< class T, class ReadPolicy, class WritePolicy>
        class AtomicQueue;
        template< class T, class ReadPolicy, class WritePolicy>
        class LockedQueue;
        template< class T, class ReadPolicy, class WritePolicy>
        class Queue;
        template< class T>
        class List;
        template< class T>
        class ListLockFree;
        template< class T>
        class ListLocked;
        template< class T>
        struct DataSourceTypeInfo;
        template< class TResult >
        struct AdaptAssignableDataSource;
        template< class TResult >
        struct AdaptDataSource;
        template<class A>
        struct DataSourceArgStorage;
        template<class CommandT,
                 class CommandF>
        class DataSourceArgsCommand;
        template<class CommandT>
        class LocalCommand;
        template<class CommandT>
        class LocalCommandImpl;
        template<class CommandT>
        class RemoteCommand;
        template<class CommandT>
        class RemoteCommandImpl;
        template<class DataType>
        struct DataSourceStorage;
        template<class DataType_>
        class SingleList;
        template<class DataType_>
        class SortedList;
        template<class EventT>
        class EventHookGenerator;
        template<class F, class BaseImpl>
        struct Invoker;
        template<class F>
        class ArgMember;
        template<class F>
        class UnMember;
        template<class F>
        class UnPointer;
        template<class F>
        struct FunctorDataSource;
        template<class F>
        struct InvokerBase;
        template<class F>
        struct MethodBinder;
        template<class From, class To>
        struct AssignableDataSourceAdaptor;
        template<class From, class To>
        struct DataSourceAdaptor;
        template<class FunctionT>
        class LocalMethodImpl;
        template<class FunctionT>
        struct LocalMethod;
        template<class MethodT>
        class RemoteMethod;
        template<class MethodT>
        class RemoteMethodImpl;
        template<class R>
        struct DataSourceResultStorage;
        template<class SignatureT, class FunctorT>
        class DataSourceArgsEvent;
        template<class SignatureT, class FunctorT>
        class DataSourceArgsMethod;
        template<class T, class Enable>
        struct DSWrap;
        template<class T>
        class CopyPropertyCommand;
        template<class T>
        class FixedSizeMemoryPool;
        template<class T>
        class MemoryPool;
        template<class T>
        class RefreshPropertyCommand;
        template<class T>
        class RefreshPropertyFromDSCommand;
        template<class T>
        class UpdatePropertyCommand;
        template<class T>
        struct AStore;
        template<class T>
        struct NA;
        template<class ToBind>
        struct BindStorage;
        template<typename BoundType>
        class UnboundDataSource;
        template<typename ComponentT, typename FunctorT, typename Arg1T>
        class FunctorDataSourceDS1;
        template<typename ComponentT, typename FunctorT>
        class FunctorDataSourceDS0;
        template<typename EventT>
        struct EventHook0;
        template<typename EventT>
        struct EventHook1;
        template<typename EventT>
        struct EventHook2;
        template<typename EventT>
        struct EventHook3;
        template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T, typename Arg6T>
        class FunctorDataSource6;
        template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T>
        class FunctorDataSource5;
        template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
        class FunctorDataSource4;
        template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
        class FunctorDataSource3;
        template<typename FunctorT, typename Arg1T, typename Arg2T>
        struct FunctorDataSource2;
        template<typename FunctorT, typename Arg1T>
        struct FunctorDataSource1;
        template<typename FunctorT>
        struct FunctorDataSource0;
        template<typename R, typename FunctorT>
        struct FunctionForwarder;
        template<typename ResultT, typename FunctorT, typename arg1_type>
        class FunctorFactoryPart1;
        template<typename ResultT, typename FunctorT, typename arg1_type>
        class OperationFactoryPart1;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart0;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart2;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart3;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart4;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart5;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart6;
        template<typename ResultT, typename FunctorT>
        class OperationFactoryPart0;
        template<typename ResultT, typename FunctorT>
        class OperationFactoryPart2;
        template<typename ResultT, typename FunctorT>
        class OperationFactoryPart3;
        template<typename ResultT, typename FunctorT>
        class OperationFactoryPart4;
        template<typename ResultT>
        class FunctorFactoryPart;
        template<typename ResultT>
        class OperationFactory;
        template<typename ResultT>
        class OperationFactoryPart;
        template<typename SignatureT, typename FunctorT>
        class CommandFunctor;
        template<typename SignatureT, typename FunctorT>
        class ConditionFunctor;
        template<typename T, typename APred, typename S>
        class AssignContainerCommand;
        template<typename T, typename Index, typename SetType, typename IPred, typename APred>
        class IndexedValueDataSource;
        template<typename T, typename Index, typename SetType, typename Pred>
        class AssignIndexCommand;
        template<typename T, typename S>
        class AssignCommand;
        template<typename T>
        class AssignableDataSource;
        template<typename T>
        class ChannelBufferElement;
        template<typename T>
        class ChannelDataElement;
        template<typename T>
        class ConnInputEndpoint;
        template<typename T>
        class ConnOutputEndpoint;
        template<typename T>
        class ConstantDataSource;
        template<typename T>
        class DataSource;
        template<typename T>
        class InputPortSource;
        template<typename T>
        class ReferenceDataSource;
        template<typename T>
        class ValueDataSource;
        template<typename function>
        class BinaryDataSource;
        template<typename function>
        class NArityDataSource;
        template<typename function>
        class SixaryDataSource;
        template<typename function>
        class TernaryDataSource;
    }
    namespace detail {
        using namespace internal;
    }
}
#endif
