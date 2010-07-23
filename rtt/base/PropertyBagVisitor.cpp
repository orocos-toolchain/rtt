#include "PropertyBagVisitor.hpp"
#include "../types/PropertyDecomposition.hpp"
#include "../types/Types.hpp"
#include "../Property.hpp"
#include "../PropertyBag.hpp"
#include "../Logger.hpp"

namespace RTT {
    using namespace detail;

    bool PropertyBagVisitor::introspectAndDecompose(PropertyBase* v)
    {
        Property<PropertyBag> res(v->getName(), v->getDescription() );
        if ( types::propertyDecomposition(v, res.value() ) ) {
            this->introspect( res );
            deletePropertyBag( res.value() );
            return true;
        }else {
            DataSourceBase::shared_ptr dsb = v->getTypeInfo()->convertType( v->getDataSource() );
            // convertible ?
            if ( dsb ) {
                base::PropertyBase* p = dsb->getTypeInfo()->buildProperty(v->getName(), v->getDescription(), dsb);
                this->introspect( p );
                delete p;
                return true;
            } else {
                log(Warning) << "Property " << v->getType() << " "<< v->getName()<< "'s type is not known and not convertible. Dropping it." << endlog();
                return false;
            }
        }
        return false;
    }

}
