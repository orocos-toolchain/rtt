#ifndef ORO_CORELIB_BUILD_TYPE_HPP
#define ORO_CORELIB_BUILD_TYPE_HPP

#include "Types.hpp"
#include "DataSourceAdaptor.hpp"
#include "DataSources.hpp"

namespace ORO_CoreLib
{
    namespace detail 
    {
        /**
         * A helper class to build a value of type T.
         */
        template<class T>
        struct BuildType
        {
            static typename AssignableDataSource<T>::shared_ptr Value(const T& value = T()) {
                typename AssignableDataSource<T>::shared_ptr _value;
#ifndef ORO_EMBEDDED
                assert( detail::DataSourceTypeInfo<T>::getTypeInfo() );
                DataSourceBase::shared_ptr dsb = detail::DataSourceTypeInfo<T>::getTypeInfo()->buildValue();
                if (dsb)
                    // First see if it is a predefined type (not unknown) and if so, build that one.
                    _value = AdaptAssignableDataSource<T>()(dsb);
                if ( _value ) {
                    _value->set( value );
                } else {
                    // this type is unknown, build a default one.
                    _value = new ValueDataSource<T>(value);
                }
#else
                _value = new ValueDataSource<T>(value);
#endif
                return _value;
            }
        };
    }
}

#endif
