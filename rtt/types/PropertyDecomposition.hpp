#ifndef ORO_PROPERTYDECOMPOSITION_HPP_
#define ORO_PROPERTYDECOMPOSITION_HPP_

#include "../base/PropertyBase.hpp"
#include "../PropertyBag.hpp"

namespace RTT {
    namespace types {

        /**
         * Uses the type decomposition to decompose a property
         * into a property bag that refers to all its parts.
         * Each modification of a part in the bag will modify the
         * original too.
         * This function can only work if every part of the source
         * is known by the RTT type system.
         */
        bool propertyDecomposition( base::PropertyBase* source, PropertyBag& targetbag );

    }
}

#endif /* ORO_PROPERTYDECOMPOSITION_HPP_ */
