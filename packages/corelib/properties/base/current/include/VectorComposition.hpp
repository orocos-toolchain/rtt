#ifndef VECTOR_COMPOSITION_HPP
#define VECTOR_COMPOSITION_HPP

#include "PropertyIntrospection.hpp"
#include "Property.hpp"
#include "PropertyBag.hpp"
#include <vector>

namespace ORO_CoreLib
{

    /**
     * A decomposeProperty method for decomposing a Property< vector<double> >
     * into a PropertyBag with Property<double>'s.
     * The dimension of the vector must be less than 100 if you want the
     * Property<double>'s to have a different name.
     */
    void decomposeProperty(PropertyIntrospection *pi, const Property< std::vector<double> >& c);

    /**
     * A composeProperty method for composing a property of a vector<double>
     * The dimension of the vector must be less than 100.
     */
    void composeProperty(const PropertyBag& bag, Property<std::vector<double> >& result);

}; // namespace ORO_CoreLib


#endif
