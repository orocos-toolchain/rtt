/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  CorbaConversion.cxx

                        CorbaConversion.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "CorbaConversion.hpp"
#include "../../PropertyBag.hpp"
#include "AttributesC.h"
#include "AttributesI.h"
#include "CORBAExpression.hpp"
#include "ControlTaskProxy.hpp"


namespace RTT {
    using namespace detail;
    using namespace std;

    bool AnyConversion<PropertyBag>::update(const CORBA::Any& any, StdType& _value) {
        Logger::In in("AnyConversion<PropertyBag>");
        //Logger::In in("AnyConversion");
        corba::CAttributeInterface_ptr attrs;
        // non deep copy:
        if ( any >>= attrs ) {
            Logger::log() << Logger::Debug << "Populating PropertyBag with CAttributeInterface Properties." <<Logger::endl;
            corba::CAttributeInterface::CPropertyNames_var props = attrs->getPropertyList();

            for (size_t i=0; i != props->length(); ++i) {
                if ( _value.find( std::string(props[i].name.in()) ) )
                    continue; // previously added.
                Logger::log() << Logger::Debug << "  Adding "<< string(props[i].name.in() ) <<Logger::endl;
                corba::CExpression_var expr = attrs->getProperty( props[i].name.in() );
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
                corba::CAssignableExpression_var as_expr = corba::CAssignableExpression::_narrow( expr.in() );
                if ( CORBA::is_nil( as_expr ) ) {
                    Logger::log() <<Logger::Error <<"Property "<< std::string(props[i].name.in()) << " was not writable !"<<Logger::endl;
                } else {
                    CORBA::String_var tn = as_expr->getTypeName();
                    TypeInfo* ti = TypeInfoRepository::Instance()->type( tn.in() );
                    Logger::log() <<Logger::Info << "Looking up Property " << tn.in();
                    if ( ti && ti->getProtocol(ORO_CORBA_PROTOCOL_ID) ) {
                        _value.add( ti->buildProperty( props[i].name.in(), props[i].description.in(),
                                                                            ti->getProtocol(ORO_CORBA_PROTOCOL_ID)->proxy( expr.in() ) ) );
                        Logger::log() <<Logger::Info<<" found!"<<Logger::endl;
                    }
                    else {
                        _value.add( new Property<CORBA::Any_ptr>( string(props[i].name.in()), string(props[i].description.in()), new corba::CORBAAssignableExpression<Property<CORBA::Any_ptr>::DataSourceType>( as_expr.in() ) ) );
                        Logger::log() <<Logger::Info<<" not found :-("<<Logger::endl;
                    }
                }
            }
            return true;
        }
        Logger::log() << Logger::Debug << "Failed to populate PropertyBag with CAttributeInterface Properties." <<Logger::endl;
        return false;
    }

    CORBA::Any_ptr AnyConversion<PropertyBag>::createAny( StdType t ) {
        Logger::log() << Logger::Debug << "Creating PropertyBag Server." <<Logger::endl;
        Orocos_CAttributeInterface_i* attrs = new Orocos_CAttributeInterface_i( new PropertyBag(t), corba::ControlTaskProxy::ProxyPOA() );
        corba::CAttributeInterface_ptr server = attrs->_this();
        CORBA::Any_ptr any = new CORBA::Any();
        *any <<= server;
        return any;
    }


};
