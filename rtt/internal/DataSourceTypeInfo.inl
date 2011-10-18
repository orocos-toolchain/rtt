#ifndef DATASOURCE_TYPE_INFO_INL
#define DATASOURCE_TYPE_INFO_INL

#include "../types/TypeInfo.hpp"
#include "../types/TypeInfoRepository.hpp"

namespace RTT
{
    namespace internal {
        template< class T>
        const std::string& DataSourceTypeInfo<T>::getType() {
	    return getTypeInfo()->getTypeName();
	}

        template< class T>
        const std::string& DataSourceTypeInfo<T>::getTypeName() {
	    return getTypeInfo()->getTypeName();
	}

        template< class T>
        const std::string& DataSourceTypeInfo<T>::getQualifier() {
            return DataSourceTypeInfo<UnknownType>::getQualifier();
        }

        template< class T>
        const types::TypeInfo* DataSourceTypeInfo<T>::getTypeInfo() {
            types::TypeInfo* ret;
            ret = types::TypeInfoRepository::Instance()->getTypeInfo<T>();
            if (!ret)
                return DataSourceTypeInfo<UnknownType>::getTypeInfo();
            return ret;
        }

        template< class T>
        types::TypeInfo* DataSourceTypeInfo<T>::TypeInfoObject = 0;

		template< class T>
		const types::TypeInfo* DataSourceTypeInfo<T*>::getTypeInfo() {
            types::TypeInfo* ret;
            ret = types::TypeInfoRepository::Instance()->getTypeInfo<T*>();
            if (!ret)
                return DataSourceTypeInfo<UnknownType>::getTypeInfo();
            return ret;
		}

        template< class T>
        types::TypeInfo* DataSourceTypeInfo<T*>::TypeInfoObject = 0;

		template< class T>
		const types::TypeInfo* DataSourceTypeInfo< types::carray<T> >::getTypeInfo() {
			// return carray type info
            if (!TypeInfoObject) {
                TypeInfoObject = types::TypeInfoRepository::Instance()->getTypeInfo< types::carray<T> >();
                if (!TypeInfoObject)
                    return DataSourceTypeInfo<UnknownType>::getTypeInfo();
            }
            return TypeInfoObject;
		}

        template< class T>
        types::TypeInfo* DataSourceTypeInfo< types::carray<T> >::TypeInfoObject = 0;

    }

}

#endif
