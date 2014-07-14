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

#ifndef STD_VECTOR_TEMPLATE_TYPE_INFO_HPP
#define STD_VECTOR_TEMPLATE_TYPE_INFO_HPP

#include "../Property.hpp"
#include "../PropertyBag.hpp"
#include "SequenceTypeInfo.hpp"
#include "Types.hpp"
#include "../Logger.hpp"
#include "../internal/DataSources.hpp"
#include <vector>
#include "VectorTemplateComposition.hpp"

namespace RTT
{ 
    namespace types {

        /**
         * Template class for std::vector<T> typekits, providing a alternative
         * demarshalling function for properties and optionally ostream printing.
         */
        template <typename T, bool has_ostream = false>
        struct StdVectorTemplateTypeInfo
            : public SequenceTypeInfo<std::vector<T>, has_ostream>
        {
            StdVectorTemplateTypeInfo<T, has_ostream>( std::string name )
            : SequenceTypeInfo<std::vector<T>, has_ostream >(name)
            {
            }

            //!Override default in order to take legacy formats into account.
            virtual bool composeType( base::DataSourceBase::shared_ptr dssource, base::DataSourceBase::shared_ptr dsresult) const
            {
                const internal::DataSource<PropertyBag>* pb = dynamic_cast< const internal::DataSource<PropertyBag>* > (dssource.get() );
                if ( !pb )
                    return false;
                internal::AssignableDataSource<std::vector<T> >::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<std::vector<T> > >( dsresult );
                if ( !ads )
                    return false;

                PropertyBag const& source = pb->rvalue();
                internal::AssignableDataSource<std::vector<T> >::reference_t result = ads->set();

                return composeTemplateProperty(source, result) || SequenceTypeInfo<std::vector<T>, has_ostream>::composeType(dssource, dsresult);
            }

        };
    }
}

#endif

