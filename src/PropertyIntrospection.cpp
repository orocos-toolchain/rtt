#include "PropertyIntrospection.hpp"

namespace RTT{

    void PropertyIntrospection::introspect_T(PropertyBase* v)
    {
        Property<PropertyBag> res(v->getName(), v->getDescription() );
        if ( v->getTypeInfo()->decomposeType( v->getDataSource(), res.value() ) ) {
            this->introspect( res );
            deletePropertyBag( res.value() );
        }else
            Logger::log() << Logger::Warning<< "Could not decompose "<< v->getName() << Logger::endl;
        // drop.
    }

}
