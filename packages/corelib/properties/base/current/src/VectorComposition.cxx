/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  VectorComposition.cxx 

                        VectorComposition.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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


#include "corelib/Property.hpp"
#include "corelib/PropertyBag.hpp"
#include "corelib/PropertyIntrospection.hpp"
#include "corelib/VectorComposition.hpp"
#include "corelib/Logger.hpp"
#include <iostream>

namespace ORO_CoreLib
{

  // A decomposeProperty method for decomposing a Property< vector<double> >
  // into a PropertyBag with Property<double>'s.
  // The dimension of the vector must be less than 100 if you want the
  // Property<double>'s to have a different name.
  void decomposeProperty(PropertyIntrospection *pi, const Property< std::vector<double> >& c)
  {
    Property<PropertyBag> result(c.getName(),c.getDescription(), PropertyBag("std::vector<double>") );

    std::vector<double> vec = c.get();
    Property<int>* dimension = new Property<int>("dim","Dimension of the Vector", vec.size() );

    result.value().add( dimension );

    std::string data_name = "d00";

    for ( int i=0; i < dimension->get() ; i++)
    {
        result.value().add( new Property<double>(data_name,"",vec[i]) ); // Put variables in the bag

        // change last number of string
        if(i < 100)
        {
            if( data_name[2] == '9')
            {
                data_name[2] = '0';
                data_name[1] += 1;
            }
            else
            {
                data_name[2] += 1;
            }
        }
    }

    pi->introspect(result); // introspect the bag.
    deleteProperties( result.value() );
  }

  void decomposeProperty(PropertyIntrospection *pi, const Property<const std::vector<double>& >& c)
  {
    Property<PropertyBag> result(c.getName(),"std::vector<double>", PropertyBag("std::vector<double>") );

    std::vector<double> vec = c.get();
    Property<int>* dimension = new Property<int>("dim","Dimension of the Vector", vec.size() );

    result.value().add( dimension );

    std::string data_name = "d00";

    for ( int i=0; i < dimension->get() ; i++)
    {
        result.value().add( new Property<double>(data_name,"",vec[i]) ); // Put variables in the bag

        // change last number of string
        if(i < 100)
        {
            if( data_name[2] == '9')
            {
                data_name[2] = '0';
                data_name[1] += 1;
            }
            else
            {
                data_name[2] += 1;
            }
        }
    }

    pi->introspect(result); // introspect the bag.
    deleteProperties( result.value() );
  }
    


  // A composeProperty method for composing a property of a vector<double>
  // The dimension of the vector must be less than 100.

  bool composeProperty(const PropertyBag& bag, Property<std::vector<double> >& result)
  {
    PropertyBase* v_base = bag.find( result.getName() );
    Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

    if (v_bag != NULL && v_bag->get().getType() == "std::vector<double>")
    {
        std::string data_name = "d00";
        Property<double>* comp;

        Property<int>* dim;
        v_base = v_bag->get().find("dim");
        if ( v_base == 0 ) {
            Logger::log() << Logger::Error << "In PropertyBag for Property< std::vector<double> > :"
                          << result.getName() << " : could not find property \"dim\"."<<Logger::endl;
            return false;
        }
        dim = dynamic_cast< Property<int>* >(v_base);
        if ( dim == 0) {
            Logger::log() << Logger::Error << "In PropertyBag for Property< std::vector<double> > :"
                          << result.getName() << " : Expected \"dim\" to be of type short."<<Logger::endl;
            return false;
        }
        int dimension = dim->get();
        // cerr << dimension << endl;

        if (dimension > 99)
        {
	  // cerr << "\033[1;33mWarning: Setting dimension of vector to maximum for properties (99) \033[0m" << endl;
            dimension = 99;
        }

        // Set dimension
        result.value().resize(dimension);

        // Get values
        for (int i = 0; i < dimension ; i++)
        {
            comp = dynamic_cast< Property<double>* >(v_bag->get().find(data_name));
            result.value()[i] = comp->get();

            // change last number of string
            if( data_name[2] == '9')
            {
                data_name[2] = '0';
                data_name[1] += 1;
            }
            else
            {
                data_name[2] += 1;
            }
        }
    }
    else
    {
      // cerr << "\033[1;33mWarning: Bag was empty! \033[0m" << endl;
        Logger::log() << Logger::Warning << "Empty PropertyBag given for Property< std::vector<double> > : "<<result.getName()<<Logger::endl;
        return false;
    }
    return true;
  }


}; // namespace ORO_CoreLib

