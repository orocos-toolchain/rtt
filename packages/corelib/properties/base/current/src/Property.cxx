/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:02 CEST 2004  Property.cxx 

                        Property.cxx -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "corelib/Property.hpp"
#include "corelib/PropertyIntrospection.hpp"

namespace ORO_CoreLib {

     template class Property<double>;
     template class Property<bool>;
     template class Property<float>;
     template class Property<int>;
     template class Property<unsigned int>;
     template class Property<std::string>;
     template class Property<const std::string&>;
#ifdef OROINT_OS_CORBA
     template class Property<CORBA::Any>;
#endif

    template<>
    bool Property<PropertyBag>::update( const Property<PropertyBag>& orig)
    {
        if ( _description.empty() )
            _description = orig.getDescription();
        return updateProperties( this->_value->set(), orig.rvalue() );
    }

    template<>
    bool Property<PropertyBag>::refresh( const Property<PropertyBag>& orig)
    {
        return refreshProperties( this->_value->set(), orig.rvalue() );
    }

    template<>
    bool Property<PropertyBag>::copy( const Property<PropertyBag>& orig)
    {
        _name = orig.getName();
        _description = orig.getDescription();
        return copyProperties( this->_value->set(), orig.rvalue() );
    }



#if 0
    Property<PropertyBag>::~Property<PropertyBag>()
    {}

    Property<PropertyBag>* Property<PropertyBag>::narrow( PropertyBase* prop ) {
        Property<PropertyBag>* res = dynamic_cast<Property<PropertyBag>*>( prop );
        if (res)
            return res->clone();
#ifdef OROINT_OS_CORBA
        if ( prop->getDataSource()->hasServer() ) {
            PropertyBag result;
            CORBA::Any_var any = prop->getDataSource()->getAny();
            if( AnyConversion<PropertyBag>::update( any.in() , result ) ) {
                return new Property<PropertyBag>( prop->getName(), prop->getDescription(), result );
            }
        } 
#endif
        return 0;
    }


    void Property<PropertyBag>::identify( PropertyIntrospection* pi) const
    {
        pi->introspect( *this );
    }

    void decomposeProperty(ORO_CoreLib::PropertyIntrospection* pi, 
                           ORO_CoreLib::Property<const std::string&> const& nt) {
        Property<std::string> msg(nt.getName(), nt.getDescription(), nt.get() );
        pi->introspect( msg );
    }

    void decomposeProperty(ORO_CoreLib::PropertyIntrospection* pi, 
                           ORO_CoreLib::Property<float> const& f) {
        Property<double> msg(f.getName(), f.getDescription(), f.get() );
        pi->introspect( msg );
    }
#endif
}
