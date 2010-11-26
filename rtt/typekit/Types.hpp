/**
 * @file Types.hpp
 * This file contains a series of 'extern template' definitions of the
 * Orocos RTT pre-defined types. You should include this header (or RTT.hpp
 * which includes this header as well) in your components in case you use
 * any of these types.
 */

#ifndef RTT_TYPEKIT_TYPES
//#define RTT_TYPEKIT_TYPES

#include <string>
#include <vector>
#include "../ConnPolicy.hpp"
#include "../SendStatus.hpp"
#include "../FlowStatus.hpp"
#include "../PropertyBag.hpp"

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< int >;
    extern template class RTT::internal::AssignableDataSource< int >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< int >;
    extern template class RTT::internal::ConstantDataSource< int >;
    extern template class RTT::internal::ReferenceDataSource< int >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< int >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< int >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< int >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< int >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< bool >;
    extern template class RTT::internal::AssignableDataSource< bool >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< bool >;
    extern template class RTT::internal::ConstantDataSource< bool >;
    extern template class RTT::internal::ReferenceDataSource< bool >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< bool >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< bool >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< bool >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< bool >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< unsigned int >;
    extern template class RTT::internal::AssignableDataSource< unsigned int >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< unsigned int >;
    extern template class RTT::internal::ConstantDataSource< unsigned int >;
    extern template class RTT::internal::ReferenceDataSource< unsigned int >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< unsigned int >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< unsigned int >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< unsigned int >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< unsigned int >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< double >;
    extern template class RTT::internal::AssignableDataSource< double >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< double >;
    extern template class RTT::internal::ConstantDataSource< double >;
    extern template class RTT::internal::ReferenceDataSource< double >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< double >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< double >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< double >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< double >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< float >;
    extern template class RTT::internal::AssignableDataSource< float >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< float >;
    extern template class RTT::internal::ConstantDataSource< float >;
    extern template class RTT::internal::ReferenceDataSource< float >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< float >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< float >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< float >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< float >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< char >;
    extern template class RTT::internal::AssignableDataSource< char >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< char >;
    extern template class RTT::internal::ConstantDataSource< char >;
    extern template class RTT::internal::ReferenceDataSource< char >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< char >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< char >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< char >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< char >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< std::string >;
    extern template class RTT::internal::AssignableDataSource< std::string >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< std::string >;
    extern template class RTT::internal::ConstantDataSource< std::string >;
    extern template class RTT::internal::ReferenceDataSource< std::string >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< std::string >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< std::string >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< std::string >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< std::string >;
#endif

#ifdef CORELIB_DATASOURCE_HPP
    extern template class RTT::internal::DataSource< std::vector<double> >;
    extern template class RTT::internal::AssignableDataSource< std::vector<double> >;
#endif
#ifdef ORO_CORELIB_DATASOURCES_HPP
    extern template class RTT::internal::ValueDataSource< std::vector<double> >;
    extern template class RTT::internal::ConstantDataSource< std::vector<double> >;
    extern template class RTT::internal::ReferenceDataSource< std::vector<double> >;
#endif
#ifdef ORO_INPUT_PORT_HPP
    extern template class RTT::OutputPort< std::vector<double> >;
#endif
#ifdef ORO_OUTPUT_PORT_HPP
    extern template class RTT::InputPort< std::vector<double> >;
#endif
#ifdef ORO_PROPERTY_HPP
    extern template class RTT::Property< std::vector<double> >;
#endif
#ifdef ORO_CORELIB_ATTRIBUTE_HPP
    extern template class RTT::Attribute< std::vector<double> >;
#endif

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
