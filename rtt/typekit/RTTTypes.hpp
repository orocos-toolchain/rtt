/**
 * @file RTTTypes.hpp
 * This file contains a series of 'extern template' definitions of the
 * Orocos RTT classes only. It does not contain any 'standard C' types.
 */

#ifndef RTT_PURE_TYPEKIT_TYPES

#include "../ConnPolicy.hpp"
#include "../SendStatus.hpp"
#include "../FlowStatus.hpp"
#include "../PropertyBag.hpp"

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< RTT::FlowStatus >;
    extern template class RTT::internal::AssignableDataSource< RTT::FlowStatus >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< RTT::FlowStatus >;
    extern template class RTT::internal::ConstantDataSource< RTT::FlowStatus >;
    extern template class RTT::internal::ReferenceDataSource< RTT::FlowStatus >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< RTT::FlowStatus >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< RTT::FlowStatus >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< RTT::FlowStatus >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< RTT::FlowStatus >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< RTT::SendStatus >;
    extern template class RTT::internal::AssignableDataSource< RTT::SendStatus >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< RTT::SendStatus >;
    extern template class RTT::internal::ConstantDataSource< RTT::SendStatus >;
    extern template class RTT::internal::ReferenceDataSource< RTT::SendStatus >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< RTT::SendStatus >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< RTT::SendStatus >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< RTT::SendStatus >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< RTT::SendStatus >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< RTT::PropertyBag >;
    extern template class RTT::internal::AssignableDataSource< RTT::PropertyBag >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< RTT::PropertyBag >;
    extern template class RTT::internal::ConstantDataSource< RTT::PropertyBag >;
    extern template class RTT::internal::ReferenceDataSource< RTT::PropertyBag >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< RTT::PropertyBag >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< RTT::PropertyBag >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< RTT::PropertyBag >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< RTT::PropertyBag >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< RTT::ConnPolicy >;
    extern template class RTT::internal::AssignableDataSource< RTT::ConnPolicy >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< RTT::ConnPolicy >;
    extern template class RTT::internal::ConstantDataSource< RTT::ConnPolicy >;
    extern template class RTT::internal::ReferenceDataSource< RTT::ConnPolicy >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< RTT::ConnPolicy >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< RTT::ConnPolicy >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< RTT::ConnPolicy >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< RTT::ConnPolicy >;
#endif


#endif
