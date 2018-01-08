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
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< RTT::WriteStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< RTT::WriteStatus >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::DataSource< RTT::SendStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::AssignableDataSource< RTT::SendStatus >;
#endif

#ifdef ORO_CORELIB_DATASOURCES_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ValueDataSource< RTT::WriteStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ConstantDataSource< RTT::WriteStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::internal::ReferenceDataSource< RTT::WriteStatus >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::OutputPort< RTT::WriteStatus >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::InputPort< RTT::WriteStatus >;
#endif
#ifdef ORO_PROPERTY_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Property< RTT::WriteStatus >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< RTT::FlowStatus >;
    RTT_TYPEKIT_EXT_TMPL template class RTT_TYPEKIT_API RTT::Attribute< RTT::WriteStatus >;
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
