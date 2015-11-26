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
RTT_EXPORT_TEMPLATE_TYPE(long long)
RTT_EXPORT_TEMPLATE_TYPE(unsigned long long)
RTT_EXPORT_TEMPLATE_TYPE(float)
RTT_EXPORT_TEMPLATE_TYPE(char)

#include <rtt/rt_string.hpp>

RTT_EXPORT_TEMPLATE_TYPE(RTT::rt_string)


