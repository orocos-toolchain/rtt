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
#include "rtt/Property.hpp"
#include "rtt/PropertyIntrospection.hpp"

namespace RTT {

#ifndef ORO_EMBEDDED
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
#endif

    template<>
    bool Property<PropertyBag>::update( const Property<PropertyBag>& orig)
    {
        if ( !ready() )
            return false;
        if ( _description.empty() )
            _description = orig.getDescription();
        return updateProperties( this->_value->set(), orig.rvalue() );
    }

    template<>
    bool Property<PropertyBag>::refresh( const Property<PropertyBag>& orig)
    {
        if ( !ready() )
            return false;
        return refreshProperties( this->_value->set(), orig.rvalue() );
    }

    template<>
    bool Property<PropertyBag>::copy( const Property<PropertyBag>& orig)
    {
        if ( !ready() )
            return false;
        _name = orig.getName();
        _description = orig.getDescription();
        return copyProperties( this->_value->set(), orig.rvalue() );
    }


    void PropertyBase::identify( PropertyBagVisitor* pbi)
    {
        pbi->introspect( this );
    }

    template<>
    void Property<PropertyBag>::identify( PropertyBagVisitor* pbi)
    {
        pbi->introspect( *this );
    }

}
