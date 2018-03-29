/***************************************************************************
  tag: Peter Soetens Fri Nov 26 16:10:18 2010 +0100 Types.inc

                        Types.inc -  description
                           -------------------
    begin                : Fri Nov 26 2010
    copyright            : (C) 2010 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "../internal/DataSource.hpp"
#include "../internal/DataSources.hpp"

#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include "../Attribute.hpp"
#include "../Property.hpp"

  namespace RTT {
    template class RTT_EXPORT OutputPort< bool >; 
    template class RTT_EXPORT InputPort< bool >; 
    template class RTT_EXPORT Property< bool >; 
    template class RTT_EXPORT Attribute< bool >; 

    template class RTT_EXPORT OutputPort< std::string >;
    template class RTT_EXPORT InputPort< std::string >;
    template class RTT_EXPORT Property< std::string >;
    template class RTT_EXPORT Attribute< std::string >;
  }

#define RTT_EXPORT_TEMPLATE_TYPE( TYPE ) \
  namespace RTT { namespace internal { \
    template class RTT_EXPORT DataSource< TYPE >; \
    template class RTT_EXPORT AssignableDataSource< TYPE >; \
    template class RTT_EXPORT ValueDataSource< TYPE >; \
    template class RTT_EXPORT ConstantDataSource< TYPE >; \
    template class RTT_EXPORT ReferenceDataSource< TYPE >; \
    } \
    template class RTT_EXPORT OutputPort< TYPE >; \
    template class RTT_EXPORT InputPort< TYPE >; \
    template class RTT_EXPORT Property< TYPE >; \
    template class RTT_EXPORT Attribute< TYPE >; \
    }

RTT_EXPORT_TEMPLATE_TYPE(double)
RTT_EXPORT_TEMPLATE_TYPE(int)
RTT_EXPORT_TEMPLATE_TYPE(unsigned int)
RTT_EXPORT_TEMPLATE_TYPE(float)
RTT_EXPORT_TEMPLATE_TYPE(char)

#include <rtt/rt_string.hpp>

RTT_EXPORT_TEMPLATE_TYPE(RTT::rt_string)


