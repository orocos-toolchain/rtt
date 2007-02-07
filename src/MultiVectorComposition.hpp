/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  MultiVectorComposition.hpp 

                        MultiVectorComposition.hpp -  description
                           -------------------
    begin                : Sat May 21 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
 
 
#ifndef MULTIVECTOR_COMPOSITION_HPP
#define MULTIVECTOR_COMPOSITION_HPP

#include "MultiVector.hpp"
#include "Property.hpp"
#include "PropertyBag.hpp"
#include "Logger.hpp"

namespace RTT
{
    class PropertyIntrospection;
    class PropertyBag;
    template<class T>
    class Property;

    /**
     * A decomposeProperty method for decomposing a Property< MultiVector<S,T> >
     * into a PropertyBag with Property<T>'s.
     */
    template<class T, int S>
    void decomposeProperty(PropertyIntrospection *pi, const Property< MultiVector<S, T> >& c)
    {
        Property<PropertyBag> result(c.getName(),c.getDescription(), PropertyBag("MultiVector") );

        MultiVector<S,T> vec = c;
        Property<int>* dimension = new Property<int>("Size","Size of the MultiVector", vec.size() );

        result.value().add( dimension );

        std::stringstream data_name;

        for ( int i=0; i < dimension->get() ; i++)
            {
                data_name  << i;
                result.value().add( new Property<T>(data_name.str(),"",vec[i]) ); // Put variables in the bag
                data_name.str("");
            }

        pi->introspect(result); // introspect the bag.
        deleteProperties( result.value() );

    }

    template<class T, int S>
    void decomposeProperty(PropertyIntrospection *pi, const Property< const MultiVector<S, T>& >& c)
    {
        Property<PropertyBag> result(c.getName(),c.getDescription(), PropertyBag("MultiVector") );

        MultiVector<S,T> vec = c;
        Property<int>* dimension = new Property<int>("Size","Size of the MultiVector", vec.size() );

        result.value().add( dimension );

        std::stringstream data_name;

        for ( int i=0; i < dimension->get() ; i++)
            {
                data_name  << i;
                result.value().add( new Property<T>(data_name.str(),"",vec[i]) ); // Put variables in the bag
                data_name.str("");
            }

        pi->introspect(result); // introspect the bag.
        deleteProperties( result.value() );

    }

    /**
     * A composeProperty method for composing a property of a MultiVector<S, T>
     */
    template<class T, int S>
    bool composeProperty(const PropertyBag& bag, Property<MultiVector<S,T> >& result)
    {
        PropertyBase* v_base = bag.find( result.getName() );
        if ( v_base == 0 )
            return false;

        Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

        if (v_bag != 0 && v_bag->get().getType() == "MultiVector")
            {
                Property<T>* comp;

                Property<int>* dim;
                v_base = v_bag->get().find("Size");
                if ( v_base == 0 ) {
                    Logger::log() << Logger::Error << "In PropertyBag for Property< MultiVector<S,T> > :"
                                  << result.getName() << " : could not find property \"Size\"."<<Logger::endl;
                    return false;
                }
                dim = dynamic_cast< Property<int>* >(v_base);
                if ( dim == 0) {
                    Logger::log() << Logger::Error << "In PropertyBag for Property< MultiVector<S,T> > :"
                                  << result.getName() << " : Expected \"Size\" to be of type short."<<Logger::endl;
                    return false;
                }
                int dimension = dim->get();

                std::stringstream data_name;

                // Get values
                for (int i = 0; i < dimension ; i++)
                    {
                        data_name  << i;
                        PropertyBase* element = v_bag->get().find( data_name.str() );
                        if ( element == 0 ) {
                            Logger::log() << Logger::Error << "Aborting composition of Property< MultiVector<S,T> > "<<result.getName()
                                          << ": Data element "<< data_name.str() <<" not found !"
                                          <<Logger::endl;
                            return false;
                        }
                        comp = dynamic_cast< Property<T>* >( element );
                        if ( comp == 0 ) {
                            DataSourceBase::shared_ptr ds = element->getDataSource();
                            Logger::log() << Logger::Error << "Aborting composition of Property< MultiVector<S,T> > "<<result.getName()
                                          << ": Exptected data element "<< data_name.str() << " to be of type "<<DataSource<T>::GetType()
                                          <<" got type " << element->getType()
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
                    Logger::log() << Logger::Error << "Composing Property< MultiVector<S,T> > :"
                                  << result.getName() << " : type mismatch, got type '"<< v_bag->get().getType()  <<"'"<<Logger::endl;
                } else {
                    Logger::log() << Logger::Error << "Composing Property< MultiVector<S,T> > :"
                                  << result.getName() << " : not a PropertyBag."<<Logger::endl;
                }
                // cerr << "\033[1;33mWarning: Bag was empty! \033[0m" << endl;
                Logger::log() << Logger::Debug << "Could not update Property< MultiVector<S,T> > : "<<result.getName()<<Logger::endl;
                return false;
            }
        return true;

    }

}; // namespace RTT


#endif
