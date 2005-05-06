#ifndef DATASOURCE_TYPE_INFO_HPP
#define DATASOURCE_TYPE_INFO_HPP

#include <string>


namespace ORO_CoreLib
{
    template<unsigned int S, class T>
    class MultiVector;
}

namespace ORO_Geometry
{
    class Frame;
    class Vector;
    class Rotation;
    class Twist;
    class Wrench;
}


namespace ORO_CoreLib
{

    namespace detail {
        struct ValueType {};

        template< class T>
        struct DataSourceTypeInfo;

        template<>
        struct DataSourceTypeInfo<ValueType> {
            static const std::string type;
            static const std::string qual;
            static const std::string& getType() { return type; }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T>
        struct DataSourceTypeInfo<const T&> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<const T&>::qual("const& ");

        template< class T>
        struct DataSourceTypeInfo<T&> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<T&>::qual("& ");

        template< class T>
        struct DataSourceTypeInfo<const T> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<const T>::qual("const ");

        template< class T>
        struct DataSourceTypeInfo<T*> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<T*>::qual("* ");

        template< class T>
        struct DataSourceTypeInfo<const T*> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<const T*>::qual("const* ");

        template<>
        struct DataSourceTypeInfo<void> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<bool> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<int> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<unsigned int> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<double> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<char> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        // Forward decls allow us to define these, even if the geometry package is not used.
        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Frame> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Vector> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Rotation> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Twist> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Wrench> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<std::string> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_CoreLib::MultiVector<6, double> > {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo< std::vector<double> > {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template< class T>
        struct DataSourceTypeInfo {
            static const std::string& getType() { return DataSourceTypeInfo<ValueType>::getType(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

    }

}
#endif
