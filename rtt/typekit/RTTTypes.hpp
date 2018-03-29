/***************************************************************************
  tag: Peter Soetens Wed Jan 26 15:36:13 2011 +0100 RTTTypes.hpp

                        RTTTypes.hpp -  description
                           -------------------
    begin                : Wed Jan 26 2011
    copyright            : (C) 2011 Peter Soetens
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


/**
 * @file RTTTypes.hpp
 * This file contains a series of 'extern template' definitions of the
 * Orocos RTT classes only. It does not contain any 'standard C' types.
 */

#ifndef RTT_PURE_TYPEKIT_TYPES_HPP

#include "../ConnPolicy.hpp"
#include "../SendStatus.hpp"
#include "../FlowStatus.hpp"
#include "../PropertyBag.hpp"
#include "rtt-typekit-config.h"

// Disable extern template warning on MSVC
#if !defined( __MINGW__ ) && defined( WIN32 )
# pragma warning( disable : 4231 )
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< RTT::FlowStatus >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< RTT::SendStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< RTT::SendStatus >;
#endif

#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< RTT::FlowStatus >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< RTT::FlowStatus >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< RTT::FlowStatus >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< RTT::FlowStatus >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< RTT::FlowStatus >;
#endif

#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< RTT::SendStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< RTT::SendStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< RTT::SendStatus >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< RTT::SendStatus >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< RTT::SendStatus >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< RTT::SendStatus >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< RTT::SendStatus >;
#endif

#if 0
#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< RTT::PropertyBag >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< RTT::PropertyBag >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< RTT::PropertyBag >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< RTT::PropertyBag >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< RTT::PropertyBag >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< RTT::PropertyBag >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< RTT::PropertyBag >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< RTT::PropertyBag >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< RTT::PropertyBag >;
#endif
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< RTT::ConnPolicy >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< RTT::ConnPolicy >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< RTT::ConnPolicy >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< RTT::ConnPolicy >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< RTT::ConnPolicy >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< RTT::ConnPolicy >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< RTT::ConnPolicy >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< RTT::ConnPolicy >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< RTT::ConnPolicy >;
#endif


#endif
