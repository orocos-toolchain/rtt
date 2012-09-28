#include "StreamFactory.hpp"
#include "../rtt-config.h"

using namespace std;
using namespace RTT;
using namespace RTT::detail;

#ifdef OS_HAVE_STREAMS
#include <sstream>
#endif

string StreamFactory::toString( DataSourceBase::shared_ptr in ) const
{
#ifdef OS_HAVE_STREAMS
    stringstream result;
    this->write( result, in );
    return result.str();
#else
    return string("(") + in->getTypeInfo()->getTypeName() + ")";
#endif
}

bool StreamFactory::fromString( const std::string& value, DataSourceBase::shared_ptr out ) const
{
    stringstream result(value);
    return this->read( result, out ).good();
}
