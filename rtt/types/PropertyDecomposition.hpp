#ifndef ORO_PROPERTYDECOMPOSITION_HPP_
#define ORO_PROPERTYDECOMPOSITION_HPP_

#include "../base/PropertyBase.hpp"
#include "../PropertyBag.hpp"

namespace RTT {
    namespace types {

        /**
         * Uses the type decomposition to decompose a property
         * into a property bag that refers to all its parts.
         * Each modification of a part in the \a targetbag will modify \a source
         * too.
         * This function can only work if every part of the source
         * is known by the RTT type system.
         * @param source Contains a C++ type to be decomposed into a hierarchy of properties.
         * @param targetbag The bag in which to place the result.
         * @return True on success, false otherwise.
         */
        bool RTT_API propertyDecomposition( base::PropertyBase* source, PropertyBag& targetbag );

        /**
         * Identical to propertyDecomposition, but takes a DataSourceBase as source.
         * @param source Contains a C++ type to be decomposed into a hierarchy of properties.
         * @param targetbag The bag in which to place the result.
         * @return True on success, false otherwise.
         */
        bool RTT_API typeDecomposition( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag);
    }
}

#endif /* ORO_PROPERTYDECOMPOSITION_HPP_ */
