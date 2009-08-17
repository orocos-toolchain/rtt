#ifndef ORO_RTT_interface_FWD_HPP
#define ORO_RTT_interface_FWD_HPP

namespace RTT {
    namespace interface {
        class AttributeRepository;
        class CommandRepository;
        class DataFlowInterface;
        class EventService;
        class ExecutionAccess;
        class MarshallingAccess;
        class MethodRepository;
        class OperationInterface;
        class PropertyLoader;
    }
    namespace detail {
        using namespace interface;
    }
}
#endif
