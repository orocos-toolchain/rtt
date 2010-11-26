#include "Types.hpp"
#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include "../Attribute.hpp"
#include "../Property.hpp"

template class RTT::OutputPort< std::vector<double> >;
template class RTT::InputPort< std::vector<double> >;
template class RTT::Property< std::vector<double> >;
template class RTT::Attribute< std::vector<double> >;

template class RTT::internal::DataSource< std::vector<double> >;
template class RTT::internal::ValueDataSource< std::vector<double> >;
template class RTT::internal::ConstantDataSource< std::vector<double> >;
template class RTT::internal::AssignableDataSource< std::vector<double> >;
template class RTT::internal::ReferenceDataSource< std::vector<double> >;

template class RTT::OutputPort< RTT::ConnPolicy >;
template class RTT::InputPort< RTT::ConnPolicy >;
template class RTT::Property< RTT::ConnPolicy >;
template class RTT::Attribute< RTT::ConnPolicy >;

template class RTT::internal::DataSource< RTT::ConnPolicy >;
template class RTT::internal::ValueDataSource< RTT::ConnPolicy >;
template class RTT::internal::ConstantDataSource< RTT::ConnPolicy >;
template class RTT::internal::AssignableDataSource< RTT::ConnPolicy >;
template class RTT::internal::ReferenceDataSource< RTT::ConnPolicy >;

template class RTT::OutputPort< RTT::FlowStatus >;
template class RTT::InputPort< RTT::FlowStatus >;
template class RTT::Property< RTT::FlowStatus >;
template class RTT::Attribute< RTT::FlowStatus >;

template class RTT::internal::DataSource< RTT::FlowStatus >;
template class RTT::internal::ValueDataSource< RTT::FlowStatus >;
template class RTT::internal::ConstantDataSource< RTT::FlowStatus >;
template class RTT::internal::AssignableDataSource< RTT::FlowStatus >;
template class RTT::internal::ReferenceDataSource< RTT::FlowStatus >;

template class RTT::OutputPort< RTT::SendStatus >;
template class RTT::InputPort< RTT::SendStatus >;
template class RTT::Property< RTT::SendStatus >;
template class RTT::Attribute< RTT::SendStatus >;

template class RTT::internal::DataSource< RTT::SendStatus >;
template class RTT::internal::ValueDataSource< RTT::SendStatus >;
template class RTT::internal::ConstantDataSource< RTT::SendStatus >;
template class RTT::internal::AssignableDataSource< RTT::SendStatus >;
template class RTT::internal::ReferenceDataSource< RTT::SendStatus >;

template class RTT::OutputPort< RTT::PropertyBag >;
template class RTT::InputPort< RTT::PropertyBag >;
template class RTT::Property< RTT::PropertyBag >;
template class RTT::Attribute< RTT::PropertyBag >;

template class RTT::internal::DataSource< RTT::PropertyBag >;
template class RTT::internal::ValueDataSource< RTT::PropertyBag >;
template class RTT::internal::ConstantDataSource< RTT::PropertyBag >;
template class RTT::internal::AssignableDataSource< RTT::PropertyBag >;
template class RTT::internal::ReferenceDataSource< RTT::PropertyBag >;
