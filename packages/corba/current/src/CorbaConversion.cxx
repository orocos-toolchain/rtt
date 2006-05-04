#include "corba/CorbaConversion.hpp"
#include "corelib/PropertyBag.hpp"
#include "corba/AttributesC.h"
#include "corba/AttributesI.h"
#include "corba/CORBAExpression.hpp"


namespace ORO_CoreLib
{
    using namespace std;

    bool AnyConversion<PropertyBag>::update(const CORBA::Any& any, StdType& _value) {
        //Logger::In in("AnyConversion");
        Orocos::AttributeInterface_ptr attrs;
        // non deep copy:
        if ( any >>= attrs ) {
            Logger::log() << Logger::Debug << "Populating PropertyBag with AttributeInterface Properties." <<Logger::endl;
            Orocos::AttributeInterface::PropertyNames_var props = attrs->getPropertyList();

            for (size_t i=0; i != props->length(); ++i) {
                if ( _value.find( std::string(props[i].name.in()) ) )
                    continue; // previously added.
                Logger::log() << Logger::Debug << "  Adding "<< string(props[i].name.in() ) <<Logger::endl;
                Orocos::Expression_var expr = attrs->getProperty( props[i].name.in() );
                if ( CORBA::is_nil( expr ) ) {
                    Logger::log() <<Logger::Error <<"Property "<< std::string(props[i].name.in()) << " present in getPropertyList() but not accessible."<<Logger::endl;
                    continue; 
                }
#if 0 // This code may trigger endless recurse if server has recursive prop bags.
      // By using Property<PropertyBag>::narrow( ... ) this is no longer needed.
                // See if it is a PropertyBag:
                CORBA::Any_var any = expr->get();
                PropertyBag bag;
                if ( AnyConversion<PropertyBag>::update( *any, bag ) ) {
                    Property<PropertyBag>* pbag = new Property<PropertyBag>( string(props[i].name.in()), string(props[i].description.in()), bag);
                    _value.add( pbag );
                    continue;
                }
#endif
                Orocos::AssignableExpression_var as_expr = Orocos::AssignableExpression::_narrow( expr.in() );
                if ( CORBA::is_nil( as_expr ) ) {
                    Logger::log() <<Logger::Error <<"Property "<< std::string(props[i].name.in()) << " was not writable !"<<Logger::endl;
                } else
                    _value.add( new Property<CORBA::Any_ptr>( std::string(props[i].name.in()), std::string(props[i].description.in()), new ORO_Corba::CORBAAssignableExpression<Property<CORBA::Any_ptr>::DataSourceType>( as_expr.in() ) ) );
            }
            return true;
        }
        Logger::log() << Logger::Error << "Failed to populate PropertyBag with AttributeInterface Properties." <<Logger::endl;
        return false;
    }

    CORBA::Any_ptr AnyConversion<PropertyBag>::createAny( StdType t ) {
        Logger::log() << Logger::Debug << "Creating PropertyBag Server." <<Logger::endl;
        Orocos_AttributeInterface_i* attrs = new Orocos_AttributeInterface_i( new PropertyBag(t) );
        Orocos::AttributeInterface_ptr server = attrs->_this();
        CORBA::Any_ptr any = new CORBA::Any();
        *any <<= server;
        return any;
    }


};
