#include "device_interface/DigitalInInterface.hpp"
#include "device_interface/DigitalOutInterface.hpp"

using namespace ORO_DeviceInterface;

// REMARK Analog In and Out are declared in the headerfiles

NameServer<DigitalInInterface*> DigitalInInterface::nameserver;
NameServer<DigitalOutInterface*> DigitalOutInterface::nameserver;
