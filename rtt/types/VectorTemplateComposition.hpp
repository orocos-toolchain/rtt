/***************************************************************************
  tag: Tinne De Laet 2007  VectorTemplateComposition.hpp
       2007 Ruben Smits
                        VectorTemplateComposition.hpp -  description

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

#ifndef VECTOR_TEMPLATE_COMPOSITION_HPP
#define VECTOR_TEMPLATE_COMPOSITION_HPP

#include "../Property.hpp"
#include "../PropertyBag.hpp"
#include "Types.hpp"
#include "../Logger.hpp"
#include "../internal/DataSources.hpp"
#include <vector>

namespace RTT
{ 
    namespace types {

        template<class T>
        bool composeTemplateProperty(const PropertyBag& bag, T& result)
        {
            TypeInfoRepository::shared_ptr tir = Types();
            if ( tir->type( bag.getType()) == tir->getTypeInfo< T >() ) {
                Property< typename T::value_type>* comp;
                int dimension = bag.size();
                result.resize( dimension );

                // Get values
                int size_correction = 0;
                for (int i = 0; i < dimension ; i++) {
                    base::PropertyBase* element = bag.getItem( i );
                    comp = dynamic_cast< Property< typename T::value_type>* >( element );
                    if ( comp == 0 ) {
                        // detect LEGACY element:
                        if ( element->getName() == "Size" ) {
                            // oops, our result vector will be one smaller.
                            size_correction += 1;
                            continue;
                        }
                        Logger::log() << Logger::Error << "Aborting composition of Property< T > "
                                      << ": Exptected data element "<< i << " to be of type " << internal::DataSourceTypeInfo< typename T::value_type>::getTypeName()
                                      <<" got type " << element->getType()
                                      <<Logger::endl;
                        return false;
                    }
                    result[ i - size_correction ] = comp->get();
                }
                result.resize( dimension - size_correction );
            }
            else {
                Logger::log() << Logger::Error << "Composing Property< T > :"
                              << " type mismatch, got type '"<< bag.getType()
                              << "', expected 'vector<" <<  internal::DataSourceTypeInfo< typename T::value_type>::getTypeName() <<">'."<<Logger::endl;
                return false;
            }
            return true;
        }
    }
}

#endif

