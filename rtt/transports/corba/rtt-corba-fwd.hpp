#ifndef ORO_RTT_corba_FWD_HPP
#define ORO_RTT_corba_FWD_HPP

namespace RTT {
    namespace corba {
        class CorbaDispatcher;
        class CorbaTypeTransporter;
        class RemoteInputPort;
        class RemoteOutputPort;
        struct ApplicationServer;
        struct IllegalServer;
        struct NonExistingDataSource;
        struct RemoteConnID;
        struct UpdatedCommand;
        template<class T>
        class CORBAAssignableExpression;
        template<class T>
        class CORBAExpression;
        template<class T>
        class CorbaTemplateProtocol;
        template<class T>
        class DataSourceProxy;
        template<class T>
        class ValueDataSourceProxy;
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
