#include "device_interface/EncoderIncrementalInterface.hpp"
#include "device_interface/EncoderAbsoluteInterface.hpp"

using namespace ORO_DeviceInterface;

NameServer<EncoderIncrementalInterface*> EncoderIncrementalInterface::nameserver;
NameServer<EncoderAbsoluteInterface*> EncoderAbsoluteInterface::nameserver;
