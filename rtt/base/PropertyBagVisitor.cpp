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
        }else if ( Types()->type("int")  && Types()->type("int") != v->getTypeInfo() ) {
            DataSourceBase::shared_ptr dsb = Types()->type("int")->convert( v->getDataSource() );
            // convertible to int ?
            if ( dsb != v->getDataSource() ) {
                DataSource<int>::shared_ptr ds = DataSource<int>::narrow( dsb.get() );
                assert( ds );
                Property<int> pint(v->getName(), v->getDescription(), ds->get() );
                this->introspect( &pint );
                return true;
            } else {
                log(Warning) << "Property " << dsb->getTypeName() << " "<< v->getName()<< "'s type is not known and not convertible to int. Dropping it." << endlog();
                return false;
            }
        }
        return false;
    }

}
