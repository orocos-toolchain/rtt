/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:04 CEST 2004  PropertyBagIntrospector.cxx

                        PropertyBagIntrospector.cxx -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be

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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "PropertyBagIntrospector.hpp"

using namespace RTT;
using namespace RTT::marsh;

PropertyBagIntrospector::PropertyBagIntrospector( PropertyBag& bag )
{
    mystack.push( &bag );
}

PropertyBagIntrospector::~PropertyBagIntrospector()
{
    mystack.pop();
}

/**
 * Reset to do a new introspection.
 * @param new_bag The new bag to save the results in.
 */
void PropertyBagIntrospector::reset( PropertyBag& new_bag)
{
    mystack.pop();
    mystack.push( &new_bag );
}


/**
 * Use this entry function to inspect a bag.
 * @param v the properties of this bag will be inspected.
 */
void PropertyBagIntrospector::introspect(const PropertyBag& v )
{
    v.identify(this);
}

void PropertyBagIntrospector::introspect(base::PropertyBase* v)
{
    // if it is decomposable, identify a new bag, otherwise add a clone.
    Property<PropertyBag> res(v->getName(), v->getDescription() );
    if ( types::typeDecomposition( v->getDataSource(), res.value() ))
        res.identify( this );
    else
        mystack.top()->add( v->clone() );
}

void PropertyBagIntrospector::introspect(Property<PropertyBag> &v)
{
    PropertyBag* cur_bag = mystack.top();
    Property<PropertyBag>* bag_cl
        = new Property<PropertyBag>( v.getName(),
                                     v.getDescription(),
                                     PropertyBag( v.get().getType() ) );
    cur_bag->add( bag_cl );

    mystack.push( &bag_cl->value() );

    v.value().identify(this);

    mystack.pop();
}
