#ifndef RTT_TYPEINFO_GENERATOR_HPP
#define RTT_TYPEINFO_GENERATOR_HPP

#include <string>
#include "rtt-types-fwd.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * All generator classes inherit from this object
         * in order to allow them to be added to the
         * TypeInfoRepository. This class is solely
         * meant for installing TypeInfo objects, so a helper
         * class to the type system which has no function
         * once the type registration is done.
         */
        class TypeInfoGenerator
        {
        public:
            virtual ~TypeInfoGenerator() {}

            /**
             * Return unique the type name.
             */
            virtual const std::string& getTypeName() const = 0;
            
            /**
             * Installs the type info object in the global data source type info handler.
             * This will be called by the TypeInfoRepository, in order to register this
             * type's factories into the TypeInfo object.
             */
            virtual bool installTypeInfoObject(TypeInfo* ti) = 0;

            /**
             * Returns the TypeInfo object of this type, or null
             * if none exists yet.
             */
            virtual TypeInfo* getTypeInfoObject() const = 0;
        };
    }
}

#endif
