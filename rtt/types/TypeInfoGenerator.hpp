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
             * Return the type name for which this generator
             * generates type info features. This name will be
             * aliased by the TypeInfo object.
             */
            virtual const std::string& getTypeName() const = 0;
            
            /**
             * Installs the type info object in the global data source type info handler
             * and adds any additional features to the type info object.
             * This method will be called by the TypeInfoRepository, in order to register this
             * type's factories into the TypeInfo object.
             * @param ti A valid TypeInfo object into which new features
             * may be installed
             * @return true if this object may be deleted, false if not.
             * @post When true is returned, this instance is still valid and
             * the caller (TypeInfoRepository) will delete it. When false is returned,
             * the validity is undefined and the instance will not be used
             * anymore by the caller.
             */
            virtual bool installTypeInfoObject(TypeInfo* ti) = 0;

            /**
             * Returns the TypeInfo object of this type, or null
             * if none exists yet.
             * @return All generators should return here TypeInfoRepository::Instance()->getTypeInfo<T>();
             */
            virtual TypeInfo* getTypeInfoObject() const = 0;
        };
    }
}

#endif
