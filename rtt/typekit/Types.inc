#include "../internal/DataSources.hpp"

template class RTT_EXPORT RTT::internal::DataSource< std::string >;
template class RTT_EXPORT RTT::internal::AssignableDataSource< std::string >;
template class RTT_EXPORT RTT::internal::ValueDataSource< std::string >;
template class RTT_EXPORT RTT::internal::ConstantDataSource< std::string >;
template class RTT_EXPORT RTT::internal::ReferenceDataSource< std::string >;

#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include "../Attribute.hpp"
#include "../Property.hpp"


template class RTT_EXPORT RTT::OutputPort< std::string >;
template class RTT_EXPORT RTT::InputPort< std::string >;
template class RTT_EXPORT RTT::Property< std::string >;
template class RTT_EXPORT RTT::Attribute< std::string >;

#define RTT_EXPORT_TEMPLATE_TYPE( TYPE ) \
    template class RTT_EXPORT RTT::internal::DataSource< TYPE >; \
    template class RTT_EXPORT RTT::internal::AssignableDataSource< TYPE >; \
    template class RTT_EXPORT RTT::internal::ValueDataSource< TYPE >; \
    template class RTT_EXPORT RTT::internal::ConstantDataSource< TYPE >; \
    template class RTT_EXPORT RTT::internal::ReferenceDataSource< TYPE >; \
    template class RTT_EXPORT RTT::OutputPort< TYPE >; \
    template class RTT_EXPORT RTT::InputPort< TYPE >; \
    template class RTT_EXPORT RTT::Property< TYPE >; \
    template class RTT_EXPORT RTT::Attribute< TYPE >; \

RTT_EXPORT_TEMPLATE_TYPE(double)
RTT_EXPORT_TEMPLATE_TYPE(int)
RTT_EXPORT_TEMPLATE_TYPE(unsigned int)
RTT_EXPORT_TEMPLATE_TYPE(float)
RTT_EXPORT_TEMPLATE_TYPE(char)
RTT_EXPORT_TEMPLATE_TYPE(bool)
//RTT_EXPORT_TEMPLATE_TYPE(std::string)


