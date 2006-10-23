/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  CorbaConversion.cxx 

                        CorbaConversion.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#include "rtt/corba/CorbaConversion.hpp"
#include "rtt/PropertyBag.hpp"
#include "rtt/corba/AttributesC.h"
#include "rtt/corba/AttributesI.h"
#include "rtt/corba/CORBAExpression.hpp"
#include "rtt/corba/ControlTaskProxy.hpp"


namespace RTT
{
    using namespace std;

    bool AnyConversion<PropertyBag>::update(const CORBA::Any& any, StdType& _value) {
        Logger::In in("AnyConversion<PropertyBag>");
        //Logger::In in("AnyConversion");
        Corba::AttributeInterface_ptr attrs;
        // non deep copy:
        if ( any >>= attrs ) {
            Logger::log() << Logger::Debug << "Populating PropertyBag with AttributeInterface Properties." <<Logger::endl;
            Corba::AttributeInterface::PropertyNames_var props = attrs->getPropertyList();

            for (size_t i=0; i != props->length(); ++i) {
                if ( _value.find( std::string(props[i].name.in()) ) )
                    continue; // previously added.
                Logger::log() << Logger::Debug << "  Adding "<< string(props[i].name.in() ) <<Logger::endl;
                Corba::Expression_var expr = attrs->getProperty( props[i].name.in() );
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
                Corba::AssignableExpression_var as_expr = Corba::AssignableExpression::_narrow( expr.in() );
                if ( CORBA::is_nil( as_expr ) ) {
                    Logger::log() <<Logger::Error <<"Property "<< std::string(props[i].name.in()) << " was not writable !"<<Logger::endl;
                } else {
                    CORBA::String_var tn = as_expr->getTypeName();
                    TypeInfo* ti = TypeInfoRepository::Instance()->type( tn.in() );
                    Logger::log() <<Logger::Info << "Looking up Property " << tn.in();
                    if ( ti ) {
                        _value.add( ti->buildProperty( props[i].name.in(), props[i].description.in(), 
                                                                            ti->buildCorbaProxy( as_expr.in() ) ) );
                        Logger::log() <<Logger::Info<<" found!"<<Logger::endl;
                    }
                    else {
                        _value.add( new Property<CORBA::Any_ptr>( string(props[i].name.in()), string(props[i].description.in()), new Corba::CORBAAssignableExpression<Property<CORBA::Any_ptr>::DataSourceType>( as_expr.in() ) ) );
                        Logger::log() <<Logger::Info<<" not found :-("<<Logger::endl;
                    }
                }
            }
            return true;
        }
        Logger::log() << Logger::Debug << "Failed to populate PropertyBag with AttributeInterface Properties." <<Logger::endl;
        return false;
    }

    CORBA::Any_ptr AnyConversion<PropertyBag>::createAny( StdType t ) {
        Logger::log() << Logger::Debug << "Creating PropertyBag Server." <<Logger::endl;
        Orocos_AttributeInterface_i* attrs = new Orocos_AttributeInterface_i( new PropertyBag(t), Corba::ControlTaskProxy::ProxyPOA() );
        Corba::AttributeInterface_ptr server = attrs->_this();
        CORBA::Any_ptr any = new CORBA::Any();
        *any <<= server;
        return any;
    }


};
