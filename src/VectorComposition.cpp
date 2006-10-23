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


#include "Property.hpp"
#include "PropertyBag.hpp"
#include "VectorComposition.hpp"
#include "Logger.hpp"
#include <ostream>
#include <sstream>

namespace RTT
{

    // A decomposeProperty method for decomposing a Property< vector<double> >
    // into a PropertyBag with Property<double>'s.
    // The dimension of the vector must be less than 100 if you want the
    // Property<double>'s to have a different name.
    void decomposeProperty(const std::vector<double>& vec, PropertyBag& targetbag)
    {
        targetbag.setType("std::vector<double>");
        Property<int>* dimension = new Property<int>("Size","Dimension of the Vector", vec.size() );

        targetbag.add( dimension );

        std::stringstream data_name;

        for ( int i=0; i < dimension->get() ; i++)
            {
                data_name  << i;
                targetbag.add( new Property<double>(data_name.str(),"",vec[i]) ); // Put variables in the bag
                data_name.str("");
            }
    }

    // A composeProperty method for composing a property of a vector<double>
    // The dimension of the vector must be less than 100.

    bool composeProperty(const PropertyBag& bag, std::vector<double>& result)
    {
        if ( bag.getType() == "std::vector<double>")
            {
                Property<int>* dim;
                PropertyBase* v_base = bag.find("Size");
                if ( v_base == 0 ) {
                    Logger::log() << Logger::Error << "In PropertyBag for Property< std::vector<double> > :"
                                  << " could not find property \"Size\"."<<Logger::endl;
                    return false;
                }
                dim = dynamic_cast< Property<int>* >(v_base);
                if ( dim == 0) {
                    Logger::log() << Logger::Error << "In PropertyBag for Property< std::vector<double> > :"
                                  << " Expected \"Size\" to be of type short."<<Logger::endl;
                    return false;
                }

                int dimension = dim->get();
                // cerr << dimension << endl;
                // Set dimension
                result.resize(dimension);

                Property<double>* comp;
                std::stringstream data_name;


                // Get values
                for (int i = 0; i < dimension ; i++)
                    {
                        data_name  << i;
                        PropertyBase* element = bag.find(data_name.str());
                        if ( element == 0 ) {
                            Logger::log() << Logger::Error << "Aborting composition of Property< vector<double> > "
                                          << ": Data element "<< data_name.str() <<" not found !"
                                          <<Logger::endl;
                            return false;
                        }
                        comp = dynamic_cast< Property<double>* >( element );
                        if ( comp == 0 ) {
                            Logger::log() << Logger::Error << "Aborting composition of Property< vector<double> > "
                                          << ": Exptected data element "<< data_name.str() << " to be of type double"
                                          <<" got type " << element->getType()
                                          <<Logger::endl;
                            return false;
                        }
                        result[i] = comp->get();

                        data_name.str("");
                    }
            }
        else
            {
                Logger::log() << Logger::Error << "Composing Property< std::vector<double> > :"
                              << " type mismatch, got type '"<< bag.getType()
                              << "', expected type 'std::vector<double>'."<<Logger::endl;
                return false;
            }
        return true;
    }


}; // namespace RTT

