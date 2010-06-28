
#include "../rtt-config.h"
#include "../types/Types.hpp"
#include "../types/TemplateTypeInfo.hpp"

namespace RTT
{
    namespace types {

        /**
         * Type information for standard C types or structs of which
         * no introspection is required. No boost::serialization function
         * is required to use this class.
         *
         * @see StructTypeInfo
         */
        template<class T>
        struct StdTypeInfo
        : public TemplateTypeInfo<T,true>
        {
            StdTypeInfo(const char* type)
            : TemplateTypeInfo<T,true>(type)
              {}
            virtual bool decomposeType( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
                return false;
            }

            virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
                // First, try a plain update.
                if ( result->update( source.get() ) )
                    return true;
                return false;
            }
        };
    }
}
