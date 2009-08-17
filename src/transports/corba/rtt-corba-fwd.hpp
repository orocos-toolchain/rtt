#ifndef ORO_RTT_corba_FWD_HPP
#define ORO_RTT_corba_FWD_HPP

namespace RTT {
    namespace corba {
        class CorbaTypeTransporter;
        class RemoteInputPort;
        class RemoteOutputPort;
        struct ApplicationServer;
        struct IllegalServer;
        struct UpdatedCommand;
        template<class T>
        class CORBAAssignableExpression;
        template<class T>
        class CORBAExpression;
        template<class T>
        class CorbaTemplateProtocol;
        template<class T>
        struct CreateConstantHelper;
        template<class Type, class _CorbaType>
        struct AnyConversionHelper;
        template<class Type>
        struct AnyConversion;
        template<typename PortClass>
        class RemotePort;
        template<typename T>
        class RemoteChannelElement;
    }
    namespace detail {
        using namespace corba;
    }
}
#endif
