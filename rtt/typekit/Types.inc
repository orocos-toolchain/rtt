
// bool needs a special treatment, since the GCC compiler already
// instantiates it when it sees a line like DataSource<bool>::shared_ptr...
// So we need to put it before the ports/Operations code which contains lots of these.
#include "../internal/DataSource.hpp"
#include "../internal/DataSources.hpp"
  namespace RTT { namespace internal { 
    template class RTT_EXPORT DataSource< bool >; 
    template class RTT_EXPORT AssignableDataSource< bool >; 
    template class RTT_EXPORT ReferenceDataSource< bool >; 
    }}

#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include "../Attribute.hpp"
#include "../Property.hpp"

  namespace RTT { namespace internal { 
    template class RTT_EXPORT ValueDataSource< bool >; 
    template class RTT_EXPORT ConstantDataSource< bool >; 
    } 
    template class RTT_EXPORT OutputPort< bool >; 
    template class RTT_EXPORT InputPort< bool >; 
    template class RTT_EXPORT Property< bool >; 
    template class RTT_EXPORT Attribute< bool >; 
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

//RTT_EXPORT_TEMPLATE_TYPE(bool)
RTT_EXPORT_TEMPLATE_TYPE(double)
RTT_EXPORT_TEMPLATE_TYPE(int)
RTT_EXPORT_TEMPLATE_TYPE(unsigned int)
RTT_EXPORT_TEMPLATE_TYPE(float)
RTT_EXPORT_TEMPLATE_TYPE(char)
RTT_EXPORT_TEMPLATE_TYPE(std::string)

#include <rtt/rt_string.hpp>

template class std::basic_string<char, std::char_traits<char>, RTT::os::rt_allocator<char> >;
RTT_EXPORT_TEMPLATE_TYPE(RTT::rt_string)


