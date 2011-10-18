#ifndef ORO_STD_PRIMITIVE_TYPE_INFO_HPP
#define ORO_STD_PRIMITIVE_TYPE_INFO_HPP

#include "../rtt-config.h"
#include "../types/Types.hpp"
#include "../types/PrimitiveTypeInfo.hpp"

namespace RTT
{
    namespace types {

        /**
         * Type information for standard C types or structs of which
         * no introspection is required and \b NOT used on data ports. 
         * No boost::serialization function
         * is required to use this class.
         *
         * @see StructTypeInfo
         */
        template<class T>
        struct StdPrimitiveTypeInfo
        : public PrimitiveTypeInfo<T,true>
        {
            StdPrimitiveTypeInfo(const char* type)
            : PrimitiveTypeInfo<T,true>(type)
              {}

            virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
                // First, try a plain update.
                if ( result->update( source.get() ) )
                    return true;
                return false;
            }

            /**
             * A primitive type is decomposed into itself.
             */
            virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const {
                return source;
            }
        };
    }
}

#endif
