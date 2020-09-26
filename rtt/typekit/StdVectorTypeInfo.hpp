/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  StdVectorTypeInfo.hpp

                        StdVectorTypeInfo.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#include <vector>
#include "rtt/types/TypeStream.hpp"
#include "rtt/types/SequenceTypeInfo.hpp"
#include "rtt/types/VectorComposition.hpp"
#include "rtt/types/TemplateTypeInfo.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * This class tells Orocos how to handle std::vector<double>.
         */
        struct StdVectorTypeInfo: public SequenceTypeInfo<std::vector<double>, true>
        {
            StdVectorTypeInfo(std::string name) :
                SequenceTypeInfo<std::vector<double>, true> (name)
            {
            }

            //!Override default in order to take legacy formats into account.
            virtual bool composeType( base::DataSourceBase::shared_ptr dssource, base::DataSourceBase::shared_ptr dsresult) const
            {
                const internal::DataSource<PropertyBag>* pb = dynamic_cast< const internal::DataSource<PropertyBag>* > (dssource.get() );
                if ( !pb )
                    return false;
                internal::AssignableDataSource<std::vector<double> >::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<std::vector<double> > >( dsresult );
                if ( !ads )
                    return false;

                PropertyBag const& source = pb->rvalue();
                internal::AssignableDataSource<std::vector<double> >::reference_t result = ads->set();

                return composeProperty(source, result) || SequenceTypeInfo<std::vector<double>, true>::composeType(dssource, dsresult);
            }

        };
    }
}
