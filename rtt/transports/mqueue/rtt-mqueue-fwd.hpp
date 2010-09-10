#ifndef ORO_RTT_mqueue_FWD_HPP
#define ORO_RTT_mqueue_FWD_HPP

namespace RTT {
    namespace mqueue {
        class Dispatcher;
        template<class T>
        class MQSerializationProtocol;
        template<class T>
        class MQTemplateProtocol;
        template<typename T>
        class MQChannelElement;
    }
    namespace detail {
        using namespace mqueue;
    }
}
#endif
