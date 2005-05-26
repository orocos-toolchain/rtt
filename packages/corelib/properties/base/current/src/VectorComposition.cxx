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
    Property<int>* dimension = new Property<int>("Size","Dimension of the Vector", vec.size() );

    result.value().add( dimension );

    std::stringstream data_name;


    for ( int i=0; i < dimension->get() ; i++)
    {
        data_name  << i;
        result.value().add( new Property<double>(data_name.str(),"",vec[i]) ); // Put variables in the bag

        data_name.str("");
    }

    pi->introspect(result); // introspect the bag.
    deleteProperties( result.value() );
  }

  void decomposeProperty(PropertyIntrospection *pi, const Property<const std::vector<double>& >& c)
  {
    Property<PropertyBag> result(c.getName(),c.getDescription(), PropertyBag("std::vector<double>") );

    std::vector<double> vec = c.get();
    Property<int>* dimension = new Property<int>("Size","Dimension of the Vector", vec.size() );

    result.value().add( dimension );

    std::stringstream data_name;

    for ( int i=0; i < dimension->get() ; i++)
    {
        data_name  << i;
        result.value().add( new Property<double>(data_name.str(),"",vec[i]) ); // Put variables in the bag

        data_name.str("");
    }

    pi->introspect(result); // introspect the bag.
    deleteProperties( result.value() );
  }
    


  // A composeProperty method for composing a property of a vector<double>
  // The dimension of the vector must be less than 100.

  bool composeProperty(const PropertyBag& bag, Property<std::vector<double> >& result)
  {
    PropertyBase* v_base = bag.find( result.getName() );
    if ( v_base == 0 )
        return false;

    Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

    if (v_bag != 0 && v_bag->get().getType() == "std::vector<double>")
    {
        Property<int>* dim;
        v_base = v_bag->get().find("Size");
        if ( v_base == 0 ) {
            Logger::log() << Logger::Error << "In PropertyBag for Property< std::vector<double> > :"
                          << result.getName() << " : could not find property \"Size\"."<<Logger::endl;
            return false;
        }
        dim = dynamic_cast< Property<int>* >(v_base);
        if ( dim == 0) {
            Logger::log() << Logger::Error << "In PropertyBag for Property< std::vector<double> > :"
                          << result.getName() << " : Expected \"Size\" to be of type short."<<Logger::endl;
            return false;
        }

        int dimension = dim->get();
        // cerr << dimension << endl;
        // Set dimension
        result.value().resize(dimension);

        Property<double>* comp;
        std::stringstream data_name;


        // Get values
        for (int i = 0; i < dimension ; i++)
        {
            data_name  << i;
            PropertyBase* element = v_bag->get().find(data_name.str());
            if ( element == 0 ) {
                Logger::log() << Logger::Error << "Aborting composition of Property< vector<double> > "<<result.getName()
                              << ": Data element "<< data_name.str() <<" not found !"
                              <<Logger::endl;
                return false;
            }
            comp = dynamic_cast< Property<double>* >( element );
            if ( comp == 0 ) {
                DataSourceBase::shared_ptr ds = element->createDataSource();
                Logger::log() << Logger::Error << "Aborting composition of Property< vector<double> > "<<result.getName()
                              << ": Exptected data element "<< data_name.str() << " to be of type double"
                              <<" got type " << ds->getType()
                              <<Logger::endl;
                return false;
            }
            result.value()[i] = comp->get();

            data_name.str("");
        }
    }
    else
    {
        if ( v_bag != 0 ) {
            Logger::log() << Logger::Error << "Composing Property< std::vector<double> > :"
                          << result.getName() << " : type mismatch, got type '"<< v_bag->get().getType()  <<"'"<<Logger::endl;
        } else {
            Logger::log() << Logger::Error << "Composing Property< std::vector<double> > :"
                          << result.getName() << " : not a PropertyBag."<<Logger::endl;
        }
      // cerr << "\033[1;33mWarning: Bag was empty! \033[0m" << endl;
        Logger::log() << Logger::Debug << "Could not update Property< std::vector<double> > : "<<result.getName()<<Logger::endl;
        return false;
    }
    return true;
  }


}; // namespace ORO_CoreLib

