#ifndef ORO_PROPERTYCOMPOSITION_HPP_
#define ORO_PROPERTYCOMPOSITION_HPP_

#include "../base/PropertyBase.hpp"
#include "../PropertyBag.hpp"

namespace RTT {
    namespace types {

        /**
         * Uses the type composition to compose all typed properties
         * from a property bag.
         *
         * For each PropertyBag found with a type not equal to 'PropertyBag', it
         * will use the type's TypeInfo::composeType function and return
         * that in target if it generates a Property.
         *
         * @param sourcebag Contains a PropertyBag to be composed into a known type.
         * @param target The target type to compose the source bag into.
         * @return true if all typed PropertyBags in sourcebag could be composed to a user type,
         * false if one or more compositions failed.
         */
        bool RTT_API composePropertyBag( PropertyBag const& sourcebag, PropertyBag& target );

        /**
         * Inverse of RTT::types::composePropertyBag().
         * This function effectively replaces the property introspection mechanism for
         * decomposition.
         * @param sourcebag The bag from which to compose the type.
         * @param target Contains a C++ type to be composed into a hierarchy of properties.
         * @return True on success, false otherwise.
         * @see RTT::types::propertyComposition
         */
        bool RTT_API decomposePropertyBag( PropertyBag const& sourcebag, PropertyBag&  target);
    }
}

#endif /* ORO_PROPERTYCOMPOSITION_HPP_ */
