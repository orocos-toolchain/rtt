/***************************************************************************
  tag: Peter Soetens Fri Nov 26 16:10:18 2010 +0100 Types.hpp

                        Types.hpp -  description
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


/**
 * @file Types.hpp
 * This file contains a series of 'extern template' definitions of the
 * Orocos RTT pre-defined types. You should include this header in your components in case you use
 * any of these types. You then also need to link with the rtt-typekit library.
 */

#ifndef RTT_TYPEKIT_TYPES
//#define RTT_TYPEKIT_TYPES

#include "rtt-typekit-config.h"
#include "RTTTypes.hpp"
#include <rtt/rt_string.hpp>
#include <string>
#include <vector>

// Disable extern template warning on MSVC
#if !defined( __MINGW__ ) && defined( WIN32 )
# pragma warning( disable : 4231 )
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< int >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< int >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< int >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< int >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< int >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< int >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< int >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< int >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< int >;
#endif

#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< bool >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< bool >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< bool >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< bool >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< unsigned int >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< unsigned int >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< unsigned int >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< unsigned int >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< unsigned int >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< unsigned int >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< unsigned int >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< unsigned int >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< unsigned int >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< double >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< double >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< double >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< double >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< double >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< double >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< double >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< double >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< double >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< float >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< float >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< float >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< float >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< float >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< float >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< float >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< float >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< float >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< char >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< char >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< char >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< char >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< char >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< char >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< char >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< char >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< char >;
#endif


#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< std::string >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< std::string >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< std::string >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< std::string >;
#endif


#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< std::vector<double> >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< std::vector<double> >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< std::vector<double> >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< std::vector<double> >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< std::vector<double> >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< std::vector<double> >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< std::vector<double> >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< std::vector<double> >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< std::vector<double> >;
#endif


#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< RTT::rt_string >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< RTT::rt_string >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< RTT::rt_string >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< RTT::rt_string >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< RTT::rt_string >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< RTT::rt_string >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< RTT::rt_string >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< RTT::rt_string >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< RTT::rt_string >;
#endif


#endif
