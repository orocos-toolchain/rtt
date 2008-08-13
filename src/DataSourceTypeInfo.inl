#ifndef DATASOURCE_TYPE_INFO_INL
#define DATASOURCE_TYPE_INFO_INL

#include "Types.hpp"

namespace RTT
{
    namespace detail {
        template< class T>
        const std::string& DataSourceTypeInfo<T>::getType() {
            if (!TypeInfoObject)
                return DataSourceTypeInfo<UnknownType>::getType();
            return TypeInfoObject->getTypeName();
        }

        template< class T>
        const std::string& DataSourceTypeInfo<T>::getQualifier() {
            return DataSourceTypeInfo<UnknownType>::getQualifier();
        }

        template< class T>
        const TypeInfo* DataSourceTypeInfo<T>::getTypeInfo() {
            if (!TypeInfoObject)
                return DataSourceTypeInfo<UnknownType>::getTypeInfo();
            return TypeInfoObject;
        }

        template< class T>
        TypeInfo* DataSourceTypeInfo<T>::TypeInfoObject = 0;

    }

}

#endif
