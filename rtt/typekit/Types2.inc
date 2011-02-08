#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include "../Attribute.hpp"
#include "../Property.hpp"

template class RTT_EXPORT RTT::OutputPort< std::vector<double> >;
template class RTT_EXPORT RTT::InputPort< std::vector<double> >;
template class RTT_EXPORT RTT::Property< std::vector<double> >;
template class RTT_EXPORT RTT::Attribute< std::vector<double> >;

template class RTT_EXPORT RTT::internal::DataSource< std::vector<double> >;
template class RTT_EXPORT RTT::internal::ValueDataSource< std::vector<double> >;
template class RTT_EXPORT RTT::internal::ConstantDataSource< std::vector<double> >;
template class RTT_EXPORT RTT::internal::AssignableDataSource< std::vector<double> >;
template class RTT_EXPORT RTT::internal::ReferenceDataSource< std::vector<double> >;

template class RTT_EXPORT RTT::OutputPort< RTT::ConnPolicy >;
template class RTT_EXPORT RTT::InputPort< RTT::ConnPolicy >;
template class RTT_EXPORT RTT::Property< RTT::ConnPolicy >;
template class RTT_EXPORT RTT::Attribute< RTT::ConnPolicy >;

template class RTT_EXPORT RTT::internal::DataSource< RTT::ConnPolicy >;
template class RTT_EXPORT RTT::internal::ValueDataSource< RTT::ConnPolicy >;
template class RTT_EXPORT RTT::internal::ConstantDataSource< RTT::ConnPolicy >;
template class RTT_EXPORT RTT::internal::AssignableDataSource< RTT::ConnPolicy >;
template class RTT_EXPORT RTT::internal::ReferenceDataSource< RTT::ConnPolicy >;

template class RTT_EXPORT RTT::OutputPort< RTT::FlowStatus >;
template class RTT_EXPORT RTT::InputPort< RTT::FlowStatus >;
template class RTT_EXPORT RTT::Property< RTT::FlowStatus >;
template class RTT_EXPORT RTT::Attribute< RTT::FlowStatus >;

template class RTT_EXPORT RTT::internal::DataSource< RTT::FlowStatus >;
template class RTT_EXPORT RTT::internal::ValueDataSource< RTT::FlowStatus >;
template class RTT_EXPORT RTT::internal::ConstantDataSource< RTT::FlowStatus >;
template class RTT_EXPORT RTT::internal::AssignableDataSource< RTT::FlowStatus >;
template class RTT_EXPORT RTT::internal::ReferenceDataSource< RTT::FlowStatus >;

template class RTT_EXPORT RTT::OutputPort< RTT::SendStatus >;
template class RTT_EXPORT RTT::InputPort< RTT::SendStatus >;
template class RTT_EXPORT RTT::Property< RTT::SendStatus >;
template class RTT_EXPORT RTT::Attribute< RTT::SendStatus >;

template class RTT_EXPORT RTT::internal::DataSource< RTT::SendStatus >;
template class RTT_EXPORT RTT::internal::ValueDataSource< RTT::SendStatus >;
template class RTT_EXPORT RTT::internal::ConstantDataSource< RTT::SendStatus >;
template class RTT_EXPORT RTT::internal::AssignableDataSource< RTT::SendStatus >;
template class RTT_EXPORT RTT::internal::ReferenceDataSource< RTT::SendStatus >;

template class RTT_EXPORT RTT::OutputPort< RTT::PropertyBag >;
template class RTT_EXPORT RTT::InputPort< RTT::PropertyBag >;
template class RTT_EXPORT RTT::Property< RTT::PropertyBag >;
template class RTT_EXPORT RTT::Attribute< RTT::PropertyBag >;

template class RTT_EXPORT RTT::internal::DataSource< RTT::PropertyBag >;
template class RTT_EXPORT RTT::internal::ValueDataSource< RTT::PropertyBag >;
template class RTT_EXPORT RTT::internal::ConstantDataSource< RTT::PropertyBag >;
template class RTT_EXPORT RTT::internal::AssignableDataSource< RTT::PropertyBag >;
template class RTT_EXPORT RTT::internal::ReferenceDataSource< RTT::PropertyBag >;

