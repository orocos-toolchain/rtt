#ifndef ORO_RTT_types_FWD_HPP
#define ORO_RTT_types_FWD_HPP

namespace RTT {
    namespace types {
        class BinaryOp;
        class DotOp;
        class EmptyTypeInfo;
        class GlobalsRepository;
        class OperatorRepository;
        class RealTimeTypekitPlugin;
        class TransportPlugin;
        class TypekitPlugin;
        class TypekitRepository;
        class TypeInfo;
        class TypeInfoRepository;
        class TypeTransporter;
        class UnaryOp;
        struct MyClass;
        struct TypeBuilder;
        template <typename T, bool has_ostream>
        struct StdVectorTemplateTypeInfo;
        template< class T, int Range>
        struct RangeIndexChecker;
        template< class T>
        struct AlwaysAssignChecker;
        template< class T>
        struct ArrayIndexChecker;
        template< class T>
        struct MultiVectorIndexChecker;
        template< class T>
        struct SizeAssignChecker;
        template<class S>
        struct TemplateConstructor;
        template<class T>
        class TypeMarshaller;
        template<class T>
        struct BuildType;
        template<class T>
        struct StdTypeInfo;
        template<typename T, bool use_ostream>
        class TemplateTypeInfo;
        template<typename T, typename IndexType, typename SetType, typename IPred, typename APred, bool has_ostream>
        class TemplateContainerTypeInfo;
        template<typename T, typename IndexType, typename SetType, typename IPred, typename APred, bool has_ostream>
        class TemplateIndexTypeInfo;
        template<typename T>
        struct StdVectorBuilder;
        template<typename T>
        struct TypeInfoName;
        template<typename function>
        class BinaryOperator;
        template<typename function>
        class DotOperator;
        template<typename function>
        class UnaryOperator;
    }
    namespace detail {
        using namespace types;
    }
}
#endif
